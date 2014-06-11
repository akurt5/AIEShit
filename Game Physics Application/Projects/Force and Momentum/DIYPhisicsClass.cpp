#include "DIYPhisicsClass.h"




PhysicsObject::PhysicsObject(Collider _Type, float _Mass, float _Density, float _Radius, glm::vec3 _Force, glm::vec3 _Velocity, glm::vec3 _Position, glm::vec3 _Dimensions, glm::vec4 _Colour, bool _Static)
{
	Static		= _Static;
	Type		= _Type;
	Mass		= _Mass;
	Density		= _Density;
	Radius		= _Radius;
	Force		= _Force;
	Velocity	= _Velocity;
	Position	= _Position;
	Dimensions	= _Dimensions;
	Colour		= _Colour;
	if((Radius = NULL)&&(_Type != PLANE))
	{
		Radius = Dimensions.x;
		if(Dimensions.y > Radius){Radius = Dimensions.y;}
		if(Dimensions.z > Radius){Radius = Dimensions.z;}
	}
	else if(_Type == PLANE)
	{
		Up = glm::normalize(_Normal); 
		Right = glm::cross(Up, glm::vec3(0,0,1));
		Forward = glm::cross(Up, -Right);
	}
}
PhysicsObject::~PhysicsObject(void){}
void PhysicsObject::Update()
{
	if(!Static)
	{
		Velocity += Force;
		Velocity += Resistance;
		Position += Velocity;
		Velocity = glm::vec3(0);
	}
}
void PhysicsObject::Draw()
{
	switch(Type)
	{
	case(PLANE):
		glm::vec3 P1	= Position + ( Forward - Right) * 10000.0f;
		glm::vec3 P2	= Position + ( Forward + Right) * 10000.0f;
		glm::vec3 P3	= Position + (-Forward - Right) * 10000.0f;
		glm::vec3 P4	= Position + (-Forward + Right) * 10000.0f;

		Gizmos::addTri(P1, P2, P3, Colour);
		Gizmos::addTri(P3, P2, P4, Colour);
	break;
	case(SPHERE):
		Gizmos::addSphere(Position, 5, 5, Radius, Colour);
	break;
	case(CAPSULE):
		Gizmos::addSphere(glm::vec3(Position.x, Position.y + Radius, Position.z), 5, 5, Radius, Colour);
		Gizmos::addSphere(glm::vec3(Position.x, Position.y - Radius, Position.z), 5, 5, Radius, Colour);

	break;
	case(BOX):
		Gizmos::addAABBFilled(Position, Dimensions, Colour);
	break;
	};
}
void PhysicsObject::Unload()
{


}
DIYPhisicsHandle::DIYPhisicsHandle(void){}
DIYPhisicsHandle::~DIYPhisicsHandle(void){}

void DIYPhisicsHandle::Load()
{	
}
void DIYPhisicsHandle::Update(GLFWwindow *_Window, glm::mat4 _Camera)
{

	for(auto Actor : Actors)
	{

		Actor->Update();
	}
	Shoot(_Window, _Camera);
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
void DIYPhisicsHandle::AddBox(PhysicsObject *_Actor)
{
	Actors.push_back(_Actor);
}
void DIYPhisicsHandle::AddSphere(PhysicsObject *_Actor)
{
	Actors.push_back(_Actor);
}
void DIYPhisicsHandle::AddCapsule(PhysicsObject *_Actor)
{
	Actors.push_back(_Actor);
}
void DIYPhisicsHandle::Shoot(GLFWwindow *_Window, glm::mat4 _Camera)
{
	if(Timer <= 0.0f)
	{
		Timer = 1/20.0f;
		float power = 100;
		PhysicsObject *MyProjectile;
		MyProjectile = new PhysicsObject();
		if (glfwGetMouseButton(_Window, GLFW_MOUSE_BUTTON_5))
		{
			Actors.push_back(MyProjectile);
			
		}
		if (glfwGetMouseButton(_Window, GLFW_MOUSE_BUTTON_4))
		{
			Actors.push_back(MyProjectile);
		}
	}
}
bool DIYPhisicsHandle::IsCollide(PhysicsObject * _ActorA, PhysicsObject * _ActorB)
{
	float dist = glm::length(_ActorA->Position - _ActorB->Position);
	float leng = _ActorA->Radius - _ActorB->Radius;
	if(abs(dist) >  leng)
	{
		_ActorA->Velocity *= 0.95;
		_ActorA->Velocity *= -1; 
		_ActorB->Velocity *= 0.95;
		_ActorB->Velocity *= -1; 
		return true;
	}
	return false;
}
bool DIYPhisicsHandle::CollideScene()
{
	for (auto ActorA : Actors)
	{
		for (auto ActorB : Actors)
		{
			if(ActorA->Position != ActorB->Position)
			{
				 return IsCollide(ActorA, ActorB);
			}
		}
	}
}

