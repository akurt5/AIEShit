#include "NavMesh.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/norm.hpp>
#include <algorithm>

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

	
	//Closed.emplace_back(Open[3]);
	//delete Open[3];
	//Path(Open, Closed);//, Open[3], Open[4]);
	
	count = 0;

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



	//		Path(GetCurrentNode(glm::vec3(4, 0, 2)));
	if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_1))
	{
		if (!down)
		{
			count ++;
			down = true;
		}
	}
	else
	{
		down = false;
	}

	if (count == m_Graph.size()-1)
	{
		count = 0;
	}
	Pathtest(count);
	Path(glm::vec3(2, 0, 2), glm::vec3(10, 0, 2));

}
void NavMesh::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

	glUseProgram(m_shader);

	/*Shader.send("NAME","m4x4",m4x4);

	switch(type){
	case "m4x4":

		DO MATRIX CRAP

			break;
	}*/

	int location = glGetUniformLocation(m_shader, "projectionView");
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr( m_projectionMatrix * viewMatrix ));
	if (!glfwGetKey(m_window, GLFW_KEY_SPACE))
	{
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

	//																			get rid of useless naode / any nodes with the same position
	for(int i=0;i<a_Graph.size();i++)
	{
		for(int a=0;a<a_Graph.size();a++)
	{
			if(a_Graph[i]->Position == a_Graph[a]->Position)
			{
				if(i != a)
				{
					a_Graph.erase(a_Graph.erase(a_Graph.begin()+(i)));
				}
			}
		}
	}
}
NavMesh::NavNode* NavMesh::GetCurrentNode(glm::vec3 _Pos)
{
	//get current node / assign scores based on pos from start node
	/*for (int i=0;i<m_Graph.size();++i)
	{
		m_Graph[i]->Score = (glm::length(_Pos) - glm::length(m_Graph[i]->Position));
		
		if(m_Graph[i]->Score < 0)
		{
			m_Graph[i]->Score *= -1;
		}
		//std::cout<<m_Graph[i]->Score<<"   "<<m_Graph[i]<<'\n';
	}*/
	//																			sort list of nodes based on score
	//std::sort(m_Graph.begin(), m_Graph.end(), Compare());
	
	// :. list[0] must be current node
	return m_Graph[0];
	/*printf("Get Current Node Start\n");
	NavNode *Closest = m_Graph[30];
	NavNode *Endnode = m_Graph.back();
	//																			Determine the score for each node
	for (int i=0;i<m_Graph.size();i++)
	{
		m_Graph[i]->Score = (glm::length(m_Graph[i]->Position) - glm::length(Endnode->Position));
		if(m_Graph[i]->Score < 0)
		{m_Graph[i]->Score *= -1;}
	}

	for (int i=0;i<m_Graph.size();++i)
	{
		
		Closest = ScoreCompare(m_Graph[i], Closest);
	}
	printf("Get Current Node End\n");

	return Closest;*/
}
std::vector <NavMesh::NavNode*> NavMesh::Path(glm::vec3 _StartPos, glm::vec3 _TargetPos)
{
	
	CurrentNode = GetCurrentNode(_StartPos);
	EndNode = GetCurrentNode(_TargetPos);

	CurrentNode->Parent = nullptr;
	//do{
	/*if(CurrentNode->Position != EndNode->Position)
	{
		Open.emplace_back(CurrentNode);
	}
		

		for(int i=0;i<3;i++)
		{
			if(CurrentNode->edgeTarget[i] != nullptr)
			{
				bool Allg = true;
				for ( auto iterator : Closed)
				{
					if(Allg)
					{
						if(CurrentNode->edgeTarget[i]->Position != iterator->Position)
						{
							Allg = true;
						}
						else
						{
							Allg = false;
						}
					}
				}
				if(Closed.size() == 0){Allg = true;}
				for ( auto iterator : Open)
				{
					if (Allg)
					{
						if(CurrentNode->edgeTarget[i]->Position != iterator->Position)
						{
							Allg = true;
						}
						else
						{
							Allg = false;
						}
					}
				}
				if (Allg)
				{
					Open.emplace_back(CurrentNode->edgeTarget[i]);
					CurrentNode->edgeTarget[i]->Parent = CurrentNode;
				}
			}		
		}
		
		//																			attempt at cleaning the closed list and preventing duplicates
		for (auto Iterator : Closed )
		{
			if(Iterator->Position != CurrentNode->Position)
			{
				Closed.emplace_back(CurrentNode);
			}
		}

		Open.erase(Open.begin());

		CurrentNode = ScoreCompare(CurrentNode->edgeTarget[0], ScoreCompare(CurrentNode->edgeTarget[1], CurrentNode->edgeTarget[2]));

	}while(CurrentNode->Position != EndNode->Position);

	PathList.emplace_back(CurrentNode);

	do{
		PathList.emplace(PathList.begin(), CurrentNode->Parent);
		CurrentNode = CurrentNode->Parent;
	}while(CurrentNode->Parent != nullptr);

	for (int i=0;i<PathList.size();i++)
	{
		Gizmos::addTri(PathList[i]->Vertices[0].xyz, PathList[i]->Vertices[1].xyz, PathList[i]->Vertices[3].xyz, glm::vec4(0.8, 0, 0.8, 1));
	}
	return PathList;*/
	return m_Graph;
}
/*{
	printf("Path Start\n");

	GetCurrentNode(_Startnode->Position);

	//																			Start and End nodes are determined
	//NavNode *Startnode = m_Graph[2];
	NavNode *Endnode = m_Graph[5];
	//																			CurrentNode is our iterator
	NavNode *CurrentNode = m_Graph[30];//																			_Startnode; //										GetCurrent Node.

	

	//																			Startnode is added to open
	std::vector<NavNode*> Open(1, CurrentNode);

	//																			The Startnode is added to the closed list
	std::vector<NavNode*> Closed;
	printf("Path Top of first DO\n");

	do{ 
		Gizmos::addTri(CurrentNode->Vertices[0], CurrentNode->Vertices[1], CurrentNode->Vertices[2], glm::vec4(1, 0, 1, 1));
		
		//		   																	while we havent found the end chose the shortest path
		//																			Adds current node to the back of the vector for easy access
		//if (CurrentNode != Endnode)
		//{
			Closed.emplace_back(CurrentNode);
		//}
		//else
		//	continue;//											good enough fo now
		for (int i=0;i<3;++i)
		{
			if(CurrentNode->edgeTarget[i] != nullptr)
			{
				int check = 0;
				for (int a=0;a<Closed.size();a++)
				{
					
					if(CurrentNode->edgeTarget[i] != Closed[a])
					{
						check ++;
					}
					
				}
				if (check == Closed.size())
				{
					Open.push_back(CurrentNode->edgeTarget[i]);
				}
			}
			else
			{
				std::cout<<"nullptr   "<<CurrentNode<<"\n";
			}
		}
		//																			Set parent as back of closed list
		CurrentNode->Parent = Closed.back();
		if(Open.size() > 1)
		{
			Open.erase(Open.begin());
		}
		else
		{
			std::cout<<"";
		}
		//																			Determine which of the neighbouring nodes has the lowest score
		//												Old Codes				CurrentNode = ScoreCompare(CurrentNode->edgeTarget[0], ScoreCompare(CurrentNode->edgeTarget[1], CurrentNode->edgeTarget[2]));
		
		//																			Sort Open based on score
		std::sort(Open.begin(), Open.end(), Compare());
		CurrentNode = Open[0];
		


		
	}while(CurrentNode != Endnode);

	//																			Create a vector to store the path
	std::vector<NavNode*> PathList;
	printf("Path Top of second DO\n");

	do{
		//																			Adds the parents of the path items in reverse order or something so the list is in the correct order
		PathList.emplace(PathList.begin(), CurrentNode);
		//																			current node becomes the parent to do the whole iteration thing
		CurrentNode = CurrentNode->Parent;
		//																			do all that until the path gets back to the start
	}while(CurrentNode->Parent != _Startnode);
	printf("Path End\n");
	
}*/
	
