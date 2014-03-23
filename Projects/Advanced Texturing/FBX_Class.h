#ifndef FBXClass_H_
#define FBXClass_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "FBXFile.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <SOIL.h>
#include <string>

struct OGL_FBXRenderData
{
	unsigned int VBO; // vertex buffer object
	unsigned int IBO; // index buffer object
	unsigned int VAO; // vertex array object
};

class FBXHandle
{
public:

	FBXHandle();
	virtual~FBXHandle();
	
	void Load(const char* _MODELpath = "./Models/soulspear/soulspear.fbx"/*, char* _VERTpath*/ );
	void Update();
	void Draw(glm::mat4 a_view, glm::mat4 a_projection);
	void Unload();
	
	FBXFile* m_fbx;

	unsigned int m_decayTexture;
	unsigned int m_decayValue;
	unsigned int m_shader;

};

#endif 

