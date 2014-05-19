#include "Game_Physics_Application.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <vector>

using namespace physx;

PxFoundation* g_PhysicsFoundation = nullptr;
PxPhysics* g_Physics = nullptr;
PxScene* g_PhysicsScene = nullptr;

PxDefaultErrorCallback gDefaultErrorCallback;
PxDefaultAllocator gDefaultAllocatorCallback;
PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
PxMaterial* g_PhysicsMaterial = nullptr;
PxCooking* g_PhysicsCooker = nullptr;
std::vector<PxRigidDynamic*> g_PhysXActors;


#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Game_Physics_Application::Game_Physics_Application()
{

}

Game_Physics_Application::~Game_Physics_Application()
{

}

bool Game_Physics_Application::onCreate(int a_argc, char* a_argv[]) 
{
	// initialise the Gizmos helper class
	Gizmos::create();
	srand(time(NULL));

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	
	setUpPhysXTutorial();
	setUpVisualDebugger();
	tutorial_1();
	

	return true;
}

void Game_Physics_Application::onUpdate(float a_deltaTime) 
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	// add an identity matrix gizmo
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	// add a 20x20 grid on the XZ-plane
	for ( int i = 0 ; i < 21 ; ++i )
	{
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();

	upDatePhysx();
}

void Game_Physics_Application::onDraw() 
{
	//																			clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//																			get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//																			draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Game_Physics_Application::onDestroy()
{
	//																			clean up anything we created
	Gizmos::destroy();
	cleanUpPhsyx();
}

//																			main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	//																			explicitly control the creation of our application
	Application* app = new Game_Physics_Application();
	
	if (app->create("AIE - Game Physics Application",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	//																			explicitly control the destruction of our application
	delete app;

	return 0;
}
void Game_Physics_Application::setUpPhysXTutorial()
{
	PxAllocatorCallback *myCallback = new myAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,*myCallback, gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, PxTolerancesScale());
	g_PhysicsCooker = PxCreateCooking(PX_PHYSICS_VERSION, *g_PhysicsFoundation, PxCookingParams(PxTolerancesScale()));
	PxInitExtensions(*g_Physics);
	//																			create physics material
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f,0.5f,0.6f);
	PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0,-30.0f,0);
	sceneDesc.filterShader = gDefaultFilterShader;
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);

	if(g_PhysicsScene)
	{
		std::cout << "start physx scene2";
	}
}
void Game_Physics_Application::cleanUpPhsyx()
{
	g_PhysicsCooker->release();
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();
}
void Game_Physics_Application::upDatePhysx()
{
	g_PhysicsScene->simulate( 1/60.f );
	while (g_PhysicsScene->fetchResults() == false)
	{
	// don’t need to do anything here yet but we still need to do the fetch
	}
}
void Game_Physics_Application::AddBox(glm::vec3 _Pos)
{
	float density = 10;
	PxBoxGeometry box(2,2,2);
	PxTransform transform(PxVec3(_Pos.x,_Pos.y,_Pos.z));
	PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box,*g_PhysicsMaterial, density);	
	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);
	//add it to our copy of the scene
	g_PhysXActors.push_back(dynamicActor);
}
void Game_Physics_Application::tutorial_1()
{
	//add a plane
	PxTransform pose = PxTransform( PxVec3(0.0f, 0, 0.0f),PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
	PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose,PxPlaneGeometry(), *g_PhysicsMaterial);
	//add it to the physX scene
	g_PhysicsScene->addActor(*plane);

	//add a box
	for(int i=0;i<100;i++)
	{
		AddBox(glm::vec3(rand()%100 - 50, rand()%10, rand()%100 - 50));
	}
}
void Game_Physics_Application::setUpVisualDebugger()
{
  /// check if PvdConnection manager is available on this platform
  if (NULL == g_Physics->getPvdConnectionManager())
      return;
  // setup connection parameters
  const char*     pvd_host_ip = "127.0.0.1";  // IP of the PC which is running PVD
  int             port        = 5425;         // TCP port to connect to, where PVD is listening
  unsigned int    timeout     = 100;          // timeout in milliseconds to wait for PVD to respond,
                                              // consoles and remote PCs need a higher timeout.
  PxVisualDebuggerConnectionFlags connectionFlags = PxVisualDebuggerConnectionFlag::Debug
      | PxVisualDebuggerConnectionFlag::Profile | PxVisualDebuggerConnectionFlag::Memory;
  // and now try to connect
	    PxVisualDebuggerExt::createConnection(g_Physics->getPvdConnectionManager(),
			pvd_host_ip, port, timeout, connectionFlags);
  //    pvd_host_ip, port, timeout, connectionFlags));
}