void NavMesh::Pathtest(int _counter)
{
	Gizmos::addTri(glm::vec3(m_Graph[_counter]->Vertices[0].x, m_Graph[_counter]->Vertices[0].y + 0.5f, m_Graph[_counter]->Vertices[0].z), 
				   glm::vec3(m_Graph[_counter]->Vertices[1].x, m_Graph[_counter]->Vertices[1].y + 0.5f, m_Graph[_counter]->Vertices[1].z), 
				   glm::vec3(m_Graph[_counter]->Vertices[2].x, m_Graph[_counter]->Vertices[2].y + 0.5f, m_Graph[_counter]->Vertices[2].z), glm::vec4(0.5f, 0.5f, 0.5f, 1));
	if (m_Graph[_counter]->edgeTarget[0] != nullptr)
	{
		Gizmos::addTri(glm::vec3(m_Graph[_counter]->edgeTarget[0]->Vertices[0].x, m_Graph[_counter]->edgeTarget[0]->Vertices[0].y + 0.5f, m_Graph[_counter]->edgeTarget[0]->Vertices[0].z), 
				       glm::vec3(m_Graph[_counter]->edgeTarget[0]->Vertices[1].x, m_Graph[_counter]->edgeTarget[0]->Vertices[1].y + 0.5f, m_Graph[_counter]->edgeTarget[0]->Vertices[1].z), 
				       glm::vec3(m_Graph[_counter]->edgeTarget[0]->Vertices[2].x, m_Graph[_counter]->edgeTarget[0]->Vertices[2].y + 0.5f, m_Graph[_counter]->edgeTarget[0]->Vertices[2].z), glm::vec4(1, 0, 0, 0.5));
	}
	if (m_Graph[_counter]->edgeTarget[1] != nullptr)
	{
		Gizmos::addTri(glm::vec3(m_Graph[_counter]->edgeTarget[1]->Vertices[0].x, m_Graph[_counter]->edgeTarget[1]->Vertices[0].y + 0.5f, m_Graph[_counter]->edgeTarget[1]->Vertices[0].z), 
				       glm::vec3(m_Graph[_counter]->edgeTarget[1]->Vertices[1].x, m_Graph[_counter]->edgeTarget[1]->Vertices[1].y + 0.5f, m_Graph[_counter]->edgeTarget[1]->Vertices[1].z), 
				       glm::vec3(m_Graph[_counter]->edgeTarget[1]->Vertices[2].x, m_Graph[_counter]->edgeTarget[1]->Vertices[2].y + 0.5f, m_Graph[_counter]->edgeTarget[1]->Vertices[2].z), glm::vec4(0, 1, 0, 0.5));
	}
	if (m_Graph[_counter]->edgeTarget[2] != nullptr)
	{
		Gizmos::addTri(glm::vec3(m_Graph[_counter]->edgeTarget[2]->Vertices[0].x, m_Graph[_counter]->edgeTarget[2]->Vertices[0].y + 0.5f, m_Graph[_counter]->edgeTarget[2]->Vertices[0].z), 
				       glm::vec3(m_Graph[_counter]->edgeTarget[2]->Vertices[1].x, m_Graph[_counter]->edgeTarget[2]->Vertices[1].y + 0.5f, m_Graph[_counter]->edgeTarget[2]->Vertices[1].z), 
				       glm::vec3(m_Graph[_counter]->edgeTarget[2]->Vertices[2].x, m_Graph[_counter]->edgeTarget[2]->Vertices[2].y + 0.5f, m_Graph[_counter]->edgeTarget[2]->Vertices[2].z), glm::vec4(0, 0, 1, 0.5));
	}
	std::cout<<_counter<<"  "<<m_Graph[_counter]<<'\n';
	//																			system("cls");
}
