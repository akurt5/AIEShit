#pragma once

#include <sstream>

#include "Render_Class.h"
#include "Shader_Class.h"

class SpriteHandle
{
public:
	void Load(int _Size , const char *_Texture_Diff, const char *_Texture_Disp = nullptr);

	unsigned int Diffuse, Displacement;

protected:
	OGL_FBXRenderData RenderData;

};
