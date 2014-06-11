#include "Phyx_Assignment.h"
#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#define DEFAULT_SCREENWIDTH 1280
#define DEFAULT_SCREENHEIGHT 720
// Human Ragdoll PhysX

RagdollNode* HumanRagdoll[21] = {
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f,Z_AXIS)		,-1, 0.5f, 3.0f	, 1.0f , 1.0f, "lowerSpine"),
	new RagdollNode(physx::PxQuat(physx::PxPi,Z_AXIS)			, 0, 0.5f, 1.0f	,-1.0f , 1.0f, "leftPelvis"),
	new RagdollNode(physx::PxQuat(0,Z_AXIS)						, 0, 0.5f, 1.0f	,-1.0f , 1.0f, "rightPelvis"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f+0.2f,Z_AXIS)	, 1, 2.5f, 2.0f	,-1.0f , 1.0f, "leftUpperLeg"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f-0.2f,Z_AXIS)	, 2, 2.5f, 2.0f ,-1.0f , 1.0f, "right upperLeg"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f+0.2f,Z_AXIS)	, 3, 2.5f, 1.75f,-1.0f , 1.0f, "leftLowerLeg"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f-0.2f,Z_AXIS)	, 4, 2.5f, 1.75f,-1.0f , 1.0f, "rightLowerLeg"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f,Y_AXIS)		, 5, 0.7f, 1.5f	,-1.0f , 1.0f, "leftFoot"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f,Y_AXIS)		, 6, 0.7f, 1.5f	,-1.0f , 1.0f, "rightFoot"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f,Z_AXIS)		, 0, 0.5f, 3.0f , 1.0f ,-1.0f, "upperSpine"),
	new RagdollNode(physx::PxQuat(physx::PxPi,Z_AXIS)			, 9, 0.5f, 1.5f , 1.0f , 1.0f, "leftClavicle"),
	new RagdollNode(physx::PxQuat(0,Z_AXIS)						, 9, 0.5f, 1.5f , 1.0f , 1.0f, "rightClavicle"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f,Z_AXIS)		, 9, 0.5f, 1.0f , 1.0f ,-1.0f, "neck"),
	new RagdollNode(physx::PxQuat(physx::PxPi/2.0f,Z_AXIS)		,12, 0.5f, 3.0f , 1.0f ,-1.0f, "head"),
	new RagdollNode(physx::PxQuat(physx::PxPi-.3,Z_AXIS)		,10, 1.5f, 1.5f ,-1.0f , 1.0f, "leftUpperArm"),
	new RagdollNode(physx::PxQuat(0.3,Z_AXIS)					,11, 1.5f, 1.5f ,-1.0f , 1.0f, "rightUpperArm"),
	new RagdollNode(physx::PxQuat(physx::PxPi-.3,Z_AXIS)		,14, 1.5f, 1.0f ,-1.0f , 1.0f, "leftLowerArm"),
	new RagdollNode(physx::PxQuat(0.3,Z_AXIS)					,15, 1.5f, 1.0f ,-1.0f , 1.0f, "rightLowerArm"),
	new RagdollNode(physx::PxQuat(physx::PxPi-.3,Z_AXIS)		,16, 0.5f, 1.5f ,-1.0f , 1.0f, "leftHand"),
	new RagdollNode(physx::PxQuat(0.3,Z_AXIS)					,17, 0.5f, 1.5f ,-1.0f , 1.0f, "rightHand"),
	NULL
};

