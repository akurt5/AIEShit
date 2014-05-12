#ifndef __Game_Physics_Application_H_
#define __Game_Physics_Application_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class Game_Physics_Application : public Application
{
public:

	Game_Physics_Application();
	virtual ~Game_Physics_Application();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __Game_Physics_Application_H_