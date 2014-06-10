#include "DIYPhisicsClass.h"




using namespace physx;

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

PhysicsObject::PhysicsObject(float _Mass = NULL, float _Density = NULL, float _Radius = NULL, glm::vec3 _Force = VEC3NULL, glm::vec3 _Velocity = VEC3NULL, glm::vec3 _Dimensions = VEC3NULL, glm::vec3 _Rotation = VEC3NULL, glm::vec3 _Position = VEC3NULL, glm::vec4 _Colour = glm::vec4(1, 0, 0, 1))
{
	Mass		= _Mass;
	Density		= _Density;
	Radius		= _Radius;
	Force		= _Force;
	Velocity	= _Velocity;
	Position	= _Position;
	Rotation	= _Rotation;
	Dimensions	= _Dimensions;
	Colour		= _Colour;
}
PhysicsObject::~PhysicsObject(void){}
void PhysicsObject::Load(float _Mass = NULL, float _Density = NULL, float _Radius = NULL, glm::vec3 _Force = VEC3NULL, glm::vec3 _Velocity = VEC3NULL, glm::vec3 _Dimensions = VEC3NULL, glm::vec3 _Rotation = VEC3NULL, glm::vec3 _Position = VEC3NULL, glm::vec4 _Colour = glm::vec4(1, 0, 0, 1))
{
	Mass		= _Mass;
	Density		= _Density;
	Radius		= _Radius;
	Force		= _Force;
	Velocity	= _Velocity;
	Position	= _Position;
	Rotation	= _Rotation;
	Dimensions	= _Dimensions;
	Colour		= _Colour;
}
void PhysicsObject::Update()
{

}
void PhysicsObject::Draw()
{

}
void PhysicsObject::Unload()
{


}
DIYPhisicsHandle::DIYPhisicsHandle(void){}
DIYPhisicsHandle::~DIYPhisicsHandle(void){}

