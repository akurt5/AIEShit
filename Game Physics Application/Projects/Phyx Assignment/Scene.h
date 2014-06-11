#pragma once

#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>
#include <unordered_map>

typedef physx::PxRigidActor* RIGIDACTOR;
typedef physx::PxArticulation* RAGDOLL;

extern bool bHitTest;

//derived class to overide the call backs we are interested in...
class MycollisionCallBack : public physx::PxSimulationEventCallback {
public:
	virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) {
		for(physx::PxU32 i = 0; i < nbPairs; i++) {
			const physx::PxContactPair& cp = pairs[i];
			if(cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND){
				printf("Collision %s %s \n",pairHeader.actors[0]->getName(),pairHeader.actors[1]->getName());
			}
		}
	}
	virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 nbPairs){
		for(physx::PxU32 i = 0; i < nbPairs; i++) {
			physx::PxTriggerPair& cp = pairs[i];
			if(cp.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND){
				bHitTest = !bHitTest;
				printf("Trigger %i \n", bHitTest);
			}
		}
	};
	virtual void onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32){};
	virtual void onWake(physx::PxActor** , physx::PxU32 ){};
	virtual void onSleep(physx::PxActor** , physx::PxU32 ){};
};

class myAllocator: public physx::PxAllocatorCallback {
public:
	virtual ~myAllocator() {}
	virtual void* allocate(size_t size, const char* typeName, const char* filename, int line){
		void* pointer = _aligned_malloc(size,16);
		return pointer;
	}
	virtual void deallocate(void* ptr){
		_aligned_free(ptr);
	}
};

struct RagdollNode {
	physx::PxQuat globalRotation;  //rotation of this link in model space - we could have done this relative to the parent node but it's harder to visualize when setting up the data by hand
	physx::PxVec3 scaledGobalPos; //Position of the link centre in world space which is calculated when we process the node.  It's easiest if we store it here so we have it when we transform the child
	int parentNodeIdx;	//Index of the parent node
	float halfLength; //half length of the capsule for this node
	float radius; //radius of capsule for thisndoe
	float parentLinkPos; //relative position of link centre in parent to this node.  0 is the centre of hte node, -1 is left end of capsule and 1 is right end of capsule relative to x 
	float childLinkPos;  //relative position of link centre in child
	char* name;	//name of link
	physx::PxArticulationLink* linkPtr; //pointer to link if we are using articulation
	physx::PxRigidDynamic* actorPtr;  //Pointer the PhysX actor which is linked to this node if we are using seperate actors

	//constructor
	RagdollNode(physx::PxQuat _globalRotation,int _parentNodeIdx,float _halfLength,float _radius,float _parentLinkPos,float _childLinkPos,char* _name){
		globalRotation = _globalRotation;
		parentNodeIdx = _parentNodeIdx;
		halfLength = _halfLength;
		radius=_radius;
		parentLinkPos = _parentLinkPos;
		childLinkPos = _childLinkPos;
		name = _name;
	}
};

struct ClothData {
	glm::vec3 clothPosition;
	unsigned int m_shader;
	unsigned int m_texture;
	unsigned int m_clothIndexCount;
	unsigned int m_clothVertexCount;
	glm::vec3*	m_clothPositions;
	unsigned int m_clothVAO, m_clothVBO, m_clothTextureVBO, m_clothIBO;
	//physx::PxClothCollisionSphere *Spheres;
	physx::PxCloth* m_cloth;
};

//create some constants for axis of rotation to make definition of quaternions a bit neater
const physx::PxVec3 X_AXIS = physx::PxVec3(1,0,0);
const physx::PxVec3 Y_AXIS = physx::PxVec3(0,1,0);
const physx::PxVec3 Z_AXIS = physx::PxVec3(0,0,1);

class Scene
{
public:
	Scene(float _TicksPerSecond);
	~Scene();

	physx::PxScene* scene(){
		return g_PhysicsScene;
	}

	physx::PxPhysics* physics(){
		return g_Physics;
	}

	void Update();
	void Draw(glm::mat4 _ViewProjection);

	RIGIDACTOR AddPlane (bool _Static, float _Density, glm::vec3 _Position);
	RIGIDACTOR AddBox (bool _Static, float _Density, glm::vec3 _Position, glm::vec3 _Extents);
	RIGIDACTOR AddBall (bool _Static, float _Density, glm::vec3 _Position, float _Radius);
	RIGIDACTOR AddCapsule (bool _Static, float _Density, glm::vec3 _Position, float _Radius, float _HalfHeight);

	RAGDOLL AddRagdoll ( RagdollNode** _NodeArray, physx::PxTransform _WorldPos, float _ScaleFactor);
	void AddCloth(ClothData *_ClothIn,float _SpringSize, unsigned int _SpringRows, unsigned int _SpringCols);

	void CreateTrigger(physx::PxRigidActor* _ActorA);

	void ControlActor(float _DeltaTime, const glm::mat4& _Camera, physx::PxRigidActor* _Actor, float _Force);

private:
	float TicksPerSecond;
	physx::PxFoundation* g_PhysicsFoundation;
	physx::PxPhysics* g_Physics;
	physx::PxScene* g_PhysicsScene;
	physx::PxDefaultErrorCallback gDefaultErrorCallback;
	physx::PxDefaultAllocator gDefaultAllocatorCallback;
	physx::PxSimulationFilterShader gDefaultFilterShader;
	physx::PxMaterial* g_PhysicsMaterial;
	physx::PxCooking* g_PhysicsCooker;	
	// We could merge these down into "serializables" but better to keep seperate and have easier control
	std::vector<physx::PxRigidActor*>		g_PhysXActors;
	std::vector<physx::PxJoint*>			g_PhysXJoints;
	std::vector<physx::PxArticulation*>		g_PhysXActorsRagDolls;
	std::vector<ClothData*>					g_PhysXActorsCloths;
};

