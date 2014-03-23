#ifndef __GPU_Particles_H_
#define __GPU_Particles_H_

#include "Application.h"
#include <glm/glm.hpp>

// Derived application class that wraps up all globals neatly
class GPU_Particles : public Application
{
public:

	GPU_Particles();
	virtual ~GPU_Particles();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __GPU_Particles_H_