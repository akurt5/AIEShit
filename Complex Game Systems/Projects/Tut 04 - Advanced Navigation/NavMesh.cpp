#include "NavMesh.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <SOIL.h>


#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

class WithinRange : public Behaviour
{
public:

	WithinRange(float _Range) : Range2 (_Range*_Range){}
	virtual ~WithinRange(){}
	virtual bool Execute(Agent *_Agent)
	{
		float dist2 = 0;
		dist2 = glm::distance2(_Agent->GetPos(), _Agent->GetTarget()->GetPos());

		if (dist2 < Range2)
			return true;
		return false;
	}

	float Range2;
};
class RandomiseTarget : public Behaviour
{
public:
	RandomiseTarget(float _Radius) : Radius(_Radius){}
	virtual ~RandomiseTarget(){}
	virtual bool Execute(Agent *_Agent)
	{
		glm::vec3 Target(0);

		Target.xz = glm::circularRand(Radius);
		Agent *TargetEnemy = _Agent->GetTarget();

			TargetEnemy->SetPos(Target);
		return true;
	}
	float Radius;
};
class SeekTarget : public Behaviour
{
public:
	SeekTarget(float _Speed) : Speed(_Speed){}
	virtual ~SeekTarget(){}

	virtual bool  Execute(Agent *_Agent)
	{
		glm::vec3 Pos = _Agent->GetPos();

		Flag *TargetFlag = nullptr;
		for (auto Flag : _Agent->Flags)
		{
			float Len = 0, tLen;
			tLen = glm::length(Pos) - glm::length(Flag->GetPos());
			if(tLen < 0){tLen *= -1;}
			if(tLen > Len){Len = tLen;TargetFlag = Flag;}
		}
		if(TargetFlag != nullptr)
		{
			_Agent->SetTarget(TargetFlag->Position);
		}
	return true;
	}	
		float Speed;
};
class AttackTarget : public Behaviour
{
public:
	AttackTarget(float _Speed) : Speed(_Speed){}
	virtual ~AttackTarget(){}

	virtual bool  Execute(Agent *_Agent)
	{
		for (auto Flag : _Agent->Flags)
		{ 
			int check =0;
			if(Flag->GetController() != _Agent->MyTeam)
			{
				check ++;
			}
			if(check >1)
			{
				return false;
			}
		}
		
			glm::vec3 Pos;
			Agent *TargetEnemy = nullptr;
			Pos = _Agent->GetPos();
			for ( auto EnemyCurrent : _Agent->Enemies)
			{
				float Len = 0, tLen;

				tLen = glm::length(Pos) - glm::length(EnemyCurrent->GetPos());
				if(tLen < 0)
				{
					tLen *= -1;
				}
				if(tLen > Len)
				{
					Len = tLen;
					TargetEnemy = EnemyCurrent;
				}
			}
			if(TargetEnemy != nullptr)
			{
				_Agent->SetTarget(TargetEnemy);
			}
	return true;
	}	
		float Speed;
};


