#include "Application.h"
#include "Irrlicht-i18n\CGUITTFont.h"

using namespace irr::core;
using namespace irr::video;

Application::Application()
{
	this->device = createDevice(EDT_DIRECT3D9, dimension2d<u32>(WindowWidth, WindowHeight));
	this->largeFont = CGUITTFont::createTTFont(this->device, FontPath, LargeFontSize);
	this->regularFont = CGUITTFont::createTTFont(this->device, FontPath, RegularFontSize);

	this->device->setWindowCaption(WindowCaption);
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
