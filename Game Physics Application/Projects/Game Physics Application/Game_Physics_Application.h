#ifndef __Game_Physics_Application_H_
#define __Game_Physics_Application_H_

#include "Application.h"
#include <glm/glm.hpp>
#include <PxPhysicsAPI.h>

using namespace physx;

// Derived application class that wraps up all globals neatly
class Game_Physics_Application : public Application
{
public:

	Game_Physics_Application();
	virtual ~Game_Physics_Application();

protected:

	virtual bool onCreate(int a_argc, char* a_argv[]);
	virtual void onUpdate(float a_deltaTime);
	virtual void onDraw();
	virtual void onDestroy();

	void setUpPhysXTutorial();
	void cleanUpPhsyx();
	void upDatePhysx();
	void tutorial_1();
	void setUpVisualDebugger();
	void AddBox(glm::vec3 _Pos);

	//																			myAllocator *Allocator;

	glm::mat4	m_cameraMatrix;
	glm::mat4	m_projectionMatrix;
};

#endif // __Game_Physics_Application_H_

class myAllocator: public PxAllocatorCallback
{
public:
        virtual ~myAllocator() {}
        virtual void* allocate(size_t size, const char* typeName, const char* filename, int line)
		{
			void* pointer = _aligned_malloc(size,16);
			return pointer;
		}
        virtual void deallocate(void* ptr)
		{
			 _aligned_free(ptr);
		}
};

