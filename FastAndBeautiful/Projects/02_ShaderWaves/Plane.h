#ifndef __Plane_H_
#define __Plane_H_

#include "FBXFile.h"

struct V_4v_4c {
	glm::vec4		position;	//	Vertex Position
	glm::vec4		colour;		//	Vertex Normal			
};

class Plane{
public:
	Plane(void);
	~Plane(void);
	void GenerateGrid( unsigned int rows, unsigned int cols );
	//void PassInformation(V_4v_4c* verts,int i_VertCount);
	void Draw();
protected:
	unsigned int m_rows,m_columns;
	unsigned int m_VAO,m_VBO,m_IBO;
	V_4v_4c* aoVertices;
};

#endif