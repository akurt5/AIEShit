#ifndef Planet_H
#define Planet_H

#include "Gizmos.h"
#include "Utilities.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include "FBX_Class.h"

class Planet
{
	public:
		
		Planet(glm::vec3 v3Pos,float fSpeed, float Radius, glm::vec4 v4Colour, glm::vec3 Position = glm::vec3(0, 0, 0));

		glm::vec3 GetPosition();
		void Update (float delta, glm::vec3 _Pos);
		void Draw();
		FBXHandle Mesh;
		glm::vec3 Position, Rotation;
		float Speed, Size;
		glm::vec4 Colour;
};

#endif