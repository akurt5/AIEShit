#pragma once
#include <glm\ext.hpp>
#include <PxPhysicsAPI.h>
#include <vector>

#define Gravity glm::vec3 ( 0, -9.8, 0)



class PhysicsObject : public PxRigidDynamic
{
public:
	/*enum Collider
	{
		PLANE,
		SPHERE,
		CAPSULE,
		BOX,
	};*/

	PhysicsObject(void);
	~PhysicsObject(void);
	
	void Load		(float _Mass, float _Density, float Radius, glm::vec3 _Velocity, glm::vec3 _Position, glm::vec3 _Rotation, glm::vec3 _Dimensions, glm::vec4 _Colour);//parameters are generic sizes 
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

	//void AddPlane	(glm::vec3 _Facing, PhysicsObject* _Actor);
	//void AddBox	(glm::vec3 _Position, glm::vec3 _Dimensions, float _Density, PhysicsObject* _Actor);
	//void AddSphere	(glm::vec3 _Position, float Radius, float _Density, PhysicsObject* _Actor);
	void AddBox		(physx::PxShape* shape, PhysicsObject* _Actor);
	void AddSphere	(physx::PxShape* shape, PhysicsObject* _Actor);
	void AddPlane	(physx::PxShape* shape, PhysicsObject* _Actor);
	void AddCapsule	(physx::PxShape* shape, PhysicsObject* _Actor);
	void AddWidget	(physx::PxShape* shape, PhysicsObject* _Actor);

	void Load	();
	void Update ();
	void Draw	();

	std::vector<PhysicsObject*> Actors;

};

