#include <fstream>
#include <new>
#include <picojson.h>
#include "Application.h"
#include "Irrlicht-i18n\CGUITTFont.h"

using namespace irr::core;

Application::Application()
{
	this->device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(WindowWidth, WindowHeight));
	this->device->setWindowCaption(WindowCaption.c_str());

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
	while (device->run()) {
		auto driver = device->getVideoDriver();
		driver->beginScene();
		driver->endScene();
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
