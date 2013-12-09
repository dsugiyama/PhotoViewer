#pragma once

#include <string>
#include <vector>
#include <irrlicht.h>
#include "Photo.h"

using namespace irr;
using namespace irr::gui;
using namespace irr::video;

class Application {
public:
	Application();
	~Application();

	void Run();

private:
	const int WindowWidth = 800;
	const int WindowHeight = 600;
	const std::wstring WindowCaption = L"Photo Viewer";
	const std::string PhotosDirectory = "photos";
	const std::string MetadataFile = PhotosDirectory + "\\metadata.json";
	const std::string ResourcesDirectory = "resources";
	const std::string FontFile = ResourcesDirectory + "\\mplus-2c-regular.ttf";
	const int LargeFontSize = 36;
	const int RegularFontSize = 14;
	const std::string LeftButtonImageFile = ResourcesDirectory + "\\button_left.png";
	const std::string RightButtonImageFile = ResourcesDirectory + "\\button_right.png";
	const std::string CloseButtonImageFile = ResourcesDirectory + "\\button_close.png";

	IrrlichtDevice* device;
	std::vector<Photo> photos;
	IGUIFont* largeFont;
	IGUIFont* regularFont;
	ITexture* leftButtonImage;
	ITexture* rightButtonImage;
	ITexture* closeButtonImage;

	void LoadPhotos();
};

