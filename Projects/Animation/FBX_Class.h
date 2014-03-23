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


#include "Shader_Class.h"
//#include "Light_Class.h"

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
	
	//void Load(const char* _MODELpath = "./Models/soulspear/soulspear.fbx"/*, char* _VERTpath*/ );
	void Load(const char *_MODEL, FBXFile::UNIT_SCALE _Scale);
	void Update( ShaderHandle *_Shaders);
	void Draw(glm::mat4 a_view, glm::mat4 a_projection, ShaderHandle *_Shaders);
	//void Move(glm::vec3 _Position);
	void Unload();
	
	FBXFile *m_fbx;

	//ShaderHandle Shaders;

	int location;

	bool HasSkeleton;
	bool HasTextures;
	
	const char *MODEL;

protected:

};

#endif 

