
#include "Planet.h"

Planet::Planet(glm::vec3 v3Pos,float fSpeed, float Radius, glm::vec4 v4Colour)
{
	Rotation = v3Pos;
	Size = Radius;
	Colour = v4Colour;
	Speed = fSpeed;
}
glm::vec3 Planet::GetPosition()
{
	return Position;
}
void Planet::Update (float delta, Planet* p_Planet)
{
	if (p_Planet != nullptr)
	{
		Rotation = glm::rotate(Rotation, Speed, glm::vec3(0, 1, 0));
		Position = Rotation + p_Planet->GetPosition();
	}
	else
	{
		Rotation = glm::rotate(Rotation, Speed, glm::vec3(0, 1, 0));
		Position = Rotation;
	}
}
void Planet::Draw()
{
	Gizmos::addSphere(Position, 5, 6, Size, Colour);
}