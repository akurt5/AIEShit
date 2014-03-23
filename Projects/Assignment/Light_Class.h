//#ifndef LightClass_H_
//#define LightClass_H_
#pragma once

#include "FBX_Class.h"



class LightHandle
{
public:

	LightHandle();
	virtual~LightHandle();

	void Load(std::vector <FBXHandle*> _MeshList);
	void Draw(glm::mat4 a_view, glm::mat4 a_projection, std::vector <FBXHandle*> _MeshList, ShaderHandle *_Shaders);
	void Unload(std::vector <FBXHandle*> _MeshList);

	ShaderHandle Shaders;

protected:

};

//#endif