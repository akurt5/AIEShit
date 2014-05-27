#pragma once
#include <PxPhysicsAPI.h>
#include <vector>
#include <glm/ext.hpp>

using namespace physx;

class DIYPhysicsScene
{
public:
	DIYPhysicsScene(){}
	~DIYPhysicsScene(){}
	glm::vec3 Gravity;
	float TimeStep;
	std::vector <physx::PxRigidDynamic*> Actors;
	void AddActor (physx::PxRigidDynamic*);	
	void RemoveActor (physx::PxRigidDynamic*);
	void Update ();
	void Debug ();
	void UpdateGizmos ();
};