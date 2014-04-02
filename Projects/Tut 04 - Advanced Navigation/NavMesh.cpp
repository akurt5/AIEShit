#include "NavMesh.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

NavMesh::NavMesh()
{

}

NavMesh::~NavMesh()
{

}

bool NavMesh::onCreate(int a_argc, char* a_argv[]) 
{
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,0),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	m_sponza = new FBXFile();
	m_sponza->load("../../Build/models/SponzaSimple.fbx", FBXFile::UNITS_CENTIMETER);
	createOpenGLBuffers(m_sponza);

	m_navMesh = new FBXFile();
	m_navMesh->load("../../Build/models/SponzaSimpleNavMesh.fbx", FBXFile::UNITS_CENTIMETER);
//	createOpenGLBuffers(m_navMesh);

	BuildNavMesh(m_navMesh->getMeshByIndex(0), m_Graph);

	unsigned int vs = Utility::loadShader("../../Build/shaders/sponza.vert", GL_VERTEX_SHADER);
	unsigned int fs = Utility::loadShader("../../Build/shaders/sponza.frag", GL_FRAGMENT_SHADER);
	m_shader = Utility::createProgram(vs,0,0,0,fs);
	glDeleteShader(vs);
	glDeleteShader(fs);

	return true;
}

void NavMesh::onUpdate(float a_deltaTime) 
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	// add an identity matrix gizmo
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	// add a 20x20 grid on the Z-plane
	for ( int i = 0 ; i < 21 ; ++i )
	{
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	for (auto node : m_Graph)
	{
		Gizmos::addAABBFilled(node->Position, glm::vec3(0.05f), glm::vec4(1, 0, 0, 1));

		if (node->edgeTarget[0] != nullptr)
		{
			Gizmos::addLine(node->Position + glm::vec3(0, 0.05f, 0), node->edgeTarget[0]->Position+glm::vec3(0, 0.05f, 0), glm::vec4(1, 1, 0, 1));
		}
		if (node->edgeTarget[1] != nullptr)
		{
			Gizmos::addLine(node->Position + glm::vec3(0, 0.05f, 0), node->edgeTarget[1]->Position+glm::vec3(0, 0.05f, 0), glm::vec4(1, 1, 0, 1));
		}
		if (node->edgeTarget[2] != nullptr)
		{
			Gizmos::addLine(node->Position + glm::vec3(0, 0.05f, 0), node->edgeTarget[2]->Position+glm::vec3(0, 0.05f, 0), glm::vec4(1, 1, 0, 1));
		}
	}

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void NavMesh::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	glUseProgram(m_shader);

	int location = glGetUniformLocation(m_shader, "projectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr( m_projectionMatrix * viewMatrix ));

	unsigned int count = m_sponza->getMeshCount();
	for (unsigned int i = 0 ; i < count ; ++i )
	{
		FBXMeshNode* mesh = m_sponza->getMeshByIndex(i);

		GLData* data = (GLData*)mesh->m_userData;

		location = glGetUniformLocation(m_shader, "model");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr( mesh->m_globalTransform ));

		location = glGetUniformLocation(m_shader, "invTransposeModel");
		glUniformMatrix4fv(location, 1, GL_TRUE, glm::value_ptr( glm::inverse( mesh->m_globalTransform ) ));

		glBindVertexArray(data->vao);
		glDrawElements(GL_TRIANGLES, mesh->m_indices.size(), GL_UNSIGNED_INT, nullptr);
	}
	
	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void NavMesh::onDestroy()
{
	cleanupOpenGLBuffers(m_sponza);
	//cleanupOpenGLBuffers(m_navMesh);

	delete m_navMesh;
	delete m_sponza;

	glDeleteProgram(m_shader);

	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new NavMesh();
	
	if (app->create("AIE - NavMesh",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}

void NavMesh::createOpenGLBuffers(FBXFile* a_fbx)
{
	// create the GL VAO/VBO/IBO data for meshes
	for ( unsigned int i = 0 ; i < a_fbx->getMeshCount() ; ++i )
	{
		FBXMeshNode* mesh = a_fbx->getMeshByIndex(i);

		// storage for the opengl data in 3 unsigned int
		GLData* glData = new GLData();

		glGenVertexArrays(1, &glData->vao);
		glBindVertexArray(glData->vao);

		glGenBuffers(1, &glData->vbo);
		glGenBuffers(1, &glData->ibo);

		glBindBuffer(GL_ARRAY_BUFFER, glData->vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glData->ibo);

		glBufferData(GL_ARRAY_BUFFER, mesh->m_vertices.size() * sizeof(FBXVertex), mesh->m_vertices.data(), GL_STATIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(unsigned int), mesh->m_indices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0); // position
		glEnableVertexAttribArray(1); // normal
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::PositionOffset );
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(FBXVertex), (char*)FBXVertex::NormalOffset );

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		mesh->m_userData = glData;
	}
}

void NavMesh::cleanupOpenGLBuffers(FBXFile* a_fbx)
{
	// bind our vertex array object and draw the mesh
	for ( unsigned int i = 0 ; i < a_fbx->getMeshCount() ; ++i )
	{
		FBXMeshNode* mesh = a_fbx->getMeshByIndex(i);

		GLData* glData = (GLData*)mesh->m_userData;

		glDeleteVertexArrays(1, &glData->vao);
		glDeleteBuffers(1, &glData->vbo);
		glDeleteBuffers(1, &glData->ibo);

		delete[] glData;
	}

	
}
void NavMesh::BuildNavMesh(FBXMeshNode *a_Mesh, std::vector<NavNode*> &a_Graph)
{
	unsigned int tricount = a_Mesh->m_indices.size() / 3;

	for (unsigned int tri=0; tri<tricount; ++tri)
	{
		NavNode *node = new NavNode();

		node->edgeTarget[0] = nullptr;
		node->edgeTarget[1] = nullptr;
		node->edgeTarget[2] = nullptr;

		node->Vertices[0] = a_Mesh->m_vertices[ a_Mesh->m_indices[tri * 3+0]].position.xyz;
		node->Vertices[1] = a_Mesh->m_vertices[ a_Mesh->m_indices[tri * 3+1]].position.xyz;
		node->Vertices[2] = a_Mesh->m_vertices[ a_Mesh->m_indices[tri * 3+2]].position.xyz;

		node ->Position = (node->Vertices[0] + node->Vertices[1] + node->Vertices[2]) / 3.0f;

		a_Graph.push_back(node);
	}

	for (auto start : a_Graph)
	{
		for (auto end : a_Graph)
		{
			if (start == end)
				continue;
			if((start->Vertices[0] == end->Vertices[0] && start->Vertices[1] == end->Vertices[1])
			 ||(start->Vertices[0] == end->Vertices[1] && start->Vertices[1] == end->Vertices[2])
			 ||(start->Vertices[0] == end->Vertices[2] && start->Vertices[1] == end->Vertices[0])
			 ||(start->Vertices[0] == end->Vertices[1] && start->Vertices[1] == end->Vertices[0])
			 ||(start->Vertices[0] == end->Vertices[2] && start->Vertices[1] == end->Vertices[1])
			 ||(start->Vertices[0] == end->Vertices[0] && start->Vertices[1] == end->Vertices[2]))
			{
				start->edgeTarget[0] = end;
			}
			if((start->Vertices[1] == end->Vertices[0] && start->Vertices[2] == end->Vertices[1])
			 ||(start->Vertices[1] == end->Vertices[1] && start->Vertices[2] == end->Vertices[2])
			 ||(start->Vertices[1] == end->Vertices[2] && start->Vertices[2] == end->Vertices[0])
			 ||(start->Vertices[1] == end->Vertices[1] && start->Vertices[2] == end->Vertices[0])
			 ||(start->Vertices[1] == end->Vertices[2] && start->Vertices[2] == end->Vertices[1])
			 ||(start->Vertices[1] == end->Vertices[0] && start->Vertices[2] == end->Vertices[2]))
			{
				start->edgeTarget[1] = end;
			}
			if((start->Vertices[2] == end->Vertices[0] && start->Vertices[0] == end->Vertices[1])
			 ||(start->Vertices[2] == end->Vertices[1] && start->Vertices[0] == end->Vertices[2])
			 ||(start->Vertices[2] == end->Vertices[2] && start->Vertices[0] == end->Vertices[0])
			 ||(start->Vertices[2] == end->Vertices[1] && start->Vertices[0] == end->Vertices[0])
			 ||(start->Vertices[2] == end->Vertices[2] && start->Vertices[0] == end->Vertices[1])
			 ||(start->Vertices[2] == end->Vertices[0] && start->Vertices[0] == end->Vertices[2]))
			{
				start->edgeTarget[2] = end;
			}
		}
	}
}
