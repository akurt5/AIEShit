#ifndef __Force_and_Momentum_H_
#define __Force_and_Momentum_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "DIYPhisicsClass.h"

// Derived application class that wraps up all globals neatly
class Force_and_Momentum : public Application
{
public:

	Force_and_Momentum();
	virtual ~Force_and_Momentum();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	
	Sphere *Spring1, *Spring2, *Spring3; 
	SpringJoint *SP1, *SP2;

	DIYPhisicsHandle *DIYPhysicsScene;
};

#endif // __Force_and_Momentum_H_