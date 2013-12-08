#pragma once

#include <Windows.h>
#include <irrlicht.h>

using namespace irr;
using namespace irr::gui;

class Application {
public:
	Application();
	~Application();

	void Run();

private:
	const int WindowWidth = 800;
	const int WindowHeight = 600;
	const PCWSTR WindowCaption = L"Photo Viewer";
	const PCWSTR FontPath = L"fonts\\mplus-2c-regular.ttf";
	const int LargeFontSize = 36;
	const int RegularFontSize = 14;

	IrrlichtDevice* device;
	IGUIFont* largeFont;
	IGUIFont* regularFont;
};

