#include "Shader_Class.h"


ShaderHandle::ShaderHandle(){}
ShaderHandle::~ShaderHandle(){}

/*void ShaderHandle::Load(int _Count, char *_Shader, ...)
{
	char *End, *a, *Type;
	va_list Shaders;
	va_start(Shaders, _Shader);
	End = va_arg(Shaders, char*);
	for(int i=0;i!=*_Shader;i++){
		if((a = va_arg(Shaders, char*)) > End)
                End = a;
		
		std::string Path, Temp;
		Temp = a;
		for(i=4;i!=0;i++){
			char x = Temp.back;
			Path.append(x);
			Temp.pop_back;
		}
	}
	va_end(Shaders);
}*/

/*void ShaderHandle::Load(int _COUNT, const char *_VERT, const char *_FRAG, const char *_GEOM, ...)
//void ShaderHandle::Load(std::vector <const char*> _ShaderList)
{

	for(int i=0;i<_ShaderList.size();++i)
	{
		std::stringstream sPATH;
		sPATH << "../../Build/shaders/" << _ShaderList[i];
		std::string sVERTpath = sPATH.str();
		(const char*)_ShaderList[i] = sVERTpath.c_str();
	}


	std::stringstream sVERT;
	sVERT << "../../Build/shaders/" << _VERT;
	std::string sVERTpath = sVERT.str();
	_VERT = sVERTpath.c_str();

	//if(_FRAG == nullptr)
	//	_FRAG = _VERT;
	//else{
		std::stringstream sFRAG;
		sFRAG << "../../Build/shaders/" << _FRAG;
		std::string sFRAGpath = sFRAG.str();
		_FRAG = sFRAGpath.c_str();
	//}

	if(_GEOM != nullptr){
		std::stringstream sGEOM;
		sGEOM << "../../Build/shaders/" << _GEOM;
		std::string sGEOMpath = sGEOM.str();
		_GEOM = sGEOMpath.c_str();
		gshader = Utility::loadShader(_GEOM, GL_GEOMETRY_SHADER);
	}
	else{
	gshader = 0;
	}
		

	GLuint vshader = Utility::loadShader(_VERT, GL_VERTEX_SHADER);
	GLuint fshader = Utility::loadShader(_FRAG, GL_FRAGMENT_SHADER);
	const char* aszInputs[] =  { "Position", "Color", "Normal", "Tangent", "Binormal", "Indices", "Weights", "Texcoord1", "Texcoord2" };
	const char* aszOutputs[] = { "outColour" };
	
	m_shader = Utility::createProgram( vshader, 0, 0, gshader, fshader, 9, aszInputs, 1, aszOutputs);

	// free our shader once we built our program
	glDeleteShader( vshader );
	glDeleteShader( fshader );
	glDeleteShader( gshader );
}*/
void ShaderHandle::Load(const char *_VERT, const char *_FRAG, const char *_GEOM)
{
	std::stringstream sVERT;
	VERT = _VERT;
	sVERT << "../../Build/shaders/" << _VERT << ".vert";
	std::string sVERTpath = sVERT.str();
	_VERT = sVERTpath.c_str();

	std::stringstream sFRAG;
	FRAG = _FRAG;
	sFRAG << "../../Build/shaders/" << _FRAG << ".frag";
	std::string sFRAGpath = sFRAG.str();
	_FRAG = sFRAGpath.c_str();

	if(_GEOM != nullptr){
		std::stringstream sGEOM;
		GEOM = _GEOM;
		sGEOM << "../../Build/shaders/" << _GEOM << ".geom";
		std::string sGEOMpath = sGEOM.str();
		_GEOM = sGEOMpath.c_str();
		gshader = Utility::loadShader(_GEOM, GL_GEOMETRY_SHADER);
	}
	else{
		GEOM = 0;
		gshader = 0;
	}
		

	GLuint vshader = Utility::loadShader(_VERT, GL_VERTEX_SHADER);
	GLuint fshader = Utility::loadShader(_FRAG, GL_FRAGMENT_SHADER);
	const char* aszInputs[] =  { "Position", "Color", "Normal", "Tangent", "Binormal", "Indices", "Weights", "Texcoord1", "Texcoord2" };
	const char* aszOutputs[] = { "outColour" };
	
	m_shader = Utility::createProgram( vshader, 0, 0, gshader, fshader, 9, aszInputs, 1, aszOutputs);

	// free our shader once we built our program
	glDeleteShader( vshader );
	glDeleteShader( fshader );
	glDeleteShader( gshader );
}
void ShaderHandle::Reload(GLFWwindow *_Window, int _Key)
{
	if(glfwGetKey(_Window, _Key))
	{
		Load(VERT, FRAG, GEOM);
		printf("Reloading Shaders... \n");
	}
}
void ShaderHandle::Unload()
{
	glDeleteShader(m_shader);

}