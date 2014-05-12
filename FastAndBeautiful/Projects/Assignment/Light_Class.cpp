#include "Light_Class.h"

LightHandle::LightHandle(){}
LightHandle::~LightHandle(){}
void LightHandle::Load(std::vector <FBXHandle*> _MeshList)
{
	// create the GL VAO/VBO/IBO data for meshes
	for(int a=0;a<_MeshList.size();++a)
	{
		for ( unsigned int i = 0 ; i < _MeshList[a]->m_fbx->getMeshCount() ; ++i )
		{
			FBXMeshNode* mesh = _MeshList[a]->m_fbx->getMeshByIndex(i);

			// storage for the opengl data in 3 unsigned int
			unsigned int* glData = new unsigned int[3];

			glGenVertexArrays(1, &glData[0]);
			glBindVertexArray(glData[0]);

			glGenBuffers(1, &glData[1]);
			glGenBuffers(1, &glData[2]);

			glBindBuffer(GL_ARRAY_BUFFER, glData[1]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData[2]);

			glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

			glEnableVertexAttribArray(0); // position
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), 0);

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			mesh->m_userData = glData;
		}
	}
}
void LightHandle::Draw(glm::mat4 a_view, glm::mat4 a_projection, std::vector <FBXHandle*> _MeshList, ShaderHandle *_Shaders)
{
	// bind shader to the GPU
	glUseProgram(_Shaders->m_shader);

	// fetch locations of the view and projection matrices and bind them
	GLint location = glGetUniformLocation(_Shaders->m_shader,"view");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(a_view));

	location = glGetUniformLocation(_Shaders->m_shader,"projection");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(a_projection));

	// bind our vertex array object and draw the mesh
	for(int a=0;a<_MeshList.size();++a)
	{
		for ( unsigned int i = 0 ; i < _MeshList[a]->m_fbx->getMeshCount() ; ++i )
		{
			FBXMeshNode* mesh = _MeshList[a]->m_fbx->getMeshByIndex(i);

			unsigned int* glData = (unsigned int*)mesh->m_userData;
			
			glBindVertexArray( glData[0] );
			glDrawElements(GL_TRIANGLES, (unsigned int)mesh->m_indices.size(), GL_UNSIGNED_INT, 0);
		}
	}
	// send camera position
	glm::mat4 m_cameraMatrix = glm::inverse( a_view );
	location = glGetUniformLocation(_Shaders->m_shader,"cameraPosition");
	glUniform3fv(location, 1, glm::value_ptr(m_cameraMatrix[3]));
}
void LightHandle::Unload(std::vector <FBXHandle*> _MeshList)
{
	// bind our vertex array object and draw the mesh
	for(int a=0;a<_MeshList.size();++a)
	{
		for ( unsigned int i = 0 ; i < _MeshList[a]->m_fbx->getMeshCount() ; ++i )
		{
			FBXMeshNode* mesh = _MeshList[a]->m_fbx->getMeshByIndex(i);

			unsigned int* glData = (unsigned int*)mesh->m_userData;

			glDeleteVertexArrays(1, &glData[0]);
			glDeleteBuffers(1, &glData[1]);
			glDeleteBuffers(1, &glData[2]);

			delete[] glData;
		}
	}
}
