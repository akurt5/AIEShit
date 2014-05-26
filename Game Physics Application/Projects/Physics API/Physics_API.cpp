#include "Physics_API.h"
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

Physics_API::Physics_API(){}
Physics_API::~Physics_API(){}

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

void AddPlane(glm::vec3 v3_Facing){
	//add a plane
	physx::PxTransform pose = physx::PxTransform( physx::PxVec3(0,0,0),physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));
	physx::PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, physx::PxPlaneGeometry(), *g_PhysicsMaterial);
	//add it to the physX scene
	g_PhysicsScene->addActor(*plane);
}

void AddBox(const glm::vec3& v3_Transform, const glm::vec3& v3_Dimensions,float f_density) {
	//add a box
	physx::PxBoxGeometry box(v3_Dimensions.x,v3_Dimensions.y,v3_Dimensions.z);
	physx::PxTransform transform(physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z));
	physx::PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, box,*g_PhysicsMaterial, f_density); 
	//add it to the physX scene
	g_PhysicsScene->addActor(*dynamicActor);
	//add it to our copy of the scene
	g_PhysXActors.push_back(dynamicActor);
}

void AddBall(const glm::vec3& v3_Transform, const float _Dimensions,float f_density) {
	//add a ball
	physx::PxSphereGeometry ball(_Dimensions);
	physx::PxTransform transform(physx::PxVec3(v3_Transform.x,v3_Transform.y,v3_Transform.z));
	physx::PxRigidDynamic* dynamicActor = PxCreateDynamic(*g_Physics, transform, ball,*g_PhysicsMaterial, f_density); 
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

void addBox(physx::PxShape* pShape,physx::PxRigidDynamic* actor) 
{
	//get the geometry for this PhysX collision volume
	physx::PxBoxGeometry geometry;
	float width = 1, height = 1, length = 1;
	bool status = pShape->getBoxGeometry(geometry);
	if(status)
	{
		width = geometry.halfExtents.x;
		height = geometry.halfExtents.y;
		length = geometry.halfExtents.z;
	}
	//get the transform for this PhysX collision volume
	physx::PxMat44 m(physx::PxShapeExt::getGlobalPose(*pShape));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
	m.column1.x, m.column1.y, m.column1.z, m.column1.w,
	m.column2.x, m.column2.y, m.column2.z, m.column2.w,
	m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	glm::vec3 position; 
	//get the position out of the transform
	position.x = m.getPosition().x;
	position.y = m.getPosition().y;
	position.z = m.getPosition().z;
	glm::vec3 extents = glm::vec3(width,height,length);
	glm::vec4 colour = glm::vec4(1,0,0,1);
	//create our box gizmo
	Gizmos::addAABBFilled(position,extents,colour,&M);
}

void addWidget(physx::PxShape* shape,physx::PxRigidDynamic* actor) 
{ 
    physx::PxGeometryType::Enum type = shape->getGeometryType();
    switch(type) 
    {  
		case physx::PxGeometryType::eBOX:
			addBox(shape,actor);
		break;
		//case PxGeometryType::eSPHERE:
		//	addSphere(shape,actor);
		//break;
    } 
}
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
	// Add widgets to represent all the phsyX actors which are in the scene
	for (auto actor:g_PhysXActors)
	{
		physx::PxU32 nShapes = actor->getNbShapes();
		physx::PxShape** shapes = new physx::PxShape*[nShapes];
		actor->getShapes(shapes, nShapes);
		// Render all the shapes in the physx actor (for early tutorials there is just one)
		while (nShapes--)
		{
			addWidget(shapes[nShapes],actor); 
		}
		delete [] shapes;
	}
}




