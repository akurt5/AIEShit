#include "Bahaviour_tree.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "Agent.h"

class WithinRange : public Behaviour
{
public:

	WithinRange(float _Range ) : Range2 (_Range*_Range) {}
	virtual ~WithinRange(){}
	virtual bool Execute(Agent *_Agent)
	{
		float dist2 = glm::distance2(_Agent->GetPos(), _Agent->GetTarget());
		if (dist2 < Range2)
			return true;
		return false;
	}

	float Range2;
};
class RandomiseTarget : public Behaviour
{
public:
	RandomiseTarget(float _Radius) : Radius(_Radius) {}
	virtual ~RandomiseTarget(){}
	virtual bool Execute(Agent *_Agent)
	{
		glm::vec3 Target(0);

		Target.xz = glm::circularRand(Radius);
		_Agent->SetTarget(Target);
		return true;
	}
	float Radius;
};
class SeekTarget : public Behaviour
{
public:
	SeekTarget(float _Speed) : Speed(_Speed){}
	virtual ~SeekTarget(){}

	virtual bool  Execute(Agent *_Agent)
	{
		glm::vec3 Pos = _Agent->GetPos();
		glm::vec3 Dir = glm::normalize(_Agent->GetTarget() - Pos);

		_Agent->SetPos(Pos + Dir * Speed * Utility::getDeltaTime());
		return true;
	}
	float Speed;
};

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Bahaviour_tree::Bahaviour_tree()
{

}

bool Bahaviour_tree::onCreate(int a_argc, char* a_argv[]) 
{
	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	Guy = new Agent();

	Behaviour* Seek = new SeekTarget(3);
	Behaviour* Rand = new RandomiseTarget(10);
	Behaviour* Within = new WithinRange(0.5f);

	Sequence* Seq = new Sequence();
	Seq->addchild(Within);
	Seq->addchild(Rand);

	Selector* Root = new Selector();
	Root->addchild(Seq);
	Root->addchild(Seek);

	Agenda = Root;
	Guy->SetBehaviour(Agenda);

	return true;
}

void Bahaviour_tree::onUpdate(float a_deltaTime) 
{
	// update our camera matrix using the keyboard/mouse
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	// clear all gizmos from last frame
	Gizmos::clear();
	
	// add an identity matrix gizmo
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	// add a 20x20 grid on the XZ-plane
	for ( int i = 0 ; i < 21 ; ++i )
	{
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i), 
						 i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}


	Guy->update(a_deltaTime);

	Gizmos::addAABBFilled(Guy->GetPos(), glm::vec3(0.5f), glm::vec4(1, 0, 0, 1));
	Gizmos::addAABBFilled(Guy->GetTarget(), glm::vec3(0.5f), glm::vec4(0, 1, 0, 1));

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();
}

void Bahaviour_tree::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Bahaviour_tree::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Bahaviour_tree();
	
	if (app->create("AIE - Bahaviour tree",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}