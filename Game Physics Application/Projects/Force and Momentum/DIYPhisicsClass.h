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
		PLANE,
		SPHERE,
		CAPSULE,
		BOX,
	};

	PhysicsObject(Collider _Type = BOX, 
		float _Mass = 100, 
		float _Density = 100, 
		glm::vec3 _Position = glm::vec3(0.0f, 2.0f, 0.0f), 
		glm::vec3 _Force = glm::vec3(0.0f), 
		glm::vec3 _Velocity = glm::vec3(0.0f), 
		glm::vec4 _Colour = glm::vec4(1, 0, 0, 1), 
		bool _Static = false);
	~PhysicsObject(void);
	
	void Load		(Collider _Type = BOX, float _Mass = 100, float _Density = 100, float _Radius = NULL, glm::vec3 _Position = glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3 _Dimensions = glm::vec3(1.0f), glm::vec3 _Force = glm::vec3(0.0f), glm::vec3 _Velocity = glm::vec3(0.0f), glm::vec4 _Colour = glm::vec4(1, 0, 0, 1), bool _Static = false);//parameters are generic sizes 
	void Update		();
	void Draw		();
	void Unload		();

	void AddForce	(glm::vec3 _Force, glm::vec3 _Damping);

	float Mass, Density;
	glm::vec3 Velocity, Position, Force;
	glm::vec4 Colour;
	Collider Type;
	bool Static;


};

class Plane : public PhysicsObject {
public:
	glm::vec3 Forward, Up, Right;

public:
	Plane(glm::vec3 _Normal, glm::vec3 _Offset, glm::vec4 _Colour) { 
		Position = _Offset;
		Colour = _Colour;
		Up = glm::normalize(_Normal); 
		Right = glm::cross(Up, glm::vec3(0,0,1));
		Forward = glm::cross(Up, -Right);

		Type = Collider::PLANE; 
	}
	virtual ~Plane() { }

	virtual void Draw(glm::vec3 Position) {
		glm::vec3 P1	= Position + ( Forward - Right) * 10000.0f;
		glm::vec3 P2	= Position + ( Forward + Right) * 10000.0f;
		glm::vec3 P3	= Position + (-Forward - Right) * 10000.0f;
		glm::vec3 P4	= Position + (-Forward + Right) * 10000.0f;

		Gizmos::addTri(P1, P2, P3, Colour);
		Gizmos::addTri(P3, P2, P4, Colour);
	}
};
class Box : public PhysicsObject
{
	Box(){}
	~Box(){}
	glm::vec3  Dimensions;
};
class Sphere : public PhysicsObject
{
	Sphere(){}
	~Sphere(){}
	float Radius;
};
class Capsule : public PhysicsObject
{
	Capsule(){}
	~Capsule(){}
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

