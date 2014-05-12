#ifndef __Advanced_Texturing_H_
#define __Advanced_Texturing_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include "FBX_Class.h"


// Derived application class that wraps up all globals neatly
class Advanced_Texturing : public Application
{
public:

	Advanced_Texturing();
	virtual ~Advanced_Texturing();

protected:
	ShaderHandle *Shader;
	FBXHandle *Mesh;


	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __Advanced_Texturing_H_