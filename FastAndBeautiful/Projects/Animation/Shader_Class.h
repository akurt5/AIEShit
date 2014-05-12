//#pragma once
#ifndef ShaderClass_H_
#define ShaderClass_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <string>
#include <sstream>
#include <vector>

struct Shader
{
	const char *Path, *Type;

};

class ShaderHandle
{
public:
	ShaderHandle();
	virtual~ShaderHandle();
	//FILE TYPE ONLY 4 CHAR AFTER '.' (#shaders, .vert, .frag, .geom, inputs ...)
	//void Load(int _Count, char *_Shader, ...);
	//void Load(std::vector <const char*> _ShaderList);
	void Load(const char *_VERT, const char *_FRAG, const char *_GEOM = nullptr);
	void Reload();
	void Unload();


	unsigned int m_shader;
	GLint gshader;
};

#endif