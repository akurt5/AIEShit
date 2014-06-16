#pragma once
#include <glm\ext.hpp>
//#include <PxPhysicsAPI.h>
#include <vector>
#include "Gizmos.h"
#include <GL/glew.h>
#include <GLFW\glfw3.h>


#define Gravity glm::vec3 ( 0, -9.8, 0)
#define Resistance glm::vec3 (-0.1)
#define VEC3NULL glm::vec3(NULL, NULL, NULL)

class PhysicsObject
{
public:
	enum Collider
	{
		ENULL = 0,
		PLANE = 1,
		SPHERE = PLANE << 1,
		CAPSULE = SPHERE << 1,
		BOX = CAPSULE <<1,
	};

	PhysicsObject(bool _Static, float _Density, float _Mass, glm::vec3 _Velocity, glm::vec3 _Position, glm::vec3 _Force, glm::vec4 _Colour)
	{
	Static		= _Static;
	Mass		= _Mass;
	Velocity	= _Velocity;
	Position	= _Position;
	Force		= _Force;
	Colour		= _Colour;
	}
	~PhysicsObject(){}
	
	virtual void Update		();
	virtual void Draw		() = NULL;

	void AddForce	(glm::vec3 _Force, glm::vec3 _Damping);

	float Mass, Density;
	glm::vec3 Velocity, Position, Force;
	glm::vec4 Colour;
	Collider Type;
	bool Static;


};
class Plane : public PhysicsObject
{
	Plane(glm::vec3 _Normal, glm::vec3 _Offset) : PhysicsObject(_Static, _Density, _Mass, _Velocity, _Position, _Force, _Colour)
	{
		Position = _Offset;
		Up = glm::normalize(_Normal); 
		Right = glm::cross(Up, glm::vec3(0,0,1));
		Forward = glm::cross(Up, -Right);

		Type = Collider::PLANE; 
	}
	~Plane(){}
	void Create();
	virtual void Draw(glm::vec3 Position) 
	{
		glm::vec3 P1	= Position + ( Forward - Right) * 10000.0f;
		glm::vec3 P2	= Position + ( Forward + Right) * 10000.0f;
		glm::vec3 P3	= Position + (-Forward - Right) * 10000.0f;
		glm::vec3 P4	= Position + (-Forward + Right) * 10000.0f;

		Gizmos::addTri(P1, P2, P3, Colour);
		Gizmos::addTri(P3, P2, P4, Colour);
	}

	glm::vec3  Dimensions, Up, Right, Forward;
};
class Box : public PhysicsObject
{
	Box(){}
	~Box(){}
	void Create();
	void Update(){PhysicsObject::Update();}
	void Draw(){Gizmos::addAABBFilled(Position, Dimensions, Colour);}

	glm::vec3  Dimensions;
};
class Sphere : public PhysicsObject
{
	Sphere(float _Radius,bool _Static, float _Density, float _Mass, glm::vec3 _Velocity, glm::vec3 _Position, glm::vec3 _Force, glm::vec4 _Colour) : PhysicsObject(_Static, _Density, _Mass, _Velocity, _Position, _Force, _Colour)
	{
		Radius = _Radius;
	}
	~Sphere(){}
	void Create();
	void Draw(){Gizmos::addSphere(Position, 5, 5, Radius, Colour);}

	float Radius;
};
class Capsule : public PhysicsObject
{
	Capsule(){}
	~Capsule(){}
	void Create();
	void Draw()
	{
		Gizmos::addSphere(glm::vec3(Position.x, Position.y + Radius, Position.z), 5, 5, Radius, Colour);
		Gizmos::addLine(glm::vec3(Position.x, Position.y + Radius, Position.z), glm::vec3(Position.x, Position.y - Radius, Position.z), Colour);
		Gizmos::addSphere(glm::vec3(Position.x, Position.y - Radius, Position.z), 5, 5, Radius, Colour);
	}

	float Radius, HalfLength;
};


class DIYPhisicsHandle
{
public:
	DIYPhisicsHandle(void);
	~DIYPhisicsHandle(void);

	void AddBox		(PhysicsObject *_Actor);
	void AddSphere	(PhysicsObject *_Actor);
	void AddCapsule	(PhysicsObject *_Actor);
	void AddPlane	(PhysicsObject *_Actor);

	void Load	();
	void Update (GLFWwindow *_Window, glm::mat4 _Camera);
	void Draw	();
	void Unload	();

	void Shoot	(GLFWwindow *_Window, glm::mat4 _Camera);

	bool IsCollide(PhysicsObject * _ActorA, PhysicsObject * _ActorB);
	bool CollideScene();

	int Timer;

	std::vector<PhysicsObject*> Actors;

};

