#ifndef __Texturing_H_
#define __Texturing_H_

#include "Application.h"
#include <glm/glm.hpp>
#include <SOIL.h>

// Derived application class that wraps up all globals neatly
class Texturing : public Application
{
public:

	Texturing();
	virtual ~Texturing();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	unsigned int m_texture;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
	unsigned int m_vertShader;
	unsigned int m_fragShader;
	unsigned int m_shader;
};

#endif // __Texturing_H_