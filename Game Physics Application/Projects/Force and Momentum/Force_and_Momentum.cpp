#include "Force_and_Momentum.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <PxPhysicsAPI.h>
#include <vector>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

physx::PxFoundation* g_PhysicsFoundation =nullptr;
physx::PxPhysics* g_Physics = nullptr;
physx::PxScene* g_PhysicsScene = nullptr;
physx::PxDefaultErrorCallback gDefaultErrorCallback;
physx::PxDefaultAllocator gDefaultAllocatorCallback;
physx::PxSimulationFilterShader gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
physx::PxMaterial* g_PhysicsMaterial = nullptr;
physx::PxCooking* g_PhysicsCooker = nullptr;
std::vector<physx::PxRigidDynamic*> g_PhysXActors;

Force_and_Momentum::Force_and_Momentum(){}
Force_and_Momentum::~Force_and_Momentum(){}
class BallClass
{
public:
	BallClass(){}
	~BallClass(){}


};
class myAllocator: public physx::PxAllocatorCallback{
public:
	virtual ~myAllocator() {}
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line){
		void* pointer = _aligned_malloc(size,16);
		return pointer;
	}
	virtual void deallocate(void* ptr){
		_aligned_free(ptr);
	}
};
void setUpPhysXTutorial(){
	physx::PxAllocatorCallback *myCallback = new myAllocator();
	g_PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,*myCallback, gDefaultErrorCallback);
	g_Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *g_PhysicsFoundation, physx::PxTolerancesScale());
	g_PhysicsCooker = PxCreateCooking(PX_PHYSICS_VERSION, *g_PhysicsFoundation, physx::PxCookingParams(physx::PxTolerancesScale()));
	PxInitExtensions(*g_Physics);
	//create physics material
	g_PhysicsMaterial = g_Physics->createMaterial(0.5f,0.5f,0.6f);
	physx::PxSceneDesc sceneDesc(g_Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0,-30.0f,0);
	sceneDesc.filterShader = gDefaultFilterShader;
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	g_PhysicsScene = g_Physics->createScene(sceneDesc);

	if(g_PhysicsScene) {
		printf("start physx scene2\n");
	}
}
void cleanUpPhsyx() {
	g_PhysicsCooker->release();
	g_PhysicsScene->release();
	g_Physics->release();
	g_PhysicsFoundation->release();
}
void upDatePhysx(){
	g_PhysicsScene->simulate( 1/120.f );
	while (g_PhysicsScene->fetchResults() == false) {
	// don’t need to do anything here yet but we still need to do the fetch
	}
}
void AddPlane(glm::vec3 v3_Facing){
	//add a plane
	physx::PxTransform pose = physx::PxTransform( physx::PxVec3(0,0,0),physx::PxQuat(physx::PxHalfPi * 0.95f, physx::PxVec3(0.0f, 0.0f, 1.0f)));
	physx::PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, physx::PxPlaneGeometry(), *g_PhysicsMaterial);
	//add it to the physX scene
	g_PhysicsScene->addActor(*plane);
}
void AddBox(const glm::vec3& v3_Transform, const glm::vec3& v3_Dimentions,float f_density) {
	//add a box
	physx::PxBoxGeometry box(v3_Dimentions.x,v3_Dimentions.y,v3_Dimentions.z);
	physx::PxTransform transform(physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z));
	physx::PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box,*g_PhysicsMaterial, f_density); 
	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);
	//add it to our copy of the scene
	g_PhysXActors.push_back(dynamicActor);
}
void setUpVisualDebugger() {
	// check if PvdConnection manager is available on this platform
	if (NULL == g_Physics->getPvdConnectionManager())
		return;
	// setup connection parameters
	const char* pvd_host_ip = "127.0.0.1"; // IP of the PC which is running PVD
	int port = 5425; // TCP port to connect to, where PVD is listening
	unsigned int timeout = 100; // timeout in milliseconds to wait for PVD to respond,
	// consoles and remote PCs need a higher timeout.
	physx::PxVisualDebuggerConnectionFlags connectionFlags = physx::PxVisualDebuggerConnectionFlag::Debug | physx::PxVisualDebuggerConnectionFlag::Profile | physx::PxVisualDebuggerConnectionFlag::Memory;
	// and now try to connect
	physx::PxVisualDebuggerExt::createConnection(g_Physics->getPvdConnectionManager(),pvd_host_ip, port, timeout, connectionFlags);
	// pvd_host_ip, port, timeout, connectionFlags));
}
bool Force_and_Momentum::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	//!- TUTORIAL
	setUpPhysXTutorial();
	setUpVisualDebugger();
	AddPlane(glm::vec3(0,1,0));
	AddBox(glm::vec3(0,0,0),glm::vec3(1),100);
	//!- TUTORIAL


	BallClass *newBall;
	physicsScene = new DIYPhysicScene();
	physicsScene->gravity = glm::vec3(0,-10,0);
	physicsScene->timeStep = .1f;
	//add four balls to our simulation
	newBall = new BallClass(glm::vec3(-40,0,0),PI/4.0f,30.0f,0,3.0f,1,glm::vec4(1,0,0,1)); 
	physicsScene->addActor(newBall);
	newBall = new BallClass(glm::vec3(-40,0,0),PI/3.0f,30.0f,0,3.0f,1,glm::vec4(0,1,0,1)); 
	physicsScene->addActor(newBall);
	newBall = new BallClass(glm::vec3(-40,0,0),PI/2.0f,30.0f,0,3.0f,1,glm::vec4(0,0,1,1));  
	physicsScene->addActor(newBall);
	newBall = new BallClass(glm::vec3(-40,0,0),0,30.0f,0,3.0f,1,glm::vec4(1,1,0,1));  
	physicsScene->addActor(newBall);
	return true;
}

void Force_and_Momentum::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}
	
	//!- TUTORIAL
	upDatePhysx();
	//!- TUTORIAL
	
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		quit();
	}
}

void Force_and_Momentum::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//!- TUTORIAL
	
	//!- TUTORIAL
	
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Force_and_Momentum::onDestroy(){
	//!- TUTORIAL
	cleanUpPhsyx();
	//!- TUTORIAL
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new Force_and_Momentum();
	if (app->create("AIE - Force and Momentum",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}

