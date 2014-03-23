#ifndef __Assignment_H_
#define __Assignment_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "GPUParticle_Class.h"
#include "Light_Class.h"
#include "FBX_Class.h"
#include "Planet.h"


// Derived application class that wraps up all globals neatly
class Assignment : public Application
{
public:

	Assignment();
	virtual ~Assignment();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();
	std::vector <FBXHandle*> MeshList;
	std::vector <const char*> Shaders;
	//Planet *Rabbit;
	FBXHandle *Rabbit;
	FBXHandle *Pyro;
	LightHandle *Lighting;
	ShaderHandle *LightShader;
	ShaderHandle *AnimShader;
	ShaderHandle *GPUParticleShader;
	GPUParticleHandle *Particles1;
	GPUParticleHandle *Particles2;
	Planet *Emitter1;
	Planet *Emitter2;
	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __Assignment_H_