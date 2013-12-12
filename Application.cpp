#include <fstream>
#define NOMINMAX
#include <Windows.h>
#include <picojson.h>
#include "Application.h"
#include "GUIElementID.h"
#include "Irrlicht-i18n\CGUITTFont.h"

using namespace irr::core;

Application::Application()
{
	this->device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(WindowWidth, WindowHeight));
	this->device->setWindowCaption(WindowCaption.c_str());
	this->device->setEventReceiver(this);

	this->LoadPhotos();

	auto driver = this->device->getVideoDriver();
	this->leftButtonImage = driver->getTexture(LeftButtonImageFile.c_str());
	this->rightButtonImage = driver->getTexture(RightButtonImageFile.c_str());
	this->closeButtonImage = driver->getTexture(CloseButtonImageFile.c_str());

	this->largeFont = CGUITTFont::createTTFont(this->device, FontFile.c_str(), LargeFontSize);
	this->regularFont = CGUITTFont::createTTFont(this->device, FontFile.c_str(), RegularFontSize);
}

Application::~Application()
{
	this->device->drop();
	this->largeFont->drop();
	this->regularFont->drop();
}

void Application::Run()
{
	this->ShowThumbnailView();

	while (this->device->run()) {
		auto driver = device->getVideoDriver();
		driver->beginScene();
		device->getGUIEnvironment()->drawAll();
		driver->endScene();
	}
}

bool Application::OnEvent(const SEvent& ev)
{
	if (ev.EventType != EET_GUI_EVENT) {
		return false;
	}

	auto caller = ev.GUIEvent.Caller;

	switch (ev.GUIEvent.EventType) {
	case EGET_BUTTON_CLICKED:
		{
			auto id = caller->getID();
			switch (id) {
			case GUIElementID::ThumbnailLeftButton:
				{
					auto nextIdx = std::max(this->thumbnailIndex - NumberOfThumbnails, 0);
					if (nextIdx != this->thumbnailIndex) {
						this->thumbnailIndex = nextIdx;
						this->scrollBar->setPos(this->thumbnailIndex);
						this->RemoveThumbnails();
						this->SetThumbnails();
					}
				}
				break;

			case GUIElementID::ThumbnailRightButton:
				{
					auto nextIdx = std::min(this->thumbnailIndex + NumberOfThumbnails, (int)this->photos.size() - NumberOfThumbnails);
					if (nextIdx != this->thumbnailIndex) {
						this->thumbnailIndex = nextIdx;
						this->scrollBar->setPos(this->thumbnailIndex);
						this->RemoveThumbnails();
						this->SetThumbnails();
					}
				}
				break;

			case GUIElementID::FullscreenLeftButton:
				{
					auto nextIdx = std::max(this->fullscreenIndex - 1, 0);
					if (nextIdx != this->fullscreenIndex) {
						this->fullscreenIndex = nextIdx;
						this->RemoveFullscreenImage();
						this->SetFullscreenImage();
						if (this->fullscreenIndex < this->thumbnailIndex) {
							this->thumbnailIndex = this->fullscreenIndex;
						}
					}
				}
				break;

			case GUIElementID::FullscreenRightButton:
				{
					auto nextIdx = std::min(this->fullscreenIndex + 1, (int)this->photos.size() - 1);
					if (nextIdx != this->fullscreenIndex) {
						this->fullscreenIndex = nextIdx;
						this->RemoveFullscreenImage();
						this->SetFullscreenImage();
						if (this->fullscreenIndex > this->thumbnailIndex + NumberOfThumbnails - 1) {
							this->thumbnailIndex = this->fullscreenIndex - NumberOfThumbnails + 1;
						}
					}
				}
				break;

			case GUIElementID::CloseButton:
				this->ShowThumbnailView();
				break;

			default:
				this->fullscreenIndex = this->thumbnailIndex + id - GUIElementID::Thumbnail;
				this->ShowFullscreenView();
				break;
			}
		}
		return true;

	case EGET_SCROLL_BAR_CHANGED:
		this->thumbnailIndex = ((IGUIScrollBar*)caller)->getPos();
		this->RemoveThumbnails();
		this->SetThumbnails();
		return true;
	}
}

void Application::LoadPhotos()
{
	std::ifstream ifstr(MetadataFile);
	picojson::value value;
	ifstr >> value;

	auto entries = value.get<picojson::array>();

	for (auto i = 0; i < entries.size(); i++) {
		auto entry = entries[i].get<picojson::object>();

		auto filename = entry["filename"].get<std::string>();
		auto path = PhotosDirectory + "\\" + filename;
		auto data = this->device->getVideoDriver()->getTexture(path.c_str());

		auto caption = entry["caption"].get<std::string>();

		this->photos.push_back(Photo(data, caption));
	}
}

