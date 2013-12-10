#include <fstream>
#include <new>
#include <picojson.h>
#include "Application.h"
#include "GUIElementId.h"
#include "Irrlicht-i18n\CGUITTFont.h"

Application::Application()
{
	this->device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(WindowWidth, WindowHeight));
	this->device->setWindowCaption(L"Photo Viewer");
	this->device->setEventReceiver(this);

	this->LoadPhotos();

	this->largeFont = CGUITTFont::createTTFont(this->device, FontFile.c_str(), LargeFontSize);
	this->regularFont = CGUITTFont::createTTFont(this->device, FontFile.c_str(), RegularFontSize);

	auto driver = this->device->getVideoDriver();
	this->leftButtonImage = driver->getTexture(LeftButtonImageFile.c_str());
	this->rightButtonImage = driver->getTexture(RightButtonImageFile.c_str());
	this->closeButtonImage = driver->getTexture(CloseButtonImageFile.c_str());
}

Application::~Application()
{
	this->device->drop();
	this->largeFont->drop();
	this->regularFont->drop();
}

void Application::Run()
{
	while (this->device->run()) {
		if (this->displayChanged) {
			this->ConfigureGUIElements();
			this->displayChanged = false;
		}

		auto driver = device->getVideoDriver();
		driver->beginScene();
		this->DrawNonGUIElements();
		device->getGUIEnvironment()->drawAll();
		driver->endScene();
	}
}

bool Application::OnEvent(const SEvent& ev)
{
	if (ev.EventType != EET_GUI_EVENT) {
		return false;
	}

	auto id = ev.GUIEvent.Caller->getID();

	switch (ev.GUIEvent.EventType) {
	case EGET_BUTTON_CLICKED:
		switch (id) {
		case GUIElementId::ThumbnailLeftButton:
			{
				auto nextIdx = this->thumbnailIndex - NumberOfThumbnails;
				if (nextIdx >= 0) {
					this->thumbnailIndex = nextIdx;
					this->displayChanged = true;
				}
			}
			break;

		case GUIElementId::ThumbnailRightButton:
			{
				auto nextIdx = this->thumbnailIndex + NumberOfThumbnails;
				if (nextIdx < this->photos.size()) {
					this->thumbnailIndex = nextIdx;
					this->displayChanged = true;
				}
			}
			break;

		case GUIElementId::CloseButton:
			this->fullscreen = false;
			this->displayChanged = true;
			break;

		case GUIElementId::FullscreenLeftButton:
			{
				auto nextIdx = this->fullscreenIndex - 1;
				if (nextIdx >= 0) {
					this->fullscreenIndex = nextIdx;
					this->thumbnailIndex = this->fullscreenIndex / NumberOfThumbnails * NumberOfThumbnails;
					this->displayChanged = true;
				}
			}
			break;

		case GUIElementId::FullscreenRightButton:
			{
				auto nextIdx = this->fullscreenIndex + 1;
				if (nextIdx < this->photos.size()) {
					this->fullscreenIndex = nextIdx;
					this->thumbnailIndex = this->fullscreenIndex / NumberOfThumbnails * NumberOfThumbnails;
					this->displayChanged = true;
				}
			}
			break;

		default:
			this->fullscreenIndex = this->thumbnailIndex + id - GUIElementId::Thumbnail;
			this->fullscreen = true;
			this->displayChanged = true;
			break;
		}
		break;
	}
}

void Application::LoadPhotos()
{
	std::ifstream ifstr(MetadataFile);
	picojson::value value;
	ifstr >> value;

	auto entries = value.get<picojson::array>();
	this->photos.resize(entries.size());

	for (auto i = 0; i < entries.size(); i++) {
		auto entry = entries[i].get<picojson::object>();
		auto filename = entry["filename"].get<std::string>();
		auto caption = entry["caption"].get<std::string>();

		auto path = PhotosDirectory + "\\" + filename;
		auto data = this->device->getVideoDriver()->getTexture(path.c_str());

		new(&this->photos[i]) Photo(data, caption);
	}
}

