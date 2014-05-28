#ifndef __Projectile_Motion_H_
#define __Projectile_Motion_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class Projectile_Motion : public Application
{
public:

	Projectile_Motion();
	virtual ~Projectile_Motion();

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

#endif // __Projectile_Motion_H_