#include "Sprite_Class.h"
#include <sstream>


SpriteHandle::SpriteHandle(void)
{
}


SpriteHandle::~SpriteHandle(void)
{
}
void SpriteHandle::Load(const char* _Path)
{
	std::stringstream sTextureDiff;
	sTextureDiff << "../../Build/textures/" << _Path;
	std::string sTexturepath = sTextureDiff.str();
	_Path = sTexturepath.c_str();

	//  load image data
	int width = 0;
	int height = 0;
	int channels = 0;
	unsigned char* pixelData = SOIL_load_image(_Path, &width, &height, &channels, SOIL_LOAD_AUTO);

	printf("Width: %i Height: %i Channels: %i\n", width, height, channels);

	delete[] pixelData;
}