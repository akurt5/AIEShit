#ifndef __FBX_Loader_H_
#define __FBX_Loader_H_

#include "Application.h"
#include <glm/glm.hpp>


// Derived application class that wraps up all globals neatly
class FBX_Loader : public Application
{
public:

	FBX_Loader();
	virtual ~FBX_Loader();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __FBX_Loader_H_

