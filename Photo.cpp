#include "Photo.h"

Photo::Photo() : Photo(nullptr, "")
{
}

Photo::Photo(ITexture* data, const std::string& caption)
{
	this->data = data;
	this->caption = caption;
}

Photo::~Photo()
{
}
