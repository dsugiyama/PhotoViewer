#pragma once

#include <string>
#include <vector>
#include <irrlicht.h>
#include "Photo.h"

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
	const std::wstring WindowCaption = L"Photo Viewer";
	const std::string FontFile = "fonts\\mplus-2c-regular.ttf";
	const int LargeFontSize = 36;
	const int RegularFontSize = 14;
	const std::string PhotosDirectory = "photos";
	const std::string MetadataFile = PhotosDirectory + "\\metadata.json";

	IrrlichtDevice* device;
	IGUIFont* largeFont;
	IGUIFont* regularFont;
	std::vector<Photo> photos;

	void LoadPhotos();
};

