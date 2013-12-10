#pragma once

#include <string>
#include <vector>
#include <irrlicht.h>
#include "Photo.h"

using namespace irr;
using namespace irr::core;
using namespace irr::gui;
using namespace irr::video;

class Application : public IEventReceiver {
public:
	Application();
	virtual ~Application();

	void Run();
	virtual bool OnEvent(const SEvent& ev);

private:
	const std::string PhotosDirectory = "photos";
	const std::string MetadataFile = PhotosDirectory + "\\metadata.json";
	const std::string ResourcesDirectory = "resources";
	const std::string FontFile = ResourcesDirectory + "\\mplus-2c-regular.ttf";
	const std::string LeftButtonImageFile = ResourcesDirectory + "\\button_left.png";
	const std::string RightButtonImageFile = ResourcesDirectory + "\\button_right.png";
	const std::string CloseButtonImageFile = ResourcesDirectory + "\\button_close.png";

	const int WindowWidth = 800;
	const int WindowHeight = 480;
	const int LargeFontSize = 36;
	const int RegularFontSize = 14;
	const int NumberOfThumbnails = 5;
	const int MaxThumbnailSize = 130;
	const int MinThumbnailMargin = 20;
	const int MinThumbnailTop = 220;

	IrrlichtDevice* device;
	std::vector<Photo> photos;
	IGUIFont* largeFont;
	IGUIFont* regularFont;
	ITexture* leftButtonImage;
	ITexture* rightButtonImage;
	ITexture* closeButtonImage;

	bool thumbnailPage = true;
	bool displayChanged = true;
	int thumbnailIndex = 0;

	void LoadPhotos();
	void ConfigureGUIElements();
	rect<s32> GetThumbnailRect(int idx);
};

