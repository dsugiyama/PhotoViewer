#pragma once

#include <string>
#include <irrlicht.h>

using namespace irr;
using namespace irr::core;
using namespace irr::video;

class Photo {
public:
	Photo();
	Photo(ITexture* data, const std::string& caption);

	ITexture* GetData();
	std::string GetCaption();
	dimension2d<s32> GetOptimalSize(int maxWidth, int maxHeight);

private:
	ITexture* data;
	std::string caption;
};

