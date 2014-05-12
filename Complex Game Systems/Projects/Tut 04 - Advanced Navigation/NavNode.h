#pragma once

#include <glm/ext.hpp>


struct NavNode
	{
		glm::vec3 Position, Vertices[3];
		NavNode *edgeTarget[3], *Parent;
		float Score;
	};

struct Compare
	{
		inline bool operator() (NavNode *_Node1, NavNode *_Node2)
		{
			NavNode *Temp1 = _Node1, *Temp2 = _Node2;
			if(Temp1->Score < 0)
			{
				//Temp1->Score *= -1;
			}
			return (Temp1->Score<Temp2->Score);
		}
	};