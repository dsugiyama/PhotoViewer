#pragma once

#include <string>
#include <irrlicht.h>

using namespace irr::video;

class Photo {
public:
	Photo();
	Photo(ITexture* data, const std::string& caption);
	~Photo();

private:
	ITexture* data;
	std::string caption;
};

