#ifndef __Tesselation_H_
#define __Tesselation_H_

#include "Application.h"
#include <glm/glm.hpp>

//#include "Shader_Class.h"
#include "Sprite_Class.h"

// Derived application class that wraps up all globals neatly
class Tesselation : public Application
{
public:

	Tesselation();
	virtual ~Tesselation();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	OGL_FBXRenderData RenderData;
	ShaderHandle *Shaders;
	SpriteHandle *Sprite;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

};

#endif // __Tesselation_H_