#ifndef __Animation_H_
#define __Animation_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "FBX_Class.h"

// Derived application class that wraps up all globals neatly
class Animation : public Application
{
public:

	Animation();
	virtual ~Animation();
	
	FBXHandle *AnimModel;
	
	ShaderHandle *AnimShaders;
protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;

	
};

#endif // __Animation_H_