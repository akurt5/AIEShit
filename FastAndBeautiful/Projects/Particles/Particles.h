#ifndef __Particles_H_
#define __Particles_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "ParticleSystem.h"

// Derived application class that wraps up all globals neatly
class Particles : public Application
{
public:
	unsigned int m_shader,m_texture;
	
	Particles();
	virtual ~Particles();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4 viewMatrix;
	//ParticleEmitter* m_emitter;
};

#endif // __Particles_H_