#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

	Scene::Scene()
	{

	}
	Scene::~Scene()
	{

	}
	bool Scene::onCreate(int a_argc, char* a_argv[]) 
	{
		srand(time(NULL));
		// initialise the Gizmos helper class
		Gizmos::create();

		// create a world-space matrix for a camera
		m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(20,20,0),glm::vec3(0,0,0), glm::vec3(0,1,0)) );

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

		uiBlueUp = SOIL_load_OGL_texture("../../Build/textures/blueup.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
		uiBlueDown = SOIL_load_OGL_texture("../../Build/textures/bluedown.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
		uiRedUp = SOIL_load_OGL_texture("../../Build/textures/redup.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);
		uiRedDown = SOIL_load_OGL_texture("../../Build/textures/reddown.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

		unsigned int vs = Utility::loadShader("../../Build/shaders/sponza.vert", GL_VERTEX_SHADER);
		unsigned int fs = Utility::loadShader("../../Build/shaders/sponza.frag", GL_FRAGMENT_SHADER);
		m_shader = Utility::createProgram(vs,0,0,0,fs);
		glDeleteShader(vs);
		glDeleteShader(fs);


		count = 0;

		Screen = new ShaderHandle();
		Screen->Load(2, "Screen.vert", "Screen.frag");

		bBlueUp = new Button(glm::vec2(25 , 25), glm::vec2(50));
		bBlueDown = new Button(glm::vec2(25, 100), glm::vec2(50));
		bRedUp = new Button(glm::vec2(100, 25), glm::vec2(50));
		bRedDown = new Button(glm::vec2(100 ,100), glm::vec2(50));

		Behaviour* Defend = new SeekTarget(10);
		Behaviour* Attack = new AttackTarget(10);
		Behaviour* Rand = new RandomiseTarget(10);
		Behaviour* Within = new WithinRange(0.5f);

		//																			A selector will select one (OR)
		//																			A sequence will go in sequence (AND)

		//																			IF	not within range	THEN	seek	ELSE	randomise target
		//																			IF	not attack			THEN	Above

		//																			If team has control seek go into attack mode otherwise seek out a flag and gain control


		Sequence* Kill = new Sequence();//AND																			if you are within range rand
		Kill->addchild(Within);
		Kill->addchild(Rand);

		Sequence* CanAttack = new Sequence();//AND																			if you dont have control defend
		CanAttack->addchild(Attack);
		CanAttack->addchild(Kill);


		Selector* Root = new Selector();//OR																			defend if you cant attack
		Root->addchild(CanAttack);
		Root->addchild(Defend);

		Agenda = Root;

		RedSize = 3;
		BlueSize = 3;



		Red = new Team();
		Blue = new Team();

		int FlagCount = 3;
		for (int i=0;i<FlagCount;i++)
		{
			Flags.emplace_back(new Flag());

		}
		Flags[0]->Position = glm::vec3(15, 0, 7);
		Flags[1]->Position = glm::vec3(0, 0, 0);
		Flags[2]->Position = glm::vec3(-15, 0, -7);

		for (int i=0;i<RedSize;++i)
		{
			Red->AddMember(m_Graph, "Red");
			glm::vec3 NewPos;
			NewPos.xz = glm::circularRand(20.0f);
			Red->Members[i]->SetPos(NewPos);

		}
		for (int i=0;i<BlueSize;++i)
		{
			Blue->AddMember(m_Graph, "Blue");

			glm::vec3 NewPos;
			NewPos.xz = glm::circularRand(20.0f);
			Blue->Members[i]->AgentPosition = NewPos;



		}
		for(int i=0;i<RedSize;++i)
		{
			Red->Members[i]->CalcEnemy(Blue->Members);
			Red->Members[i]->SetBehaviour(Agenda);
			Red->Members[i]->SetPos(Red->Members[i]->GiveScore(m_Graph, Red->Members[i]->AgentPosition)->Position);
		}
		for (int i=0;i<BlueSize;++i)
		{
			Blue->Members[i]->SetBehaviour(Agenda);
			Blue->Members[i]->CalcEnemy(Red->Members);
			Blue->Members[i]->SetPos(Blue->Members[i]->GiveScore(m_Graph, Blue->Members[i]->AgentPosition)->Position);

		}


		return true;
	}
void Scene::onUpdate(float a_deltaTime) 
	{
		// update our camera matrix using the keyboard/mouse
		Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

		// clear all gizmos from last frame
		Gizmos::clear();

		// add an identity matrix gizmo
		//Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );


		for (auto node : m_Graph)
		{
			Gizmos::addAABBFilled(node->Position, glm::vec3(0.005f), glm::vec4(1, 0, 0, 1));

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

		bBlueUp->Update();
		if (bBlueUp->IsActivated()){
			Blue->AddMember(m_Graph, "Blue");
			glm::vec3 NewPos;
			NewPos.xz = glm::circularRand(10.0f);
			Blue->Members.back()->SetPos(NewPos);
			Blue->Members.back()->SetPos(Blue->Members.back()->GiveScore(m_Graph, Blue->Members.back()->AgentPosition)->Position);

			Blue->Members.back()->SetTarget(Flags.back()->GetPos());
			Blue->Members.back()->SetBehaviour(Agenda);
			Blue->Members.back()->CalcEnemy(Red->Members);
		}
		bBlueDown->Update();
		if (bBlueDown->IsActivated()){
			if(Blue->Members.size() <= 1)
			{
				Blue->DelMember();
			}
		}
		bRedUp->Update();
		if (bRedUp->IsActivated()){
			Red->AddMember(m_Graph, "Red");
			glm::vec3 NewPos;
			NewPos.xz = glm::circularRand(10.0f);
			Red->Members.back()->SetPos(NewPos);
			Red->Members.back()->SetPos(Red->Members.back()->GiveScore(m_Graph, Red->Members.back()->AgentPosition)->Position);

			Red->Members.back()->SetTarget(Flags[0]->GetPos());
			Red->Members.back()->SetBehaviour(Agenda);
			Red->Members.back()->CalcEnemy(Blue->Members);
		}
		bRedDown->Update();
		if (bRedDown->IsActivated()){
			if(Red->Members.size() <= 1)
			{
				Red->DelMember();
			}
		}

		for (int i=0;i<Red->Members.size();++i)
		{
			Red->Members[i]->CalcEnemy(Blue->Members);
			Red->Members[i]->Flags = Flags;
			Red->Members[i]->update(a_deltaTime, glm::vec4(1, 0, 0, 1));
		}
		for (int i=0;i<Blue->Members.size();++i)
		{
			Blue->Members[i]->CalcEnemy(Red->Members);
			Blue->Members[i]->Flags = Flags;
			Blue->Members[i]->update(a_deltaTime, glm::vec4(0, 0, 1, 1));
		}

		for (auto i : Flags)
		{
			i->Update(Red, Blue);
		}
	}
	void Scene::onDraw() 
	{
		// clear the backbuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// get the view matrix from the world-space camera matrix
		glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );

		glUseProgram(Screen->m_shader);

		int location = glGetUniformLocation(Screen->m_shader, "Projection");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr( m_screenProjectionMatrix));

		GLint uDiffuseTexture1 = glGetUniformLocation(Screen->m_shader, "Texture");
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, uiBlueUp );
		glUniform1i( uDiffuseTexture1, 0 );
		bBlueUp->Draw();

		GLint uDiffuseTexture2 = glGetUniformLocation(Screen->m_shader, "Texture");
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, uiBlueDown );
		glUniform1i( uDiffuseTexture2, 0 );
		bBlueDown->Draw();

		GLint uDiffuseTexture3 = glGetUniformLocation(Screen->m_shader, "Texture");
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, uiRedUp );
		glUniform1i( uDiffuseTexture3, 0 );
		bRedUp->Draw();

		GLint uDiffuseTexture4 = glGetUniformLocation(Screen->m_shader, "Texture");
		glActiveTexture( GL_TEXTURE0 );
		glBindTexture( GL_TEXTURE_2D, uiRedDown );
		glUniform1i( uDiffuseTexture4, 0 );
		bRedDown->Draw();

		glUseProgram(m_shader);


		location = glGetUniformLocation(m_shader, "projectionView");
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr( m_projectionMatrix * viewMatrix ));



		if (glfwGetKey(m_window, GLFW_KEY_SPACE))
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
	void Scene::onDestroy()
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
		Application* app = new Scene();

		if (app->create("AIE - NavMesh",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
			app->run();

		// explicitly control the destruction of our application
		delete app;

		return 0;
	}
	void Scene::createOpenGLBuffers(FBXFile* a_fbx)
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
	void Scene::cleanupOpenGLBuffers(FBXFile* a_fbx)
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
	void Scene::BuildNavMesh(FBXMeshNode *a_Mesh, std::vector<NavNode*> &a_Graph)
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

			if(node->Position.y < 1)
			{
				a_Graph.push_back(node);
			}
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

		for(int i=0;i<a_Graph.size();i++)
		{
			for(int a=0;a<a_Graph.size();a++)
			{
				if(a_Graph[i]->Position == a_Graph[a]->Position)
				{
					if(i != a)
					{
						a_Graph.erase(a_Graph.erase(a_Graph.begin()+(i)));//																			get rid of useless nodes / any nodes with the same position
						i = 0;
					}
				}
			}

			int check = 0;
			for(int a=0;a<3;a++)
			{
				if(a_Graph[i]->edgeTarget[a] == nullptr)
				{
					check ++;
				}
			}
			if((check == 3)||(a_Graph[i]->Position.y >1))
			{
				a_Graph.erase(a_Graph.begin()+i);
				i=0;
			}
		}
	}

	
	void Scene::Pathtest(int _counter)
	{
		static bool Press = false;

		Gizmos::addTri(TestNode->Vertices[0], TestNode->Vertices[1],TestNode->Vertices[2], glm::vec4(1, 1, 1, 1));
		if(TestNode->edgeTarget[0] != nullptr)
		{
			Gizmos::addTri(TestNode->edgeTarget[0]->Vertices[0], TestNode->edgeTarget[0]->Vertices[1], TestNode->edgeTarget[0]->Vertices[2], glm::vec4(1, 0, 0, 1));
		}
		if(TestNode->edgeTarget[1] != nullptr)
		{
			Gizmos::addTri(TestNode->edgeTarget[1]->Vertices[0], TestNode->edgeTarget[1]->Vertices[1], TestNode->edgeTarget[1]->Vertices[2], glm::vec4(0, 1, 0, 1));
		}
		if(TestNode->edgeTarget[2] != nullptr)
		{
			Gizmos::addTri(TestNode->edgeTarget[2]->Vertices[0], TestNode->edgeTarget[2]->Vertices[1], TestNode->edgeTarget[2]->Vertices[2], glm::vec4(0, 0, 1, 1));
		}


		if((glfwGetKey(m_window, GLFW_KEY_R))&&(TestNode->edgeTarget[0] != nullptr)&&(!Press)){TestNode = TestNode->edgeTarget[0];Press = true;printf("r", "\n");}
		else if((glfwGetKey(m_window, GLFW_KEY_G))&&(TestNode->edgeTarget[1] != nullptr)&&(!Press)){TestNode = TestNode->edgeTarget[1];Press = true;printf("g", "\n");}
		else if((glfwGetKey(m_window, GLFW_KEY_B))&&(TestNode->edgeTarget[2] != nullptr)&&(!Press)){TestNode = TestNode->edgeTarget[2];Press = true;printf("b", "\n");}
		else if((!glfwGetKey(m_window, GLFW_KEY_R))&&(!glfwGetKey(m_window, GLFW_KEY_G))&&(!glfwGetKey(m_window, GLFW_KEY_B))&&(Press == true)) {Press = false;}

		//																			system("cls");
	}

