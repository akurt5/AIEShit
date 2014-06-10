#pragma once
#include <glm\ext.hpp>
#include <PxPhysicsAPI.h>
#include <vector>
#include "Gizmos.h"
#include <GL/glew.h>
#include <GLFW\glfw3.h>


#define Gravity glm::vec3 ( 0, -9.8, 0)
#define VEC3NULL glm::vec3(NULL, NULL, NULL)


class PhysicsObject
{
public:
	/*enum Collider
	{
		PLANE,
		SPHERE,
		CAPSULE,
		BOX,
	};*/

	PhysicsObject(float _Mass, float _Density, float _Radius, glm::vec3 _Force, glm::vec3 _Velocity, glm::vec3 _Position, glm::vec3 _Rotation, glm::vec3 _Dimensions, glm::vec4 _Colour);
	~PhysicsObject(void);
	
	void Load		(float _Mass, float _Density, float _Radius, glm::vec3 _Force, glm::vec3 _Velocity, glm::vec3 _Position, glm::vec3 _Rotation, glm::vec3 _Dimensions, glm::vec4 _Colour);//parameters are generic sizes 
	void Update		();
	void Draw		();
	void Unload		();

	//void AddForce	(glm::vec3 _Force, glm::vec3 _Damping);

	float Mass, Density, Radius;
	glm::vec3 Velocity, Position, Rotation, Dimensions, Force;
	glm::vec4 Colour;

	physx::PxRigidDynamic *RigidDynamic;

};

class DIYPhisicsHandle
{
public:
	DIYPhisicsHandle(void);
	~DIYPhisicsHandle(void);

	void AddBox		(PhysicsObject *_Actor);
	void AddSphere	(PhysicsObject *_Actor);
	void AddCapsule	(PhysicsObject *_Actor);
	void AddBox		(physx::PxShape* shape, PhysicsObject *_Actor);
	void AddSphere	(physx::PxShape* shape, PhysicsObject *_Actor);
	void AddPlane	(physx::PxShape* shape, PhysicsObject *_Actor);
	void AddCapsule	(physx::PxShape* shape, PhysicsObject *_Actor);										
	void AddWidget	(physx::PxShape* shape, PhysicsObject* _Actor);

	void SetUpPhysXTutorial();
	void SetUpVisualDebugger();

	void Load	();
	void Update (GLFWwindow *_Window, glm::mat4 _Camera);
	void Draw	();
	void Unload	();

	void Shoot	(GLFWwindow *_Window, glm::mat4 _Camera);

	int Timer;

	std::vector<PhysicsObject*> Actors;

	physx::PxScene *PhysicsScene;
	physx::PxFoundation* PhysicsFoundation;
	physx::PxPhysics* Physics;
	physx::PxDefaultErrorCallback DefaultErrorCallback;
	physx::PxDefaultAllocator DefaultAllocatorCallback;
	physx::PxSimulationFilterShader DefaultFilterShader;
	physx::PxMaterial* PhysicsMaterial;
	physx::PxCooking* PhysicsCooker;
};

