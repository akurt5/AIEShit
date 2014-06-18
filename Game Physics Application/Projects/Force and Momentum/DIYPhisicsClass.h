#pragma once
#include <glm\ext.hpp>
//#include <PxPhysicsAPI.h>
#include <vector>
#include "Gizmos.h"
#include <GL/glew.h>
#include <GLFW\glfw3.h>


#define Gravity glm::vec3 ( 0, -9.8, 0)
#define Resistance glm::vec3 (-0.0001)
#define VEC3NULL glm::vec3(NULL, NULL, NULL)

class PhysicsObject
{
public:
	enum Collider
	{
		ENULL = 0,
		PLANE = 1,
		SPHERE = PLANE << 1,
	};
	PhysicsObject(bool _Static, float _Density, float _Mass, glm::vec3 _Velocity, glm::vec3 _Position, glm::vec4 _Colour)
	{
	Static		= _Static;
	Mass		= _Mass;
	Velocity	= _Velocity;
	Position	= _Position;
	Colour		= _Colour;
	}
	PhysicsObject()
	{
		Static		= false;
 		Mass		= 1.0f;
		Velocity	= glm::vec3(0);
		Position	= glm::vec3(0);
		Colour		= glm::vec4(1.0f, 0, 0, 1.0f);
	}
	PhysicsObject(glm::vec3 _Position)
	{
		Static		= false;
		Mass		= 1.0f;
		Velocity	= glm::vec3(0);
		Position	= _Position;
		Colour		= glm::vec4(1.0f, 0, 0, 1.0f);
	}
	~PhysicsObject(){}
	
	virtual void Update		(float _Delta);
	virtual void Draw		() = NULL;

	//																			void AddForce	(glm::vec3 _Force, glm::vec3 _Damping);

	float Mass, Density;
	glm::vec3 Velocity, Position;
	glm::vec4 Colour;
	Collider Type;
	bool Static;
};
class Plane : public PhysicsObject
{
public:
	Plane(glm::vec3 _Normal, glm::vec3 _Offset, bool _Static, float _Density, float _Mass, glm::vec3 _Velocity, glm::vec3 _Position, glm::vec4 _Colour) : PhysicsObject(_Static, _Density, _Mass, _Velocity, _Position, _Colour)
	{
		Position = _Offset;
		Up = glm::normalize(_Normal); 
		Right = glm::cross(Up, glm::vec3(0,0,1));
		Forward = glm::cross(Up, -Right);
		Static = true;
		

		Type = Collider::PLANE; 
	}
	Plane() : PhysicsObject()
	{
		Up			= glm::normalize(glm::vec3(0, 1, 0));
		Right		= glm::cross(Up, glm::vec3(0,0,1));
		Forward		= glm::cross(Up, -Right);
		Static		= true;
		Colour		= glm::vec4(1, 1, 1, 1); 

		Type = Collider::PLANE; 
	}
	~Plane(){}
	void Create();
	virtual void Draw() 
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
class Sphere : public PhysicsObject
{
public:
	Sphere(float _Radius, bool _Static, float _Density, float _Mass, glm::vec3 _Velocity, glm::vec3 _Position, glm::vec4 _Colour) : PhysicsObject(_Static, _Density, _Mass, _Velocity, _Position, _Colour)
	{
		Radius = _Radius;
		Type = Collider::SPHERE;

	}
	Sphere() : PhysicsObject()
	{
		Radius		= 1.0f;
		Type = Collider::SPHERE;

	}
	Sphere(glm::vec3 _Position) : PhysicsObject(_Position)
	{
		Radius		= 1.0f;
		Type = Collider::SPHERE;

	}
	~Sphere(){}
	void Create();
	void Draw(){Gizmos::addSphere(Position, 5, 5, Radius, Colour);}

	float Radius;
};
class DIYPhisicsHandle
{
public:
	DIYPhisicsHandle(void);
	~DIYPhisicsHandle(void);

	void AddActor	(PhysicsObject *_Actor);

	void Load	();
	void Update (GLFWwindow *_Window, glm::mat4 _Camera, float _Delta);
	void Draw	();
	void Unload	();

	void IsCollide(PhysicsObject *_ActorA, PhysicsObject *_ActorB);
	void IsCollide(Plane *_ActorA, Sphere *_ActorB);
	void IsCollide(Sphere *_ActorA, Plane *_ActorB);
	void IsCollide(Sphere *_ActorA, Sphere *_ActorB);
	void CollideScene();

	int Timer;

	std::vector<PhysicsObject*> Actors;
};

