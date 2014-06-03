#pragma once
#include <glm\ext.hpp>
#include <PxPhysicsAPI.h>
#include <vector>

#define Gravity glm::vec3 ( 0, -9.8, 0)

/*enum Collider
	{
		PLANE,
		SPHERE,
		BOX,
	};*/

class PhysicsObject
{
public:
	PhysicsObject(void);
	~PhysicsObject(void);
	void LoadPlane	(glm::vec3 _Facing);
	void LoadBox	(glm::vec3 _Position, glm::vec3 _Dimensions, float _Density);
	void LoadSphere	(glm::vec3 _Position, float Radius, float _Density);

	void Update		();
	void Draw		();

	float Mass, Density, Radius;
	glm::vec3 Velocity, Position, Rotation, Dimensions;
	glm::vec4 Colour;
};

class DIYPhisicsHandle
{
public:
	DIYPhisicsHandle(void);
	~DIYPhisicsHandle(void);
	void Load	();
	void Update ();
	void Draw	();

	std::vector<PxRigidDynamic*> Actors;

	physx::PxFoundation* g_PhysicsFoundation;
	physx::PxPhysics* g_Physics;
	physx::PxScene* g_PhysicsScene;
	physx::PxDefaultErrorCallback gDefaultErrorCallback;
	physx::PxDefaultAllocator gDefaultAllocatorCallback;
	physx::PxSimulationFilterShader gDefaultFilterShader;
	physx::PxMaterial* g_PhysicsMaterial;
	physx::PxCooking* g_PhysicsCooker;
};

