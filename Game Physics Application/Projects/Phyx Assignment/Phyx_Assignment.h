#ifndef __Phyx_Assignment_H_
#define __Phyx_Assignment_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "Scene.h"
#include "ParticleEmmiter.h"
#include "ParticleFluidEmmiter.h"

// Derived application class that wraps up all globals neatly
class Phyx_Assignment : public Application
{
public:

	Phyx_Assignment(){};
	virtual ~Phyx_Assignment(){};

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	Scene *MyScene;

	RIGIDACTOR Player;

	ParticleFluidEmitter *particleFluidEmitter;
	ParticleEmitter *particleEmitter;

	ClothData *MyClothData;

};

#endif // __Phyx_Assignment_H_