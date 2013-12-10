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

	this->LoadPhotos();

	this->largeFont = CGUITTFont::createTTFont(this->device, FontFile.c_str(), LargeFontSize);
	this->regularFont = CGUITTFont::createTTFont(this->device, FontFile.c_str(), RegularFontSize);

	auto driver = this->device->getVideoDriver();
	this->leftButtonImage = driver->getTexture(LeftButtonImageFile.c_str());
	this->rightButtonImage = driver->getTexture(RightButtonImageFile.c_str());
	this->closeButtonImage = driver->getTexture(LeftButtonImageFile.c_str());
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
		device->getGUIEnvironment()->drawAll();
		driver->endScene();
	}
}

bool Application::OnEvent(const SEvent& ev)
{
	if (ev.EventType != EET_GUI_EVENT) {
		return false;
	}

	int id = ev.GUIEvent.Caller->getID();

	switch (ev.GUIEvent.EventType) {
	case EGET_BUTTON_CLICKED:
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

	for (int i = 0; i < entries.size(); i++) {
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

	if (this->thumbnailPage) {
		int num = std::min((int)this->photos.size() - this->thumbnailIndex, NumberOfThumbnails);
		for (int i = 0; i < num; i++) {
			auto rect = this->GetThumbnailRect(i);
			auto button = env->addButton(rect, nullptr, Thumbnail + i);
			button->setImage(this->photos[this->thumbnailIndex + i].getData());
			button->setScaleImage(true);
		}


	} else {
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
