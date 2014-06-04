#include "DIYPhisicsClass.h"
#include "Gizmos.h"


using namespace physx;

//																			override for a Plane
PhysicsObject::PhysicsObject(glm::vec3 _Facing)
{
	LoadPlane(_Facing);
}
//																			Box
PhysicsObject::PhysicsObject(glm::vec3 _Position, glm::vec3 _Dimensions, float _Density)
{

}//																			Sphere
PhysicsObject::PhysicsObject(glm::vec3 _Position, float Radius, float _Density)
{

}
void PhysicsObject::LoadPlane(glm::vec3 _Facing)
{
	//add a plane
	physx::PxTransform pose = physx::PxTransform( physx::PxVec3(0,0,0),physx::PxQuat(physx::PxHalfPi * 0.95f, physx::PxVec3(0.0f, 0.0f, 1.0f)));
	physx::PxRigidStatic* plane = PxCreateStatic(*g_Physics, pose, physx::PxPlaneGeometry(), *g_PhysicsMaterial);
}

DIYPhisicsHandle::DIYPhisicsHandle(void){}
DIYPhisicsHandle::~DIYPhisicsHandle(void){}

void DIYPhisicsHandle::Load()
{

}
void DIYPhisicsHandle::Update()
{
	g_PhysicsScene->simulate( 1/120.f );
	while (g_PhysicsScene->fetchResults() == false) {
	// don’t need to do anything here yet but we still need to do the fetch
	}
	// Add widgets to represent all the phsyX actors which are in the scene
	for(auto Actor : Actors)
	{
		physx::PxU32 nShapes = Actor->getNbShapes();
		physx::PxShape** shapes = new physx::PxShape*[nShapes];
		Actor->getShapes(shapes, nShapes);
		// Render all the shapes in the physx actor (for early tutorials there is just one)
		while (nShapes--)
		{
			AddWidget(shapes[nShapes],Actor); 
		}
		delete [] shapes;

		Actor->Update();
	}
}
void DIYPhisicsHandle::Draw ()
{
	for(auto Actor : Actors)
	{
		Actor->Draw();
	}
}
void DIYPhisicsHandle::AddBox	(PxShape* shape, PhysicsObject* _Actor)
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
void DIYPhisicsHandle::AddSphere	(PxShape* shape, PhysicsObject* _Actor)
{
	//get the geometry for this PhysX collision volume
	physx::PxBoxGeometry geometry;
	bool status = shape->getBoxGeometry(geometry);
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
void DIYPhisicsHandle::AddPlane	(PxShape* shape, PhysicsObject* _Actor)
{

}
void DIYPhisicsHandle::AddCapsule	(PxShape* shape, PhysicsObject* _Actor)
{
	//get the geometry for this PhysX collision volume
	physx::PxBoxGeometry geometry;
	bool status = shape->getBoxGeometry(geometry);
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


