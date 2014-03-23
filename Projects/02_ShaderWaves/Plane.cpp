#include "Plane.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Plane::Plane(void) {}
Plane::~Plane(void) {}

void Plane::GenerateGrid( unsigned int rows, unsigned int cols ) {
	m_rows = rows;
	m_columns = cols;

	aoVertices = new V_4v_4c[ m_rows * m_columns ];
	for ( unsigned int r = 0 ; r < m_rows ; ++r ){
		for ( unsigned int c = 0 ; c < m_columns ; ++c ){
		aoVertices[ r * m_columns + c ].position = glm::vec4((float)c*0.5f, 0, (float)r*0.5f, 1);
		//Create some arbitrary colour based off something 
		//that might not be related to tiling a texture
		aoVertices[ r * m_columns + c ].colour = glm::vec4( 0.0f ,0.0f ,0.0f , 0.0f );
		}
	}

	// defining index count based off quad count (2 triangles per quad)
	unsigned int* auiIndices = new unsigned int[ (m_rows - 1) * (m_columns - 1) * 6 ];
	unsigned int index = 0;unsigned intindex = 0;
	for (unsigned int r=0;r<(m_rows -1);++r){
		for ( unsigned int c = 0 ; c < (m_columns - 1) ; ++c ){
		// triangle 1
		auiIndices[ index++ ] = (r + 1) * m_columns + (c + 1);
		auiIndices[ index++ ] = r * m_columns + c;
		auiIndices[ index++ ] = (r + 1) * m_columns + c;
		// triangle 2
		auiIndices[ index++ ] = r * m_columns + (c + 1);
		auiIndices[ index++ ] = r * m_columns + c;
		auiIndices[ index++ ] = (r + 1) * m_columns + (c + 1);

		}
	}

	//Add the following line to generate a VertexArrayObject
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray( m_VAO );

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (m_rows - 1) * (m_columns - 1) * 6 * sizeof(unsigned int), auiIndices, GL_STATIC_DRAW);

	// create and bind buffers to a vertex array object
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, (m_rows * m_columns) * sizeof(V_4v_4c), aoVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(V_4v_4c), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(V_4v_4c), (void*)16);

	glBindVertexArray(0);
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	delete[] aoVertices;
}

void Plane::Draw(){
	glBindVertexArray(m_VAO);
	unsigned int indexCount = (m_rows - 1) * (m_columns - 1) * 6;
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}