void Application::ShowThumbnailView()
{
	auto env = this->device->getGUIEnvironment();
	env->clear();

	auto headingRect = rect<s32>(vector2d<s32>(HeadingLeft, HeadingTop), dimension2d<s32>(HeadingWidth, HeadingHeight));
	auto headingText = env->addStaticText(Heading.c_str(), headingRect);
	headingText->setOverrideFont(this->largeFont);
	headingText->setOverrideColor(SColor(TextColor));

	auto buttonSize = this->closeButtonImage->getSize();
	auto arrowButtonTop = WindowHeight - buttonSize.Height - ThumbnailArrowButtonBottomMargin;
	auto leftButtonPos = vector2d<s32>(WindowWidth / 2 - buttonSize.Width - (ThumbnailArrowButtonDistance / 2), arrowButtonTop);
	auto rightButtonPos = vector2d<s32>(WindowWidth / 2 + (ThumbnailArrowButtonDistance / 2), arrowButtonTop);
	auto leftButton = env->addButton(rect<s32>(leftButtonPos, buttonSize), nullptr, GUIElementID::ThumbnailLeftButton);
	auto rightButton = env->addButton(rect<s32>(rightButtonPos, buttonSize), nullptr, GUIElementID::ThumbnailRightButton);
	leftButton->setImage(this->leftButtonImage);
	rightButton->setImage(this->rightButtonImage);

	auto scrollBarRect = rect<s32>(vector2d<s32>(0, WindowHeight - ScrollBarHeight), dimension2d<s32>(WindowWidth, ScrollBarHeight));
	this->scrollBar = env->addScrollBar(true, scrollBarRect);
	this->scrollBar->setMin(0);
	this->scrollBar->setMax(std::max((int)this->photos.size() - NumberOfThumbnails, 0));
	this->scrollBar->setSmallStep(1);
	this->scrollBar->setLargeStep(NumberOfThumbnails);
	this->scrollBar->setPos(this->thumbnailIndex);

	this->SetThumbnails();
}

void Application::ShowFullscreenView()
{
	auto env = this->device->getGUIEnvironment();
	env->clear();

	auto buttonSize = this->closeButtonImage->getSize();
	auto closeButtonPos = vector2d<s32>(WindowWidth - buttonSize.Width - FullscreenButtonMargin, FullscreenButtonMargin);
	auto closeButton = env->addButton(rect<s32>(closeButtonPos, buttonSize), nullptr, GUIElementID::CloseButton);
	closeButton->setImage(this->closeButtonImage);

	auto arrowButtonTop = (WindowHeight - buttonSize.Height) / 2;
	auto leftButtonPos = vector2d<s32>(FullscreenButtonMargin, arrowButtonTop);
	auto rightButtonPos = vector2d<s32>(WindowWidth - buttonSize.Width - FullscreenButtonMargin, arrowButtonTop);
	auto leftButton = env->addButton(rect<s32>(leftButtonPos, buttonSize), nullptr, GUIElementID::FullscreenLeftButton);
	auto rightButton = env->addButton(rect<s32>(rightButtonPos, buttonSize), nullptr, GUIElementID::FullscreenRightButton);
	leftButton->setImage(this->leftButtonImage);
	rightButton->setImage(this->rightButtonImage);

	this->SetFullscreenImage();
}

void Application::SetThumbnails()
{
	auto num = std::min(NumberOfThumbnails, (int)this->photos.size() - this->thumbnailIndex);
	for (auto i = 0; i < num; i++) {
		auto env = this->device->getGUIEnvironment();
		auto photo = this->photos[this->thumbnailIndex + i];

		auto thumbnailSize = photo.GetOptimalSize(MaxThumbnailSize, MaxThumbnailSize);
		auto thumbnailLeft = OuterMargin + (MaxThumbnailSize + MinThumbnailMargin) * i
			+ (MaxThumbnailSize - thumbnailSize.Width) / 2;
		auto thumbnailTop = MinThumbnailTop + (MaxThumbnailSize - thumbnailSize.Height) / 2;
		auto thumbnailRect = rect<s32>(vector2d<s32>(thumbnailLeft, thumbnailTop), thumbnailSize);
		this->thumbnailButtons[i] = env->addButton(thumbnailRect, nullptr, GUIElementID::Thumbnail + i);
		this->thumbnailButtons[i]->setImage(photo.GetData());
		this->thumbnailButtons[i]->setPressedImage(photo.GetData());
		this->thumbnailButtons[i]->setScaleImage(true);

		auto captionLeft = OuterMargin + (MaxThumbnailSize + MinThumbnailMargin) * i;
		auto captionTop = MinThumbnailTop + MaxThumbnailSize;
		auto captionRect = rect<s32>(vector2d<s32>(captionLeft, captionTop), dimension2d<s32>(MaxThumbnailSize, CaptionHeight));

		wchar_t caption[20];
		MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, photo.GetCaption().c_str(), -1, caption, 20);
		
		this->captionTexts[i] = env->addStaticText(caption, captionRect);
		this->captionTexts[i]->setOverrideFont(this->regularFont);
		this->captionTexts[i]->setOverrideColor(SColor(TextColor));
		this->captionTexts[i]->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);
	}
}

void Application::SetFullscreenImage()
{
	auto env = this->device->getGUIEnvironment();
	auto photo = this->photos[this->fullscreenIndex];

	auto buttonSize = this->closeButtonImage->getSize();
	auto maxFullscreenWidth = WindowWidth - (buttonSize.Width + FullscreenButtonMargin * 2) * 2;

	auto fullscreenSize = photo.GetOptimalSize(maxFullscreenWidth, WindowHeight);
	auto fullscreenLeft = (WindowWidth - fullscreenSize.Width) / 2;
	auto fullscreenTop = (WindowHeight - fullscreenSize.Height) / 2;
	auto fullscreenRect = rect<s32>(vector2d<s32>(fullscreenLeft, fullscreenTop), fullscreenSize);
	this->fullscreenImage = env->addImage(fullscreenRect);
	this->fullscreenImage->setImage(this->photos[this->fullscreenIndex].GetData());
	this->fullscreenImage->setScaleImage(true);
}

void Application::RemoveThumbnails()
{
	auto num = std::min(NumberOfThumbnails, (int)this->photos.size() - this->thumbnailIndex);
	for (auto i = 0; i < num; i++) {
		this->thumbnailButtons[i]->remove();
		this->captionTexts[i]->remove();
	}
}

void Application::RemoveFullscreenImage()
{
	this->fullscreenImage->remove();
}
