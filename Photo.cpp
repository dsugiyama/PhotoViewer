#include "Photo.h"

Photo::Photo() : Photo(nullptr, "")
{
}

Photo::Photo(ITexture* data, const std::string& caption)
{
	this->data = data;
	this->caption = caption;
}

ITexture* Photo::GetData()
{
	return this->data;
}

std::string Photo::GetCaption()
{
	return this->caption;
}

dimension2d<s32> Photo::GetOptimalSize(int maxWidth, int maxHeight)
{
	auto originalSize = this->data->getSize();
	auto widthRatio = (float)originalSize.Width / maxWidth;
	auto heightRatio = (float)originalSize.Height / maxHeight;
	return widthRatio >= heightRatio ?
		dimension2d<s32>(maxWidth, originalSize.Height * maxWidth / originalSize.Width) :
		dimension2d<s32>(originalSize.Width * maxHeight / originalSize.Height, maxHeight);
}
