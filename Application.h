#pragma once

#include <string>
#include <vector>
#include <irrlicht.h>
#include "Photo.h"

using namespace irr;
using namespace irr::gui;
using namespace irr::video;

class Application : public IEventReceiver {
public:
	Application();
	virtual ~Application();

	void Run();
	virtual bool OnEvent(const SEvent& ev);

private:
	static const int WindowWidth = 900;
	static const int WindowHeight = 540;
	static const int NumberOfThumbnails = 5;
	static const int MaxThumbnailSize = 144;
	static const int MinThumbnailMargin = 20;
	static const int MinThumbnailTop = 170;
	static const int ThumbnailArrowButtonDistance = 160;
	static const int ThumbnailArrowButtonBottomMargin = 80;
	static const int FullscreenButtonMargin = 10;
	static const int ScrollBarHeight = 16;
	static const int LargeFontSize = 40;
	static const int RegularFontSize = 14;
	static const int HeadingLeft = 40;
	static const int HeadingTop = 50;
	static const int HeadingWidth = 250;
	static const int HeadingHeight = 60;
	static const int CaptionHeight = 30;
	static const int TextColor = 0xFF7F7F7F;
	static const int OuterMargin = (WindowWidth - MaxThumbnailSize * NumberOfThumbnails
		- MinThumbnailMargin * (NumberOfThumbnails - 1)) / 2;

	const std::string PhotosDirectory = "photos";
	const std::string MetadataFile = PhotosDirectory + "\\metadata.json";
	const std::string ResourcesDirectory = "resources";
	const std::string FontFile = ResourcesDirectory + "\\mplus-2c-regular.ttf";
	const std::string LeftButtonImageFile = ResourcesDirectory + "\\button_left.png";
	const std::string RightButtonImageFile = ResourcesDirectory + "\\button_right.png";
	const std::string CloseButtonImageFile = ResourcesDirectory + "\\button_close.png";

	const std::wstring WindowCaption = L"Photo Viewer";
	const std::wstring Heading = L"Photos";

	IrrlichtDevice* device;
	std::vector<Photo> photos;
	ITexture* leftButtonImage;
	ITexture* rightButtonImage;
	ITexture* closeButtonImage;
	IGUIFont* largeFont;
	IGUIFont* regularFont;

	IGUIButton* thumbnailButtons[NumberOfThumbnails];
	IGUIStaticText* captionTexts[NumberOfThumbnails];
	IGUIImage* fullscreenImage;
	IGUIScrollBar* scrollBar;

	int thumbnailIndex = 0;
	int fullscreenIndex = 0;

private:
	void LoadPhotos();
	void ShowThumbnailView();
	void ShowFullscreenView();
	void SetThumbnails();
	void SetFullscreenImage();
	void RemoveThumbnails();
	void RemoveFullscreenImage();
};
