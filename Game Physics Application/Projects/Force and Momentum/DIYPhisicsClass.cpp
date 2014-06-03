#include "DIYPhisicsClass.h"


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
	for(auto Actor : Actors)
	{
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

