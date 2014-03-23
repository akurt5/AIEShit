#ifndef __Lighting_H_
#define __Lighting_H_

#include "Application.h"
#include <glm/glm.hpp>
#include <FBXFile.h>


// Derived application class that wraps up all globals neatly
class Lighting : public Application
{
public:

	Lighting();
	virtual ~Lighting();
	void createOpenGLBuffers(FBXFile* a_fbx);
	void cleanupOpenGLBuffers(FBXFile* a_fbx);

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	unsigned int m_vertShader;
	unsigned int m_fragShader;
	unsigned int m_shader;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __Lighting_H_