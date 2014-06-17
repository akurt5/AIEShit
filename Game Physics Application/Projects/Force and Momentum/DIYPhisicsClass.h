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

	PhysicsObject(bool _Static, float _Density, float _Mass, glm::vec3 _Velocity, glm::vec3 _Position/*, glm::vec3 _Force*/, glm::vec4 _Colour)
	{
	Static		= _Static;
	Mass		= _Mass;
	Velocity	= _Velocity;
	Position	= _Position;
	//Force		= _Force;
	Colour		= _Colour;
	}
	PhysicsObject()
	{
		Static		= false;
		Mass		= 100.0f;
		Velocity	= glm::vec3(0);
		Position	= glm::vec3(0);
		//Force		= glm::vec3(0);
		Colour		= glm::vec4(1.0f, 0, 0, 1.0f);
	}
	~PhysicsObject(){}
	
	virtual void Update		();
	virtual void Draw		() = NULL;

	void AddForce	(glm::vec3 _Force, glm::vec3 _Damping);

	float Mass, Density, CollideRadius;
	glm::vec3 Velocity, Position, Force;
	glm::vec4 Colour;
	Collider Type;
	bool Static;


};
class Plane : public PhysicsObject
{
public:
	Plane(glm::vec3 _Normal, glm::vec3 _Offset, bool _Static, float _Density, float _Mass, glm::vec3 _Velocity, glm::vec3 _Position/*, glm::vec3 _Force*/, glm::vec4 _Colour) : PhysicsObject(_Static, _Density, _Mass, _Velocity, _Position/*, _Force*/, _Colour)
	{
		Position = _Offset;
		Up = glm::normalize(_Normal); 
		Right = glm::cross(Up, glm::vec3(0,0,1));
		Forward = glm::cross(Up, -Right);

		Type = Collider::PLANE; 
	}
	Plane() : PhysicsObject()
	{
		Up			= glm::normalize(glm::vec3(0));
		Static		= false;
		Mass		= 100.0f;
		Velocity	= glm::vec3(0);
		Position	= glm::vec3(0, -1.0f, 0);
		Force		= glm::vec3(0);
		Colour		= glm::vec4(1.0f, 0, 0, 1.0f);
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
public:
	Box(glm::vec3 _Dimensions, bool _Static, float _Density, float _Mass, glm::vec3 _Velocity, glm::vec3 _Position/*, glm::vec3 _Force*/, glm::vec4 _Colour) : PhysicsObject(_Static, _Density, _Mass, _Velocity, _Position/*, _Force*/, _Colour)
	{
		Dimensions = _Dimensions;
		CollideRadius = (Dimensions.x > Dimensions.y)? Dimensions.x : (Dimensions.y > Dimensions.z)?Dimensions.y : Dimensions.z;
	}
	Box() : PhysicsObject()
	{
		CollideRadius = 0.5f;
		Dimensions	= glm::vec3(0.5f);
		Static		= false;
		Mass		= 100.0f;
		Velocity	= glm::vec3(0);
		Position	= glm::vec3(0);
		Force		= glm::vec3(0);
		Colour		= glm::vec4(1.0f, 0, 0, 1.0f);
	}
	~Box(){}
	void Create();
	void Update(){PhysicsObject::Update();}
	void Draw(){Gizmos::addAABBFilled(Position, Dimensions, Colour);}

	glm::vec3  Dimensions;
};
class Sphere : public PhysicsObject
{
public:
	Sphere(float _Radius, bool _Static, float _Density, float _Mass, glm::vec3 _Velocity, glm::vec3 _Position/*, glm::vec3 _Force*/, glm::vec4 _Colour) : PhysicsObject(_Static, _Density, _Mass, _Velocity, _Position/*, _Force*/, _Colour)
	{
		Radius = _Radius;
		CollideRadius = Radius;

	}
	Sphere() : PhysicsObject()
	{
		Radius		= 1.0f;
		Static		= false;
		Mass		= 100.0f;
		Velocity	= glm::vec3(0);
		Position	= glm::vec3(0);
		Force		= glm::vec3(0);
		CollideRadius = Radius;
		Colour		= glm::vec4(1.0f, 0, 0, 1.0f);
	}
	~Sphere(){}
	void Create();
	void Draw(){Gizmos::addSphere(Position, 5, 5, Radius, Colour);}

	float Radius;
};
class Capsule : public PhysicsObject
{
public:
	Capsule(float _Radius, float _HalfLength, bool _Static, float _Density, float _Mass, glm::vec3 _Velocity, glm::vec3 _Position/*, glm::vec3 _Force*/, glm::vec4 _Colour) : PhysicsObject(_Static, _Density, _Mass, _Velocity, _Position/*, _Force*/, _Colour)
	{
		Radius = _Radius;
		HalfLength = _HalfLength;
		CollideRadius = Radius + HalfLength;
	}
	Capsule() : PhysicsObject()
	{
		Radius		= 1.0f;
		HalfLength	= 0.5f;
		Static		= false;
		Mass		= 100.0f;
		Velocity	= glm::vec3(0);
		Position	= glm::vec3(0);
		Force		= glm::vec3(0);
		Colour		= glm::vec4(1.0f, 0, 0, 1.0f);
		CollideRadius = Radius + HalfLength;

	}
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

	//bool IsCollide(Sphere * _ActorA, Sphere * _ActorB);
	//bool IsCollide(Box * _ActorA, Sphere * _ActorB);
	//bool IsCollide(Sphere * _ActorA, Box * _ActorB);
	//bool IsCollide(Box * _ActorA, Box * _ActorB);
	//																			bool IsCollide(PhysicsObject *_ActorA, PhysicsObject *_ActorB);
	//																			bool CollideScene();

	int Timer;

	std::vector<PhysicsObject*> Actors;

};

