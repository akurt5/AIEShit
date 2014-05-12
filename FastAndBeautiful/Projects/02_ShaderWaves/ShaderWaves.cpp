#include "ShaderWaves.h"
#include "Gizmos.h"
#include "Utilities.h"
#include "Shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "Plane.h"

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

float fTime = 1.0f;
float fHeight = 1.0f;

Plane WaterPlane;
Shader* WaterShader;

ShaderWaves::ShaderWaves(){ }
ShaderWaves::~ShaderWaves(){ }

bool ShaderWaves::onCreate(int a_argc, char* a_argv[]) {

	Gizmos::create();

	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(40,10,40),glm::vec3(30,0,30), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	glClearColor(0.25f,0.25f,0.25f,1);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	WaterPlane.GenerateGrid(100,100);

	WaterShader = new Shader("Shaders/Water.vert","Shaders/Water.frag");
	WaterShader->SetAttribs(2,0,"Position",1,"Colour");

	WaterShader->SetUniform("height","1f",1,fHeight);

	return true;
}

void ShaderWaves::onUpdate(float a_deltaTime) {

	fTime += a_deltaTime;

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS){ quit(); }
	if (glfwGetKey(m_window,GLFW_KEY_F1) == GLFW_PRESS){ glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); }else{ glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); }

	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );

	Gizmos::clear();
	Gizmos::addTransform(glm::mat4(1),1);

//UPDATE
	WaterShader->SetUniform("time","1f",1,fTime);
//UPDATE
}

void ShaderWaves::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_viewMatrix = glm::inverse( m_cameraMatrix );
//DRAW
	WaterShader->SetUniform("ProjectionView","m4fv",1,false,glm::value_ptr(m_projectionMatrix * m_viewMatrix));

	WaterShader->Use();
	WaterPlane.Draw();

	glUseProgram(0);
	Gizmos::addAABBFilled(glm::vec3(25,-fHeight*2,25),glm::vec3(25,0.001f,25),glm::vec4(1.0f,0,0,1));
	Gizmos::addAABBFilled(glm::vec3(0,0,25),glm::vec3(1,fHeight,25),glm::vec4(0,0,0,1));
	Gizmos::addAABBFilled(glm::vec3(50,0,25),glm::vec3(1,fHeight,25),glm::vec4(0,0,0,1));
	Gizmos::addAABBFilled(glm::vec3(25,0,0),glm::vec3(25,fHeight,1),glm::vec4(0,0,0,1));
	Gizmos::addAABBFilled(glm::vec3(25,0,50),glm::vec3(25,fHeight,1),glm::vec4(0,0,0,1));
//DRAW
	Gizmos::draw(m_viewMatrix, m_projectionMatrix);
}

void ShaderWaves::onDestroy(){ Gizmos::destroy(); }

// main that controls the creation/destruction of an application
int main(int argc, char* argv[]){
	Application* app = new ShaderWaves();
	if (app->create("AIE - 02_ShaderWaves",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
	delete app;
	return 0;
}

