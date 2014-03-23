#include "Assignment.h"



#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720

Assignment::Assignment()
{

}

Assignment::~Assignment()
{

}

bool Assignment::onCreate(int a_argc, char* a_argv[]) 
{
	
	// initialise the Gizmos helper class6
	Gizmos::create();

	// create a world-space matrix for a camera
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	
	// create a perspective projection matrix with a 90 degree field-of-view and widescreen aspect ratio
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);

	// set the clear colour and enable depth testing and backface culling
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);


	LightShader = new ShaderHandle();
//	LightShader->Load(2, "lit.vert", "lit.frag");
	LightShader->Load("lit", "lit");
	AnimShader = new ShaderHandle();
	AnimShader->Load("anim", "anim");

	Rabbit = new FBXHandle();
	Rabbit->Load("stanford/bunny", FBXFile::UNITS_CENTIMETER);

	Pyro = new FBXHandle();
	Pyro->Load("characters/demolition/demolition", FBXFile::UNITS_METER);

	//MeshList.push_back(Rabbit);
	//MeshList.push_back(Pyro);

	//Lighting = new LightHandle();
	//Lighting->Load(MeshList);

	Emitter1 = new Planet(glm::vec3(3, 3, 3), 0.001, 1, glm::vec4(0.2, 0.2, 0.2, 0.2));
	Emitter2 = new Planet(glm::vec3(0, 3, 0), 0.01, 0.5, glm::vec4(0.2, 0.8, 0.2, 0.2));
	
	Particles1 = new GPUParticleHandle();
	Particles1->Load(300, 4.0f, 20.0f, 0.1f, 0.01f, 0.1f, 0.05f, glm::vec4(0.2,0.2,0.2,0.8), glm::vec4(0.8,0.8,0.8,0.8));
	Particles1->Move(Emitter1->Position);

	Particles2 = new GPUParticleHandle();
	Particles2->Load(3000, 0.4f, 1.0f, 0.1f, 0.01f, 0.1f, 0.05f, glm::vec4(0,0,0,1), glm::vec4(0.5,0,0,0.2));
	Particles2->Move(Emitter2->Position);

	return true;
}

void Assignment::onUpdate(float a_deltaTime) 
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

	Rabbit->Update(LightShader);
	Pyro->Update(AnimShader);
	Pyro->Move(glm::vec3(1500, 0, 0));
	
	Emitter1->Update(a_deltaTime, glm::vec3(-2, -2, 0));
	Emitter2->Update(a_deltaTime, glm::vec3(0, 2, 5));

	Particles1->Move(Emitter1->Position);
	Particles2->Move(Emitter2->Position);
	
	AnimShader->Reload(m_window, GLFW_KEY_GRAVE_ACCENT);
}

void Assignment::onDraw() 
{
	// clear the backbuffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// get the view matrix from the world-space camera matrix
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	// draw the gizmos from this frame
	
	Particles1->Draw(m_cameraMatrix, m_projectionMatrix);
	Particles2->Draw(m_cameraMatrix, m_projectionMatrix);


	Emitter1->Draw();
	Emitter2->Draw();
	
	Gizmos::draw(viewMatrix, m_projectionMatrix);
	
	Rabbit->Draw(viewMatrix, m_projectionMatrix, LightShader);
	Pyro->Draw(viewMatrix, m_projectionMatrix, AnimShader);
	//Lighting->Draw(viewMatrix, m_projectionMatrix, MeshList, LightShader);

}

void Assignment::onDestroy()
{
	// clean up anything we created
	
	Gizmos::destroy();
	
	LightShader->Unload();
	Rabbit->Unload();
	Pyro->Unload();
	
	//Lighting->Unload(MeshList);
	Particles1->~GPUParticleHandle();
	Particles2->~GPUParticleHandle();
}

// main that controls the creation/destruction of an application
int main(int argc, char* argv[])
{
	// explicitly control the creation of our application
	Application* app = new Assignment();
	
	if (app->create("AIE - Assignment",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true)
		app->run();
		
	// explicitly control the destruction of our application
	delete app;

	return 0;
}