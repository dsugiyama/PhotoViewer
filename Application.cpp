#include <fstream>
#include <new>
#include <picojson.h>
#include "Application.h"
#include "Irrlicht-i18n\CGUITTFont.h"

using namespace irr::core;
using namespace irr::video;

Application::Application()
{
	this->device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(WindowWidth, WindowHeight));
	this->largeFont = CGUITTFont::createTTFont(this->device, FontFile.c_str(), LargeFontSize);
	this->regularFont = CGUITTFont::createTTFont(this->device, FontFile.c_str(), RegularFontSize);

	this->device->setWindowCaption(WindowCaption.c_str());

	this->LoadPhotos();
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
		this->largeFont->draw(L"Photo Viewer", rect<s32>(50, 50, WindowWidth - 50 - 1, 50 + 50 - 1),
			SColor(255, 127, 127, 127));
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
