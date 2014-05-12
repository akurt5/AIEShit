#ifndef __FBX_Loader_H_
#define __FBX_Loader_H_

#include "Application.h"
#include <glm/glm.hpp>
#include "FBXFile.h"



struct OGL_FBXRenderData
{
	unsigned int VBO; // vertex buffer object
	unsigned int IBO; // index buffer object
	unsigned int VAO; // vertex array object
};

// Derived application class that wraps up all globals neatly
class FBX_Loader : public Application
{
public:

	FBX_Loader();
	virtual ~FBX_Loader();
	void InitFBXSceneResource(   FBXFile *a_pScene );
	void UpdateFBXSceneResource( FBXFile *a_pScene );
	void RenderFBXSceneResource( FBXFile *a_pScene, glm::mat4 a_view, glm::mat4 a_projection);
	void DestroyFBXSceneResource(FBXFile *a_pScene );
	FBXFile* m_fbx;
	unsigned int m_shader;
	virtual bool Load(int a_argc, char* a_argv[]);
	virtual void Draw(glm::mat4 viewmatrix);
	virtual void Unload();

protected:

	
	
	

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __FBX_Loader_H_