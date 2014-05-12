#include "FBX_Class.h"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

FBXHandle::FBXHandle(){}
FBXHandle::~FBXHandle(){}
//void FBXHandle::Load(const char* _MODELpath/*, char* _VERTpath*/ ){
void FBXHandle::Load(const char*_MODEL, FBXFile::UNIT_SCALE _Scale)
{
	HasSkeleton = false;

	std::stringstream sMODEL;
	sMODEL << "../../Build/Models/" << _MODEL << ".fbx";
	std::string sMODELpath = sMODEL.str();
	MODEL = sMODELpath.c_str();


	m_fbx = new FBXFile();

	m_fbx->load(MODEL, _Scale);

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
		glEnableVertexAttribArray(0);//POSITION
		glEnableVertexAttribArray(1);//COLOUR	
		glEnableVertexAttribArray(2);//NORMAL	
		glEnableVertexAttribArray(3);//TANGENT	
		glEnableVertexAttribArray(4);//BINORMAL
		glEnableVertexAttribArray(5);//INDICES	
		glEnableVertexAttribArray(6);//WEIGHTS	
		glEnableVertexAttribArray(7);//TEXCOORD1
		glEnableVertexAttribArray(8);//TEXCOORD2

		// tell our shaders where the information within our buffers lie
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::PositionOffset);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::ColourOffset);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::NormalOffset);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::TangentOffset);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::BiNormalOffset);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::IndicesOffset);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::WeightsOffset);
		glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::TexCoord1Offset);
		glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char *)FBXVertex::TexCoord2Offset);

		// finally, where done describing our mesh to the shader
		// we can describe the next mesh
		glBindVertexArray(0);
	}

	if(m_fbx->m_skeletons.size() != 0){
		HasSkeleton = true;
		printf("Skeleton FOUND for   ");printf( _MODEL);printf( "   ASSUMING BAKED IN ANIMATIONS!.\n");
	}else{
		printf("Skeleton NOT found for   ");printf(_MODEL);printf("\n");
	}

}
void FBXHandle::Unload(){

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
void FBXHandle::Update( ShaderHandle *_Shaders)
{

	if(HasSkeleton){
		// grab the skeleton and animation we want to use
		FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);
		FBXAnimation* animation = m_fbx->getAnimationByIndex(0);

		// evaluate the animation to update bones
		skeleton->evaluate( animation, Utility::getTotalTime() );
		skeleton->updateBones();
	}	
	for(int i=0; i<m_fbx->getMeshCount(); ++i)
	{
		// get the current mesh
		FBXMeshNode *mesh = m_fbx->getMeshByIndex(i);

		// if you move an object around within your scene
		// children nodes are not updated until this function is called.
		mesh->updateGlobalTransform();
	}

}
void FBXHandle::Draw(glm::mat4 a_view, glm::mat4 a_projection, ShaderHandle *_Shaders)
{
	// activate a shader
	glUseProgram( _Shaders->m_shader );

	if(HasSkeleton){
		// update the bones to include the bind pose
		// so that the offset is local
		FBXSkeleton* skeleton = m_fbx->getSkeletonByIndex(0);

		// bind the array of bones
		int location = glGetUniformLocation(_Shaders->m_shader, "bones");
		glUniformMatrix4fv(location, skeleton->m_boneCount, GL_FALSE, (float*)skeleton->m_bones);

		//location = glGetUniformLocation(_Shaders->m_shader,"Animate");
		//glUniform1f(location, HasSkeleton);
	}

	location = glGetUniformLocation(_Shaders->m_shader,"Texture");
	glUniform1f(location, HasTextures);

	

	//float time = glGetUniformLocation(_Shaders->m_shader, "Time");

	// get the location of uniforms on the shader
	GLint uDiffuseTexture = glGetUniformLocation(_Shaders->m_shader, "DiffuseTexture");
	GLint uDiffuseColor   = glGetUniformLocation(_Shaders->m_shader, "DiffuseColor");	

	GLint uModel      = glGetUniformLocation(_Shaders->m_shader, "Model");
	GLint uView       = glGetUniformLocation(_Shaders->m_shader, "View");
	GLint uProjection = glGetUniformLocation(_Shaders->m_shader, "Projection");

	//fetch locations of the view and projection matrices and bind them
	//GLint uView = glGetUniformLocation(_Shaders->m_shader,"View");
	//glUniformMatrix4fv(location, 1, false, glm::value_ptr(a_view));

	//GLint uProjection = glGetUniformLocation(_Shaders->m_shader,"Projection");
	//glUniformMatrix4fv(location, 1, false, glm::value_ptr(a_projection));

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
//void FBXHandle::Move(glm::vec3 _Position){

//}