void DIYPhisicsHandle::Load()
{	
	PhysicsScene = nullptr;
	PhysicsFoundation = nullptr;
	Physics = nullptr;
	DefaultFilterShader = physx::PxDefaultSimulationFilterShader;
	PhysicsMaterial = nullptr;
	PhysicsCooker = nullptr;

	SetUpPhysXTutorial();
	SetUpVisualDebugger();
}
void DIYPhisicsHandle::Update(GLFWwindow *_Window, glm::mat4 _Camera)
{
	PhysicsScene->simulate( 1/120.f );
	while (PhysicsScene->fetchResults() == false) {
	// don’t need to do anything here yet but we still need to do the fetch
	}
	// Add widgets to represent all the phsyX actors which are in the scene
	for(auto Actor : Actors)
	{
		physx::PxU32 nShapes = Actor->RigidDynamic->getNbShapes();
		physx::PxShape** shapes = new physx::PxShape*[nShapes];
		Actor->RigidDynamic->getShapes(shapes, nShapes);
		// Render all the shapes in the physx actor (for early tutorials there is just one)
		while (nShapes--)
		{
			AddWidget(shapes[nShapes],Actor); 
		}
		delete [] shapes;

		//																			Actor->Update();
	}
	Shoot(_Window, _Camera);
}
void DIYPhisicsHandle::Draw ()
{
	for(auto Actor : Actors)
	{
		
	}
}
void DIYPhisicsHandle::Unload()
{
	PhysicsCooker->release();
	PhysicsScene->release();
	Physics->release();
	PhysicsFoundation->release();
	
}
void DIYPhisicsHandle::AddBox(PhysicsObject *_Actor)
{
	//add a box
	physx::PxBoxGeometry box(_Actor->Dimensions.x, _Actor->Dimensions.y,_Actor->Dimensions.z);
	physx::PxTransform transform(physx::PxVec3(_Actor->Position.x,_Actor->Position.y,_Actor->Position.z));
	_Actor->RigidDynamic = PxCreateDynamic(*Physics, transform, box,*PhysicsMaterial, _Actor->Density); 
	//add it to the physX scene
	PhysicsScene->addActor(*_Actor->RigidDynamic);
	//add it to our copy of the scene
	Actors.push_back(_Actor);
}
void DIYPhisicsHandle::AddSphere(PhysicsObject *_Actor)
{
	//add a box
	physx::PxSphereGeometry sphere(_Actor->Radius);
	physx::PxTransform transform(physx::PxVec3(_Actor->Position.x,_Actor->Position.y,_Actor->Position.z));
	_Actor->RigidDynamic = PxCreateDynamic(*Physics, transform, sphere,*PhysicsMaterial, _Actor->Density); 
	//add it to the physX scene
	PhysicsScene->addActor(*_Actor->RigidDynamic);
	//add it to our copy of the scene
	Actors.push_back(_Actor);
}
void DIYPhisicsHandle::AddCapsule(PhysicsObject *_Actor)
{
	//add a box
	physx::PxCapsuleGeometry capsule(_Actor->Radius, _Actor->Dimensions.y / 2);
	physx::PxTransform transform(physx::PxVec3(_Actor->Position.x,_Actor->Position.y,_Actor->Position.z));
	_Actor->RigidDynamic = PxCreateDynamic(*Physics, transform, capsule,*PhysicsMaterial, _Actor->Density); 
	//add it to the physX scene
	PhysicsScene->addActor(*_Actor->RigidDynamic);
	//add it to our copy of the scene
	Actors.push_back(_Actor);
}
void DIYPhisicsHandle::AddBox	(physx::PxShape* shape, PhysicsObject* _Actor)
{
	//get the geometry for this PhysX collision volume
	PxBoxGeometry geometry;
	float width = 1, height = 1, length = 1;
	bool status = shape->getBoxGeometry(geometry);
	if(status)
	{
		width = geometry.halfExtents.x;
		height = geometry.halfExtents.y;
		length = geometry.halfExtents.z;
	}
	//get the transform for this PhysX collision volume
	physx::PxMat44 m(physx::PxShapeExt::getGlobalPose(*shape));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
	m.column1.x, m.column1.y, m.column1.z, m.column1.w,
	m.column2.x, m.column2.y, m.column2.z, m.column2.w,
	m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	//get the position out of the transform
	_Actor->Position.x = m.getPosition().x;
	_Actor->Position.y = m.getPosition().y;
	_Actor->Position.z = m.getPosition().z;
	_Actor->Dimensions = glm::vec3(width,height,length);
	glm::vec4 colour = glm::vec4(1,0,0,1);
	//create our box gizmo
	Gizmos::addAABBFilled(_Actor->Position, _Actor->Dimensions, _Actor->Colour,&M);
}
void DIYPhisicsHandle::AddSphere	(physx::PxShape* shape, PhysicsObject* _Actor)
{
	//get the geometry for this PhysX collision volume
	physx::PxSphereGeometry geometry;
	bool status = shape->getSphereGeometry(geometry);
	//get the transform for this PhysX collision volume
	physx::PxMat44 m(physx::PxShapeExt::getGlobalPose(*shape));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
	m.column1.x, m.column1.y, m.column1.z, m.column1.w,
	m.column2.x, m.column2.y, m.column2.z, m.column2.w,
	m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	//get the position out of the transform
	_Actor->Position.x = m.getPosition().x;
	_Actor->Position.y = m.getPosition().y;
	_Actor->Position.z = m.getPosition().z;
	//create our box gizmo
	Gizmos::addSphere(_Actor->Position,5, 5, _Actor->Radius, _Actor->Colour,&M);
}
void DIYPhisicsHandle::AddPlane	(physx::PxShape* shape, PhysicsObject* _Actor)
{
	physx::PxTransform pose = physx::PxTransform( physx::PxVec3(0,0,0),physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f)));
	physx::PxRigidStatic* plane = PxCreateStatic(*Physics, pose, physx::PxPlaneGeometry(), *PhysicsMaterial);
	//add it to the physX scene
	PhysicsScene->addActor(*plane);
}
void DIYPhisicsHandle::AddCapsule	(physx::PxShape* shape, PhysicsObject* _Actor)
{
	//get the geometry for this PhysX collision volume
	physx::PxCapsuleGeometry geometry;
	bool status = shape->getCapsuleGeometry(geometry);
	//get the transform for this PhysX collision volume
	physx::PxMat44 m(physx::PxShapeExt::getGlobalPose(*shape));
	glm::mat4 M(m.column0.x, m.column0.y, m.column0.z, m.column0.w,
	m.column1.x, m.column1.y, m.column1.z, m.column1.w,
	m.column2.x, m.column2.y, m.column2.z, m.column2.w,
	m.column3.x, m.column3.y, m.column3.z, m.column3.w);
	//get the position out of the transform
	_Actor->Position.x = m.getPosition().x;
	_Actor->Position.y = m.getPosition().y;
	_Actor->Position.z = m.getPosition().z;
	//create our box gizmo
	Gizmos::addSphere(glm::vec3(_Actor->Position.x, _Actor->Position.y + _Actor->Radius, _Actor->Position.z),5, 5, _Actor->Radius, _Actor->Colour,&M);
	Gizmos::addCylinderFilled(_Actor->Position, _Actor->Radius, _Actor->Radius, 5, _Actor->Colour);
	Gizmos::addSphere(glm::vec3(_Actor->Position.x, _Actor->Position.y - _Actor->Radius, _Actor->Position.z),5, 5, _Actor->Radius, _Actor->Colour,&M);
}
void DIYPhisicsHandle::AddWidget(PxShape* shape,PhysicsObject* actor) 
{ 
    PxGeometryType::Enum type = shape->getGeometryType();
    switch(type) 
    {  
		case PxGeometryType::eBOX:
			AddBox(shape,actor);
		break;
		case PxGeometryType::eSPHERE:
			AddSphere(shape,actor);
		break;
		case PxGeometryType::eCAPSULE:
			AddCapsule(shape,actor);
		break;
		case PxGeometryType::ePLANE:
			AddPlane(shape,actor);
		break;
    } 
}
void DIYPhisicsHandle::SetUpPhysXTutorial(){
	physx::PxAllocatorCallback *myCallback = new myAllocator();
	PhysicsFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,*myCallback, DefaultErrorCallback);
	Physics = PxCreatePhysics(PX_PHYSICS_VERSION, *PhysicsFoundation, physx::PxTolerancesScale());
	PhysicsCooker = PxCreateCooking(PX_PHYSICS_VERSION, *PhysicsFoundation, physx::PxCookingParams(physx::PxTolerancesScale()));
	PxInitExtensions(*Physics);
	//create physics material
	PhysicsMaterial = Physics->createMaterial(0.5f,0.5f,0.6f);
	physx::PxSceneDesc sceneDesc(Physics->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0,-30.0f,0);
	sceneDesc.filterShader = DefaultFilterShader;
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	PhysicsScene = Physics->createScene(sceneDesc);

	if(PhysicsScene) {
		printf("start physx scene2\n");
	}
}
void DIYPhisicsHandle::SetUpVisualDebugger() {
	// check if PvdConnection manager is available on this platform
	if (NULL == Physics->getPvdConnectionManager())
		return;
	// setup connection parameters
	const char* pvd_host_ip = "127.0.0.1"; // IP of the PC which is running PVD
	int port = 5425; // TCP port to connect to, where PVD is listening
	unsigned int timeout = 100; // timeout in milliseconds to wait for PVD to respond,
	// consoles and remote PCs need a higher timeout.
	physx::PxVisualDebuggerConnectionFlags connectionFlags = physx::PxVisualDebuggerConnectionFlag::Debug | physx::PxVisualDebuggerConnectionFlag::Profile | physx::PxVisualDebuggerConnectionFlag::Memory;
	// and now try to connect
	physx::PxVisualDebuggerExt::createConnection(Physics->getPvdConnectionManager(),pvd_host_ip, port, timeout, connectionFlags);
	// pvd_host_ip, port, timeout, connectionFlags));
}
void DIYPhisicsHandle::Shoot(GLFWwindow *_Window, glm::mat4 _Camera)
{
	if(Timer <= 0.0f)
	{
		Timer = 1/20.0f;
		float power = 100;
		PhysicsObject *MyProjectile;
		MyProjectile = new PhysicsObject();
		if (glfwGetMouseButton(_Window, GLFW_MOUSE_BUTTON_5))
		{
			//add a box
			physx::PxSphereGeometry bullet(1);
			physx::PxTransform transform(physx::PxVec3(_Camera[3].x, _Camera[3].y, _Camera[3].z));
			physx::PxRigidDynamic* Projectile = PxCreateDynamic(*Physics, transform, bullet,*PhysicsMaterial, 100); 
			//																			add force
			glm::vec3 Force(_Camera[2] * -power);
			Projectile->setLinearVelocity(physx::PxVec3(Force.x, Force.y, Force.z));//								, physx::PxForceMode::eVELOCITY_CHANGE);
			//add it to the physX scene
			PhysicsScene->addActor(*Projectile);
			//add it to our copy of the scene
			MyProjectile->RigidDynamic = Projectile;
			Actors.push_back(MyProjectile);
			
		}
		if (glfwGetMouseButton(_Window, GLFW_MOUSE_BUTTON_4))
		{
			//add a box
			physx::PxBoxGeometry bullet(1, 1, 1);
			physx::PxTransform transform(physx::PxVec3(_Camera[3].x, _Camera[3].y, _Camera[3].z));
			physx::PxRigidDynamic* Projectile = PxCreateDynamic(*Physics, transform, bullet,*PhysicsMaterial, 100); 
			//																			add force
			glm::vec3 Force(_Camera[2] * -power);
			Projectile->setLinearVelocity(physx::PxVec3(Force.x, Force.y, Force.z));//								, physx::PxForceMode::eVELOCITY_CHANGE);
			//add it to the physX scene
			PhysicsScene->addActor(*Projectile);
			//add it to our copy of the scene
			MyProjectile->RigidDynamic = Projectile;
			Actors.push_back(MyProjectile);
		}
	}
}


