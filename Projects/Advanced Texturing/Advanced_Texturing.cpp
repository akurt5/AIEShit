#include "Advanced_Texturing.h"
#include "FBX_Class.h"


#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720



FBXHandle Mesh;

Advanced_Texturing::Advanced_Texturing()
{

}

Advanced_Texturing::~Advanced_Texturing()
{

}

bool Advanced_Texturing::onCreate(int a_argc, char* a_argv[]) 
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

	Mesh.Load(/*"./Models/soulspear/soulspear.fbx"*/);


	return true;
}

void Advanced_Texturing::onUpdate(float a_deltaTime) 
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

	// quit our application when escape is pressed
	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
		quit();

	if (glfwGetKey(m_window,GLFW_KEY_LEFT) == GLFW_PRESS) Mesh.m_decayValue -= a_deltaTime;
	if (glfwGetKey(m_window,GLFW_KEY_RIGHT) == GLFW_PRESS) Mesh.m_decayValue += a_deltaTime;

	if( Mesh.m_decayValue < 0.0f) Mesh.m_decayValue = 0.0f;
	if( Mesh.m_decayValue > 1.0f) Mesh.m_decayValue = 1.0f;

	Mesh.Update();
}

void Advanced_Texturing::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	Gizmos::draw(viewMatrix, m_projectionMatrix);

	Mesh.Draw(viewMatrix, m_projectionMatrix);
}

void Advanced_Texturing::onDestroy()
{
	// clean up anything we created
	Gizmos::destroy();
	Mesh.Unload();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Advanced_Texturing();
	
	if (app->create("AIE - Tesselation",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}