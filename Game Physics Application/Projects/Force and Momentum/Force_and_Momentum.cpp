#include "Force_and_Momentum.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <PxPhysicsAPI.h>
#include <vector>



#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720


Force_and_Momentum::Force_and_Momentum(){}
Force_and_Momentum::~Force_and_Momentum(){}

bool Force_and_Momentum::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//																			YeahYeah
	DIYPhysicsScene = new DIYPhisicsHandle();
	DIYPhysicsScene->Load();
	for(int i=0;i<4;i++)
	{
		for(int a=0;a<1;a++)
		{
			for(int b=0;b<4;b++)
			{
				Sphere *ball = new Sphere(glm::vec3 (i, a+5, b));
				DIYPhysicsScene->AddActor(ball);
			}
		}
	}

	Spring1 = new Sphere(true);
	Spring1->Position = glm::vec3(0, 20, 0);
	DIYPhysicsScene->AddActor(Spring1);
	Spring2 = new Sphere();
	Spring2->Position = glm::vec3(0, 2, 0);
	DIYPhysicsScene->AddActor(Spring2);
	Spring3 = new Sphere();
	Spring3->Position = glm::vec3(0, 10, 0);
	DIYPhysicsScene->AddActor(Spring3);

	SP1 = new SpringJoint(Spring1, Spring2);
	SP2 = new SpringJoint(Spring2, Spring3);

	Plane *plane = new Plane();
	DIYPhysicsScene->AddActor(plane);
	//																			YeahYeah
	


	return true;
}

void Force_and_Momentum::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );
	for ( int i = 0 ; i < 21 ; ++i ){
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}
	//																			YeahYeah
	
	//																			if (glfwGetKey(m_window,GLFW_KEY_SPACE) == GLFW_PRESS) {
		SP1->Update(0.1, 10);
		SP2->Update(0.1, 10);
		DIYPhysicsScene->Update(m_window, m_cameraMatrix, a_deltaTime);
	

	//																			YeahYeah
	
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		quit();
	}
}

void Force_and_Momentum::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	//																			YeahYeah
	DIYPhysicsScene->Draw();
	//																			YeahYeah

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Force_and_Momentum::onDestroy(){

	//																			YeahYeah
	DIYPhysicsScene->Unload();
	//																			YeahYeah
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new Force_and_Momentum();
	if (app->create("AIE - Force and Momentum",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}

