#include "DIYPhisicsClass.h"


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
		MyProjectile = new Sphere(1, false, 100, 100, glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec4(1, 0, 0, 1));
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

