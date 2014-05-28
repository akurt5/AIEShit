#ifndef __Collision_Imposters_H_
#define __Collision_Imposters_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class Collision_Imposters : public Application
{
public:

	Collision_Imposters();
	virtual ~Collision_Imposters();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	
	//!- TUTORIAL
	
	//!- TUTORIAL
};

#endif // __Collision_Imposters_H_