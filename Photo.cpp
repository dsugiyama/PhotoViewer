#include "Photo.h"

Photo::Photo() : Photo(nullptr, "")
{
}

Photo::Photo(ITexture* data, const std::string& caption)
{
	this->data = data;
	this->caption = caption;
}

ITexture* Photo::getData()
{
	return this->data;
}

std::string Photo::getCaption()
{
	return this->caption;
}