bool Phyx_Assignment::onCreate(int a_argc, char* a_argv[]) {
	Gizmos::create();
	m_cameraMatrix = glm::inverse( glm::lookAt(glm::vec3(10,10,10),glm::vec3(0,0,0), glm::vec3(0,1,0)) );
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, DEFAULT_SCREENWIDTH/(float)DEFAULT_SCREENHEIGHT, 0.1f, 1000.0f);
	glClearColor(0.25f,0.25f,0.25f,1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	MyScene = new Scene(60);
	MyScene->AddPlane(true,100, glm::vec3 (0));
	MyScene->AddBox(false, 100, glm::vec3(5), glm::vec3(0.5));

	MyScene->AddBox(true, 0, glm::vec3(5, 0.5, 0), glm::vec3(0.5, 1, 5));
	MyScene->AddBox(true, 0, glm::vec3(-5, 0.5, 0), glm::vec3(0.5, 1, 5));
	MyScene->AddBox(true, 0, glm::vec3(0, 0.5, 5), glm::vec3(5, 1, 0.5));
	MyScene->AddBox(true, 0, glm::vec3(0, 0.5, -5), glm::vec3(5, 1, 0.5));

	RIGIDACTOR CollisionVolume = MyScene->AddBox(true, 0, glm::vec3(0, 0.5, 0), glm::vec3(1, 1, 1));
	MyScene->CreateTrigger(CollisionVolume);

	MyClothData = new ClothData;
	MyClothData->clothPosition = glm::vec3(50,50,0);
	MyScene->AddCloth(MyClothData,1,40,40);
	MyScene->AddRagdoll(HumanRagdoll,physx::PxTransform(physx::PxVec3(0.0f,5.0f,0.0f)),0.1f);

	Player = MyScene->AddCapsule(false, 100, glm::vec3(3), 1, 0.25);

	{
		PxParticleFluid* pf;
		// create particle system in PhysX SDK
		// set immutable properties.
		PxU32 maxParticles = 1000;
		bool perParticleRestOffset = false;
		pf = MyScene->physics()->createParticleFluid(maxParticles, perParticleRestOffset);
		pf->setRestParticleDistance(1.0f);
		pf->setDynamicFriction(0.1f);
		pf->setStaticFriction(0.1f);
		pf->setContactOffset(2.0f);
		pf->setDamping(0.1f);
		pf->setParticleMass(1.0f);
		pf->setRestitution(0.0f);
		pf->setParticleReadDataFlag(PxParticleReadDataFlag::eDENSITY_BUFFER,true);
		pf->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_TWOWAY,true);
		if (pf) {
			MyScene->scene()->addActor(*pf);
			particleFluidEmitter = new ParticleFluidEmitter(maxParticles,PxVec3(0,2,0),pf,0.1f);
		}
	}

	{
		float maxParticles = 1000;
		bool perParticleRestOffset = false;
		PxParticleSystem* ps;
		// create particle system in PhysX SDK
		ps = MyScene->physics()->createParticleSystem(maxParticles, perParticleRestOffset);
		//various flags we can set/clear to get different effects
		//ps->setParticleBaseFlag(PxParticleBaseFlag::eCOLLISION_TWOWAY,true);
		//ps->setDamping(.01);
		//ps->setRestitution(2);
		//ps->setActorFlag(PxActorFlag::eDISABLE_GRAVITY,true);
		// add particle system to scene, if creation was successful
		if (ps) {
			MyScene->scene()->addActor(*ps);
			//create a particle emiter to make particles for our scene
			particleEmitter = new ParticleEmitter(maxParticles,PxVec3(0,15,0),ps,.1f);
		}
	}

	return true;
}

void Phyx_Assignment::onUpdate(float a_deltaTime) {
	Utility::freeMovement( m_cameraMatrix, a_deltaTime, 10 );
	Gizmos::clear();
	Gizmos::addTransform( glm::mat4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1) );

	MyScene->Update();
	MyScene->ControlActor(a_deltaTime, m_cameraMatrix, Player, 10);
	static bool bButton1 = false;
	if (glfwGetMouseButton(m_window,GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
		if (bButton1 == false){
			bButton1 = true;
			float Power = 50;
			RIGIDACTOR Ball = MyScene->AddBall(false,100,m_cameraMatrix[3].xyz,1.0f);
			physx::PxVec3 velocity(m_cameraMatrix[2].x * -Power,m_cameraMatrix[2].y * -Power,m_cameraMatrix[2].z * -Power);
			((physx::PxRigidDynamic*)Ball)->setLinearVelocity(velocity,true);
		}
	}else{
		bButton1 = false;
	}

	if (bHitTest && particleEmitter != nullptr){
		particleEmitter->upDate(a_deltaTime); // Not really an update, more of a removal check
	}

	if (bHitTest && particleFluidEmitter != nullptr){ 
		particleFluidEmitter->upDate(a_deltaTime); 
	}

	if (glfwGetKey(m_window,GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		quit();
	}
}

void Phyx_Assignment::onDraw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 viewMatrix = glm::inverse( m_cameraMatrix );
	
	MyScene->Draw(m_projectionMatrix * viewMatrix);

	if (particleFluidEmitter != nullptr){ 
		particleFluidEmitter->renderParticles();
	}
	if (particleEmitter != nullptr){
		particleEmitter->renderParticles();
	}

	Gizmos::draw(viewMatrix, m_projectionMatrix);
}

void Phyx_Assignment::onDestroy(){
	
	delete MyScene;
	Gizmos::destroy();
}

int main(int argc, char* argv[]) {
	Application* app = new Phyx_Assignment();
	if (app->create("AIE - Phyx Assignment",DEFAULT_SCREENWIDTH,DEFAULT_SCREENHEIGHT,argc,argv) == true){
		app->run();
	}
	delete app;
	return 0;
}

