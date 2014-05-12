#pragma once

#include "Behaviour.h"
#include "Utilities.h"
#include <glm\glm.hpp>
#include <vector>
#include <algorithm>
#include "NavNode.h"

class Flag;
class Agent
{
public:
 
	//																			add '...' to allow multiple enemies, or add a list. List is probably better.
	//Agent(){Timer = 0;Attack = false;}
	Agent(std::vector <NavNode*> _Nodes){Timer = 0;Attack = false;Nodes = _Nodes;}
	virtual ~Agent (){}

	const glm::vec3& GetPos () const {return Position;}

	const glm::vec3 GetTarget () const{return Target;}
	void CalcEnemy(std::vector <Agent*> _Team){Enemies = _Team;}
	void SetPos (const glm::vec3& _Pos) { Position = _Pos;}
	void SetTarget (glm::vec3 _Target) { Target = _Target;}
	void Move(float t)
	{
		glm::vec3 Dest = GetTarget();
		glm::vec3 Pos = Position;
		if (Path.size() >= 2)
		{
			Dest = Path[1]->Position;
			float t1 = 1-t;			
			Dest = t1 * ((t1 * Position) + (t * Path[0]->Position)) + t * ((t1 * Path[0]->Position) + (t * Path[1]->Position));
		}
	
			//	for now i will just normalise there
			//	Interpolate linearly to find the third vector // use a number greater than 1 as time // then spline
		if(Dest.x != 0 && Dest.z != 0)
		{
			glm::normalize(Dest);
		}
		Velocity += Dest;
	}
	void PathFind(glm::vec3 _EndPos)
	{
		StartNode = GiveScore(Nodes, GetPos());
		CurrentNode = StartNode;//GiveScore(Nodes, _StartPos);
		EndNode = GiveScore(Nodes, _EndPos);

		CurrentNode->Parent = nullptr;

		Open.emplace_back(CurrentNode);
		Open.emplace_back(CurrentNode);
		if((CurrentNode->Position.x != EndNode->Position.x)&&(CurrentNode->Position.z != EndNode->Position.z))
		{
			Path.emplace_back(CurrentNode);
			Path.emplace_back(EndNode);
		}
		do{
			Closed.emplace_back(Open.front());
			Open.erase(Open.begin());
			std::sort( Open.begin(), Open.end(), Compare());
			CurrentNode = Open.front();

			for(int i=0;i<3;i++)
			{
				if(CurrentNode->edgeTarget[i] != nullptr)
				{
					int check =0;
					for(int a=0;a<Open.size();a++)
					{
						if(CurrentNode->edgeTarget[i]->Position == Open[a]->Position)
						{
							check ++;
						}
					}
					for(int a=0;a<Closed.size();a++)
					{
						if(CurrentNode->edgeTarget[i]->Position == Closed[a]->Position)
						{
							check ++;
						}
					}
					if(check == 0)
					{
						Open.emplace_back(CurrentNode->edgeTarget[i]);
						CurrentNode->edgeTarget[i]->Parent = CurrentNode;
					}
				}

			}

		}while((CurrentNode->Position.x != EndNode->Position.x)&&(CurrentNode->Position.z != EndNode->Position.z));


		Path.emplace_back(CurrentNode);

		while((CurrentNode->Parent != nullptr)&&(CurrentNode != nullptr)){

			Path.emplace(Path.begin(), CurrentNode->Parent);
			CurrentNode = CurrentNode->Parent;

		}
	}
	NavNode* GiveScore(std::vector<NavNode*> a_Graph, glm::vec3 _Target)
	{
		//																			Calculate score to a position (startnode) to find the node closest to the position.	
		std::vector<NavNode*> Temp;
		Temp = a_Graph;
		for (int i=0;i<a_Graph.size();++i)
		{
			glm::vec2 Pos = a_Graph[i]->Position.xz, Target = _Target.xz;

			a_Graph[i]->Score = glm::length(Target - Pos);

			if(a_Graph[i]->Score < 0)
			{
				a_Graph[i]->Score *= -1;
			}
		}

		//																			sort list of nodes based on score

		std::sort(Temp.begin(), Temp.end(), Compare());
		//																			:. list[0] must be current node

		return Temp[0];
	}
	void SetBehaviour(Behaviour* _Behaviour) {Behave = _Behaviour;}
	void update(float _DeltaTime)
	{
		if(!(bool)Timer){Timer = 1000 * (int)Utility::getDeltaTime(); Attack =!Attack;}
		else{Timer--;}
		
		if (Behave != nullptr)
			Behave->Execute(this);
		if(Path.size() > 0)
			Move(1.0f);
		else
		
		Position += (Velocity * _DeltaTime);
		Velocity = glm::vec3(0);

			//Gizmos::addTri(StartNode->Vertices[0],StartNode->Vertices[1], StartNode->Vertices[2], glm::vec4(0, 0.2, 0, 1));
			//Gizmos::addTri(EndNode->Vertices[0],EndNode->Vertices[1], EndNode->Vertices[2], glm::vec4(0.2, 0, 0, 1));

		
	}

	Behaviour* Behave;

	NavNode *CurrentNode, *EndNode, *StartNode;

	std::vector <NavNode*> Path, Nodes, Open, Closed;
	std::vector <Agent*> Enemies;
	std::vector <Flag*> Flags;

	bool Attack;
	int Timer;
	glm::vec3 Position, Target, Velocity;
};

class Team
{
public:
	Team(){}
	virtual ~Team(){}
	void AddMember(std::vector <NavNode*> _Nodes){Members.emplace_back(new Agent(_Nodes));}
	void DelMember(){Members.erase(Members.begin());}
	
	
	std::vector <Agent*> Members;

};

class Flag
{
public:
	Flag(){fRed , fGreen, fBlue = 0;}
	virtual ~Flag(){}
	glm::vec3 GetPos(){return Position;}
	void Update(){Gizmos::addAABBFilled(Position, glm::vec3(0.1f), glm::vec4(fRed, fGreen, fBlue, 1));}
	
	Team GetController(Team _Red, Team _Blue){if(fRed>fBlue){return _Red;}else{return _Blue;}}

	float fRed, fGreen, fBlue;

	glm::vec3 Position;
};