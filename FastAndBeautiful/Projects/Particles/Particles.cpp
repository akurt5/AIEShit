#include "Particles.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <SOIL.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

ParticleEmitter* m_emitter;

Particles::Particles()
{

}

Particles::~Particles()
{

}
	

bool Particles::onCreate(int a_argc, char* a_argv[]) 
{
	m_texture = SOIL_load_OGL_texture("../../Build/textures/planets/jupiter_diffuse.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, 0);

	// load shaders and link shader program
	unsigned int m_vertShader = Utility::loadShader("../../Build/shaders/Particle.vert", GL_VERTEX_SHADER);
	unsigned int m_fragShader = Utility::loadShader("../../Build/shaders/Particle.frag", GL_FRAGMENT_SHADER);

	// our vertex buffer has 3 properties per-vertex
	const char* inputs[] = { "Position", "Colour", "Uv" };
	m_shader = Utility::createProgram(m_vertShader,0,0,0,m_fragShader, 3, inputs);
	glUseProgram(m_shader);

	m_emitter = new ParticleEmitter();
	m_emitter->initalise(1000, 500,  0.1f, 0.9f, 1, 5, 0.1f, 1.0f, glm::vec4(1,1,0,1), glm::vec4(1,0,0,1));
	

	// initialise the Gizmos helper class
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	int location = glGetUniformLocation(m_shader,"projection");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(m_projectionMatrix));

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	return true;
}

void Particles::onUpdate(float a_deltaTime) 
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
		Gizmos::addLine( glm::vec3(-10 + i, 0, 10), glm::vec3(-10 + i, 0, -10),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );		
		Gizmos::addLine( glm::vec3(10, 0, -10 + i), glm::vec3(-10, 0, -10 + i),  i == 10 ? glm::vec4(1,1,1,1) : glm::vec4(0,0,0,1) );
	}

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) quit();
	m_emitter->update(a_deltaTime, m_cameraMatrix);

}

void Particles::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// get the view matrix from the world-space camera matrix
	viewMatrix = glm::inverse( m_cameraMatrix );
	// fetch locations of the view and projection matrices and bind them
	unsigned int location = glGetUniformLocation(m_shader,"view");
	glUniformMatrix4fv(location, 1, false, glm::value_ptr(viewMatrix));

	m_emitter->draw(m_shader);	

	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Particles::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Particles();
	
	if (app->create("AIE - Particles",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}