void Gizmos::addGrid(const glm::vec3& a_center, const glm:: mat4* a_transform,int increments,float size)
{
	int halfIncrements = increments/2;
	for ( int i = 0 ; i < increments+1 ; ++i )
	{
		glm::vec4 colour =  i == halfIncrements ? glm::vec4(1,0,1,1) : glm::vec4(0,0,0,1);
		glm::vec3 l1Start = glm::vec3((-halfIncrements + i)*size, 0, halfIncrements*size);
		glm::vec3 l1End = glm::vec3((-halfIncrements + i)*size, 0, -halfIncrements*size);
		glm::vec3 l2Start = glm::vec3(halfIncrements*size, 0, (-halfIncrements + i)*size);
		glm::vec3 l2End = glm::vec3(-halfIncrements*size, 0, (-halfIncrements + i)*size);
	
		l1Start = (*a_transform * glm::vec4(l1Start,0)).xyz + a_center;
		l1End = (*a_transform * glm::vec4(l1End,0)).xyz + a_center;
		l2Start = (*a_transform * glm::vec4(l2Start,0)).xyz + a_center;
		l2End = (*a_transform * glm::vec4(l2End,0)).xyz + a_center;
		Gizmos::addLine( l1Start, l1End, colour);
		Gizmos::addLine(l2Start, l2End, colour);
	}
}

bool Physics_API::onCreate(int a_argc, char* a_argv[]) {
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
	//!- TUTORIAL
	for ( int x = -5; x < 5; x++)
	{
		for ( int y = 1; y < 10; y++)
		{
			for ( int z = -5; z < 5; z++)
			{
				AddBox(glm::vec3(x,y-0.499,z),glm::vec3(0.5),100);
			}

		}
	}
	
	
	timer = 0;
	return true;
}

void Physics_API::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}
	
	
	if(timer <= 0.0f)
	{
		timer = 1/20.0f;
		float power = 100;

		if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_5))
		{
			//add a box
			physx::PxSphereGeometry bullet(1);
			physx::PxTransform transform(physx::PxVec3(m_cameraMatrix[3].x, m_cameraMatrix[3].y, m_cameraMatrix[3].z));
			physx::PxRigidDynamic* Projectile = PxCreateDynamic(*g_Physics, transform, bullet,*g_PhysicsMaterial, 100); 
			//																			add force
			glm::vec3 Force(m_cameraMatrix[2] * -power);
			Projectile->setLinearVelocity(physx::PxVec3(Force.x, Force.y, Force.z));//								, physx::PxForceMode::eVELOCITY_CHANGE);
			//add it to the physX scene
			g_PhysicsScene->addActor(*Projectile);
			//add it to our copy of the scene
			g_PhysXActors.push_back(Projectile);
			
		}
		if (glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_4))
		{
			//add a box
			physx::PxBoxGeometry bullet(1, 1, 1);
			physx::PxTransform transform(physx::PxVec3(m_cameraMatrix[3].x, m_cameraMatrix[3].y, m_cameraMatrix[3].z));
			physx::PxRigidDynamic* Projectile = PxCreateDynamic(*g_Physics, transform, bullet,*g_PhysicsMaterial, 100); 
			//																			add force
			glm::vec3 Force(m_cameraMatrix[2] * -power);
			Projectile->setLinearVelocity(physx::PxVec3(Force.x, Force.y, Force.z));//								, physx::PxForceMode::eVELOCITY_CHANGE);
			//add it to the physX scene
			g_PhysicsScene->addActor(*Projectile);
			//add it to our copy of the scene
			g_PhysXActors.push_back(Projectile);
		}
			
	}
	else if (timer >0)
	{
		timer -= a_deltaTime;
	}

	//!- TUTORIAL
	upDatePhysx();
	//!- TUTORIAL

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		quit();
	}
}

void Physics_API::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//!- TUTORIAL
	
	//!- TUTORIAL
	
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Physics_API::onDestroy(){
	//!- TUTORIAL
	cleanUpPhsyx();
	//!- TUTORIAL
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new Physics_API();
	if (app->create("AIE - Physics API",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}