void Application::ConfigureGUIElements()
{
	auto env = this->device->getGUIEnvironment();
	env->clear();

	auto buttonSize = this->closeButtonImage->getSize();

	if (this->fullscreen) {
		auto closeButtonPos = vector2d<s32>(WindowWidth - buttonSize.Width - FullscreenButtonMargin, FullscreenButtonMargin);
		auto closeButton = env->addButton(rect<s32>(closeButtonPos, buttonSize), nullptr, GUIElementId::CloseButton);
		closeButton->setImage(this->closeButtonImage);

		auto arrowButtonTop = (WindowHeight - buttonSize.Height) / 2;
		auto leftButtonPos = vector2d<s32>(FullscreenButtonMargin, arrowButtonTop);
		auto rightButtonPos = vector2d<s32>(WindowWidth - buttonSize.Width - FullscreenButtonMargin, arrowButtonTop);
		auto leftButton = env->addButton(rect<s32>(leftButtonPos, buttonSize), nullptr, GUIElementId::FullscreenLeftButton);
		auto rightButton = env->addButton(rect<s32>(rightButtonPos, buttonSize), nullptr, GUIElementId::FullscreenRightButton);
		leftButton->setImage(this->leftButtonImage);
		rightButton->setImage(this->rightButtonImage);
	} else {
		auto num = std::min((int)this->photos.size() - this->thumbnailIndex, NumberOfThumbnails);
		for (auto i = 0; i < num; i++) {
			auto rect = this->GetThumbnailRect(i);
			auto button = env->addButton(rect, nullptr, GUIElementId::Thumbnail + i);
			button->setImage(this->photos[this->thumbnailIndex + i].getData());
			button->setScaleImage(true);
		}

		auto arrowButtonTop = WindowHeight - buttonSize.Height - ThumbnailArrowButtonBottomMargin;
		auto leftButtonPos = vector2d<s32>(WindowWidth / 2 - buttonSize.Width - (ThumbnailArrowButtonDistance / 2), arrowButtonTop);
		auto rightButtonPos = vector2d<s32>(WindowWidth / 2 + (ThumbnailArrowButtonDistance / 2), arrowButtonTop);
		auto leftButton = env->addButton(rect<s32>(leftButtonPos, buttonSize), nullptr, GUIElementId::ThumbnailLeftButton);
		auto rightButton = env->addButton(rect<s32>(rightButtonPos, buttonSize), nullptr, GUIElementId::ThumbnailRightButton);
		leftButton->setImage(this->leftButtonImage);
		rightButton->setImage(this->rightButtonImage);
	}
}

rect<s32> Application::GetThumbnailRect(int idx)
{
	auto photoSize = this->photos[this->thumbnailIndex + idx].getData()->getSize();

	auto thumbnailSize = photoSize.Width >= photoSize.Height ?
		dimension2d<s32>(MaxThumbnailSize, photoSize.Height * MaxThumbnailSize / photoSize.Width) :
		dimension2d<s32>(photoSize.Width * MaxThumbnailSize / photoSize.Height, MaxThumbnailSize);

	const int OuterMargin = (WindowWidth - MaxThumbnailSize * NumberOfThumbnails
		- MinThumbnailMargin * (NumberOfThumbnails - 1)) / 2;
	auto thumbnailLeft = OuterMargin + (MaxThumbnailSize + MinThumbnailMargin) * idx
		+ (MaxThumbnailSize - thumbnailSize.Width) / 2;
	auto thumbnailTop = MinThumbnailTop + (MaxThumbnailSize - thumbnailSize.Height) / 2;

	return rect<s32>(vector2d<s32>(thumbnailLeft, thumbnailTop), thumbnailSize);
}

void Application::DrawNonGUIElements()
{
	if (this->fullscreen) {
		auto fullscreenRect = this->GetFullscreenRect();
		auto photoData = this->photos[this->fullscreenIndex].getData();
		auto sourceRect = rect<s32>(vector2d<s32>(0, 0), photoData->getSize());
		this->device->getVideoDriver()->draw2DImage(photoData, fullscreenRect, sourceRect);
	} else {
		auto captionRect = rect<s32>(vector2d<s32>(HeadingLeft, HeadingTop), dimension2d<s32>(HeadingWidth, HeadingHeight));
		this->largeFont->draw(L"Photos", captionRect, SColor(TextColor));
	}
}

rect<s32> Application::GetFullscreenRect()
{
	auto buttonSize = this->closeButtonImage->getSize();
	auto fullscreenWidth = WindowWidth - (buttonSize.Width + FullscreenButtonMargin * 2) * 2;

	auto photoSize = this->photos[this->fullscreenIndex].getData()->getSize();
	auto widthRatio = (float)photoSize.Width / fullscreenWidth;
	auto heightRatio = (float)photoSize.Height / WindowHeight;

	auto fullscreenSize = widthRatio >= heightRatio ?
		dimension2d<s32>(fullscreenWidth, photoSize.Height * fullscreenWidth / photoSize.Width) :
		dimension2d<s32>(photoSize.Width * WindowHeight / photoSize.Height, WindowHeight);

	auto fullScreenLeft = (WindowWidth - fullscreenSize.Width) / 2;
	auto fullScreenTop = (WindowHeight - fullscreenSize.Height) / 2;

	return rect<s32>(vector2d<s32>(fullScreenLeft, fullScreenTop), fullscreenSize);
}
