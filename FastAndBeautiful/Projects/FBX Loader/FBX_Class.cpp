#include "FBX_Class.h"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

FBXHandle::FBXHandle(){}
FBXHandle::~FBXHandle(){}
void FBXHandle::Load(const char* _MODELpath/*, char* _VERTpath*/ ){
// load the shader
	const char* aszInputs[] =  { "Position", "Color", "TexCoord1" };
	const char* aszOutputs[] = { "outColour" };

	//std::string _VERTpath();
	//std::string::append("../Build/shaders/", _VERTpath, ".vert");

	// load shader internally calls glCreateShader...
	GLuint vshader = Utility::loadShader("../Build/shaders/SimpleFBX.vert", GL_VERTEX_SHADER);
	GLuint pshader = Utility::loadShader("../Build/shaders/SimpleFBX.frag", GL_FRAGMENT_SHADER);
	
	m_shader = Utility::createProgram( vshader, 0, 0, 0, pshader, 3, aszInputs, 1, aszOutputs);
		
	m_fbx = new FBXFile();
	m_fbx->load(_MODELpath, FBXFile::UNITS_CENTIMETER );
	

	unsigned int meshCount = m_fbx->getMeshCount();
	unsigned int matCount = m_fbx->getMaterialCount();

	// loop through each mesh
	for(int i=0; i<meshCount; ++i)
	{
		// get the current mesh
		FBXMeshNode *pMesh = m_fbx->getMeshByIndex(i);

		// genorate our OGL_FBXRenderData for storing the meshes VBO, IBO and VAO
		// and assign it to the meshes m_userData pointer so that we can retrive 
		// it again within the render function
		OGL_FBXRenderData *ro = new OGL_FBXRenderData();
		pMesh->m_userData = ro;

		// OPENGL: genorate the VBO, IBO and VAO
		glGenBuffers(1, &ro->VBO);
		glGenBuffers(1, &ro->IBO);
		glGenVertexArrays(1, &ro->VAO);

		// OPENGL: Bind  VAO, and then bind the VBO and IBO to the VAO
		glBindVertexArray(ro->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, ro->VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ro->IBO);

		// Send the vertex data to the VBO
		glBufferData(GL_ARRAY_BUFFER, pMesh->m_vertices.size() * sizeof(FBXVertex), pMesh->m_vertices.data(), GL_STATIC_DRAW);
		
		// send the index data to the IBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, pMesh->m_indices.size() * sizeof(unsigned int), pMesh->m_indices.data(), GL_STATIC_DRAW);

		// enable the attribute locations that will be used on our shaders
		glEnableVertexAttribArray(0);//Position
		glEnableVertexAttribArray(1);//Colour
		glEnableVertexAttribArray(2);//TexCoord1Offset
		
		// tell our shaders where the information within our buffers lie
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::ColourOffset);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::TexCoord1Offset);

		// finally, where done describing our mesh to the shader
		// we can describe the next mesh
		glBindVertexArray(0);
	}
	
	// free our shader once we built our program
	glDeleteShader( vshader );
	glDeleteShader( pshader );
}
void FBXHandle::Unload(){
	glDeleteShader(m_shader);

		// how manu meshes and materials are stored within the fbx file
	unsigned int meshCount = m_fbx->getMeshCount();
	unsigned int matCount = m_fbx->getMaterialCount();

	// remove meshes
	for(unsigned int i=0; i<meshCount; i++)
	{
		// Get the current mesh and retrive the render data we assigned to m_userData
		FBXMeshNode* pMesh = m_fbx->getMeshByIndex(i);
		OGL_FBXRenderData *ro = (OGL_FBXRenderData *)pMesh->m_userData;

		// delete the buffers and free memory from the graphics card
		glDeleteBuffers(1, &ro->VBO);
		glDeleteBuffers(1, &ro->IBO);
		glDeleteVertexArrays(1, &ro->VAO);

		// this is memory we created earlier in the InitFBXSceneResources function
		// make sure to destroy it
		delete ro;

	}

	// loop through each of the materials
	for(int i=0; i<matCount; i++)
	{
		// get the current material
		FBXMaterial *pMaterial = m_fbx->getMaterialByIndex(i);
		for(int j=0; j<FBXMaterial::TextureTypes_Count; j++)
		{
			// delete the texture if it was loaded
			if( pMaterial->textureIDs[j] != 0 )
				glDeleteTextures(1, &pMaterial->textureIDs[j]);
		}
	}
	m_fbx->unload();
	delete m_fbx;
	m_fbx = NULL;
}
void FBXHandle::Update()
{
	for(int i=0; i<m_fbx->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode *mesh = m_fbx->getMeshByIndex(i);

		// if you move an object around within your scene
		// children nodes are not updated until this function is called.
		mesh->updateGlobalTransform();
	}
}
void FBXHandle::Draw(glm::mat4 a_view, glm::mat4 a_projection)
{
	// activate a shader
	glUseProgram( m_shader );

	// get the location of uniforms on the shader
	GLint uDiffuseTexture = glGetUniformLocation(m_shader, "DiffuseTexture");
	GLint uDiffuseColor   = glGetUniformLocation(m_shader, "DiffuseColor");	
	
	GLint uModel      = glGetUniformLocation(m_shader, "Model");
	GLint uView       = glGetUniformLocation(m_shader, "View");
	GLint uProjection = glGetUniformLocation(m_shader, "Projection");
	
	// for each mesh in the model...
	for(int i=0; i<m_fbx->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode *mesh = m_fbx->getMeshByIndex(i);

		// get the render data attached to the m_userData pointer for this mesh
		OGL_FBXRenderData *ro = (OGL_FBXRenderData *) mesh->m_userData;

		// Bind the texture to one of the ActiveTextures
		// if your shader supported multiple textures, you would bind each texture to a new Active Texture ID here
		glActiveTexture( GL_TEXTURE1 );
		glBindTexture( GL_TEXTURE_2D, mesh->m_material->textureIDs[ FBXMaterial::DiffuseTexture] );

		// reset back to the default active texture
		glActiveTexture( GL_TEXTURE0 );

		// tell the shader which texture to use
		glUniform1i( uDiffuseTexture, 1 );
	 
		// send the Model, View and Projection Matrices to the shader
		glUniformMatrix4fv( uModel,	     1, false, glm::value_ptr(mesh->m_globalTransform) );
		glUniformMatrix4fv( uView,       1, false, glm::value_ptr(a_view) );
		glUniformMatrix4fv( uProjection, 1, false, glm::value_ptr(a_projection) );

		// bind our vertex array object
		// remember in the initialise function, we bound the VAO and IBO to the VAO
		// so when we bind the VAO, openGL knows what what vertices,
		// indices and vertex attributes to send to the shader
		glBindVertexArray(ro->VAO);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, 0);

	}

	// finally, we have finished rendering the meshes
	// disable this shader
	glUseProgram(0);
}

