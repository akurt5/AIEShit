#include "Sprite_Class.h"

void SpriteHandle::Load(int _Size , const char *_Texture_Diff, const char *_Texture_Disp)
{

	std::stringstream sTextureDiff;
	sTextureDiff << "../../Build/textures/" << _Texture_Diff;
	std::string sTexturepath = sTextureDiff.str();
	_Texture_Diff = sTexturepath.c_str();

	if(_Texture_Disp != nullptr)
	{
		std::stringstream sTextureDisp;
		sTextureDisp << "../../Build/textures/" << _Texture_Diff;
		std::string sTexturepath = sTextureDisp.str();
		_Texture_Diff = sTexturepath.c_str();
	}

	Utility::build3DPlane(_Size, RenderData.VAO, RenderData.VBO, RenderData.IBO);

	Diffuse = SOIL_load_OGL_texture(_Texture_Diff, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
	Displacement = SOIL_load_OGL_texture(_Texture_Diff, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	
}