#ifndef GPUPartcleClass_H_
#define GPUPartcleClass_H_

#include "Shader_Class.h"
struct GPUParticle
{
	GPUParticle() : lifetime(1), lifespan(0) {}

	glm::vec3	position;
	glm::vec3	velocity;
	float		lifetime;
	float		lifespan;
};

class GPUParticleHandle
{
public:

	GPUParticleHandle();
	virtual ~GPUParticleHandle();

	void	Load(unsigned int a_maxParticles, float a_lifespanMin, float a_lifespanMax, float a_velocityMin, float a_velocityMax, float a_startSize, float a_endSize, const glm::vec4& a_startColour, const glm::vec4& a_endColour);
	void	Draw(const glm::mat4& a_cameraTransform, const glm::mat4& a_projection);
	void	Move(const glm::vec3 a_position)	{	m_position = a_position;	}
	
	

protected:

	void	createBuffers();
	void	createUpdateShader();
	void	createDrawShader();

	GPUParticle *m_particles;

	unsigned int	m_maxParticles;

	glm::vec3	m_position;

	float	m_lifespanMin;
	float	m_lifespanMax;

	float	m_velocityMin;
	float	m_velocityMax;

	float	m_startSize;
	float	m_endSize;

	glm::vec4	m_startColour;
	glm::vec4	m_endColour;

	unsigned int	m_activeBuffer;
	unsigned int	m_vao[2];
	unsigned int	m_vbo[2];

	unsigned int m_updateShader;
	unsigned int m_drawShader;
};
#endif