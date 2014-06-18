#include "DIYPhisicsClass.h"


void PhysicsObject::Update(float _Delta)
{
	if(!Static)
	{
		if(Velocity != VEC3NULL)
		{
			Velocity += Resistance;
		}
		Velocity += Gravity;
		Position += Velocity * _Delta;
		//																			Velocity = glm::vec3(0);
	}
}
DIYPhisicsHandle::DIYPhisicsHandle(void){}
DIYPhisicsHandle::~DIYPhisicsHandle(void){}

void DIYPhisicsHandle::Load()
{	
}
void DIYPhisicsHandle::Update(GLFWwindow *_Window, glm::mat4 _Camera, float _Delta)
{

	for(auto Actor : Actors)
	{

		Actor->Update(_Delta);
	}
	CollideScene();
}
void DIYPhisicsHandle::Draw ()
{
	for(auto Actor : Actors)
	{
		Actor->Draw();
	}
}
void DIYPhisicsHandle::Unload()
{
	Actors.clear();
}
void DIYPhisicsHandle::AddActor(PhysicsObject *_Actor)
{
	Actors.push_back(_Actor);
}
void DIYPhisicsHandle::IsCollide(PhysicsObject *_ActorA, PhysicsObject *_ActorB)
{
	if((_ActorA->Type == PhysicsObject::SPHERE)&&(_ActorB->Type == PhysicsObject::SPHERE))
	{
		IsCollide((Sphere*)_ActorA, (Sphere*)_ActorB);
		return;
	}
	if((_ActorA->Type == PhysicsObject::PLANE)&&(_ActorB->Type == PhysicsObject::SPHERE))
	{
		IsCollide((Plane*)_ActorA, (Sphere*)_ActorB);
		return;
	}
	if((_ActorA->Type == PhysicsObject::SPHERE)&&(_ActorB->Type == PhysicsObject::PLANE))
	{
		IsCollide((Sphere*)_ActorA, (Plane*)_ActorB);
		return;
	}
}
void DIYPhisicsHandle::IsCollide(Plane *_ActorA, Sphere *_ActorB)
{
	//	Detection
	glm::vec3 APos = _ActorA->Position;//																			 + A->_Offset;
	glm::vec3 BPos = _ActorB->Position;//																			 + B->_Offset;

	float distance = glm::abs(glm::dot(BPos, _ActorA->Up) - glm::length(APos));

	if (distance > _ActorB->Radius)
		return;

	glm::vec3 Normal = _ActorA->Up;
	float Depth = _ActorB->Radius - distance;


	// Responce
	//	Store Plane Normal
	glm::vec3 planeNormal = _ActorA->Up;

	//	Distance to Plane
	distance = glm::dot(planeNormal, _ActorB->Position) - glm::distance(_ActorB->Position, _ActorA->Position);

	if (distance < 0)
		planeNormal *= -1.0f;

	glm::vec3 forceVector = _ActorB->Mass * planeNormal * (glm::dot(planeNormal,_ActorB->Velocity));
	glm::vec3 seperationVector = Normal * Depth;

	//	Static Checks
	if (_ActorA->Static == false && _ActorB->Static == true) {
		_ActorA->Position -= seperationVector;
		_ActorA->Velocity += (forceVector * 2.0f) / _ActorA->Mass;
	}
	else if (_ActorA->Static == true && _ActorB->Static == false) {
		_ActorB->Position += seperationVector;
		_ActorB->Velocity += (forceVector * 2.0f) / _ActorA->Mass;
	} 
	else {
		_ActorA->Position -= seperationVector * 0.505f;
		_ActorB->Position += seperationVector * 0.505f;

		_ActorA->Velocity +=( forceVector) / _ActorA->Mass;
		_ActorB->Velocity +=(-forceVector) / _ActorA->Mass;
	}

}
void DIYPhisicsHandle::IsCollide(Sphere *_ActorB, Plane *_ActorA)
{
	IsCollide(_ActorA,_ActorB);
}
void DIYPhisicsHandle::IsCollide(Sphere *_ActorA, Sphere *_ActorB)
{
	float radius = _ActorA->Radius + _ActorB->Radius;
	float distance = glm::distance(_ActorA->Position, _ActorB->Position);
	if (distance < radius)
	{
		float Depth = radius - distance;
		glm::vec3 Normal = glm::normalize(_ActorA->Position - _ActorB->Position);
		//	Relative Velocity    
		glm::vec3 relativeVelocity = _ActorA->Velocity - _ActorB->Velocity;

		//	Velocity Along Normal
		glm::vec3 collisionVector = Normal * glm::dot(relativeVelocity, Normal);

		glm::vec3 forceVector = collisionVector * 2.0f/((1.0f / _ActorA->Mass) + (1.0f / _ActorB->Mass));
		glm::vec3 seperationVector = Normal * Depth * 0.505f;
	
		//	Static Checks
		if (_ActorA->Static == false && _ActorB->Static == true) 
		{
			_ActorA->Position -= seperationVector;
			_ActorA->Velocity -= (forceVector * 2.0f) / _ActorA->Mass;
		}
		else if (_ActorA->Static == true && _ActorB->Static == false) 
		{
			_ActorB->Position += seperationVector;
			_ActorB->Velocity += (forceVector * 2.0f) / _ActorB->Mass;
		} 
		else 
		{
			_ActorA->Position -= seperationVector * 0.505f;
			_ActorB->Position += seperationVector * 0.505f;

			_ActorA->Velocity -= (forceVector) / _ActorA->Mass;
			_ActorB->Velocity += (forceVector ) / _ActorB->Mass;
		}
	}
}

void DIYPhisicsHandle::CollideScene()
{
	for (auto ActorA : Actors)
	{
		for (auto ActorB : Actors)
		{
			if(ActorA->Position != ActorB->Position)
			{
				 IsCollide(ActorA, ActorB);
			}
		}
	}
}
