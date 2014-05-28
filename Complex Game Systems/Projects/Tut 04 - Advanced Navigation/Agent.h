#pragma once

#include "Behaviour.h"
#include "Utilities.h"
#include <glm\glm.hpp>
#include <glm\ext.hpp>
#include <vector>
#include <algorithm>
#include "NavNode.h"
#define V3NULL glm::vec3(-1000)
class Flag;
class Agent
{
public:
 
	//																			add '...' to allow multiple enemies, or add a list. List is probably better.
	//Agent(){Timer = 0;Attack = false;}
	Agent(std::vector <NavNode*> _Nodes)
	{
		v3Target = V3NULL;
		MoveTimer = 0;
		MoveVal = 100;
		PathTimer = 0;
		Target = nullptr;
		Nodes = _Nodes;
		RandColour.x = rand()%100 / 100.0f;//(rand()%50);
		//RandColour.x /=100;
		RandColour.y = rand()%100 / 100.0f;//(rand()%50);
		//RandColour.y /=100;
		RandColour.z = rand()%100 / 100.0f;//(rand()%50);
		//RandColour.z /=100;
		RandColour.w = 1;
	}
	virtual ~Agent (){}

	const glm::vec3& GetPos () const {return Position;}

	Agent* GetTarget () const{return Target;}
	void CalcEnemy(std::vector <Agent*> _Team){Enemies = _Team;}
	void SetPos (glm::vec3 _Pos) { Position = _Pos;}
	void SetTarget (Agent *_Target) { Target = _Target;}
	void SetTarget (glm::vec3 _Target) { v3Target = _Target;}
	void SetTeam (char *_NewTeam){MyTeam = _NewTeam;}
	void FollowPath(float _DeltaTime)
	{
		glm::vec3 Dest = Path[0]->Position;
		
			//																			for now i will just normalise there
			//																			Interpolate linearly to find the third vector // use a number greater than 1 as time // then spline
		//																			glm::normalize(Dest);
		if(!MoveTimer)
		{
			MoveTimer = MoveVal;
			Position = Dest;
			Path.erase(Path.begin());
		}
		else
		{
			MoveTimer--;
		}

		//																			 * _DeltaTime;
	}
	void PathFind(glm::vec3 _EndPos)
	{
		Open.clear();
		Closed.clear();
		Path.clear();

		StartNode = nullptr;
		CurrentNode = nullptr;
		EndNode = nullptr;

		StartNode = GiveScore(Nodes, GetPos());
		CurrentNode = StartNode;//GiveScore(Nodes, _StartPos);
		EndNode = GiveScore(Nodes, _EndPos);

		CurrentNode->Parent = nullptr;

		Open.emplace_back(CurrentNode);
		Open.emplace_back(CurrentNode);
/*		if((CurrentNode->Position.x != EndNode->Position.x)&&(CurrentNode->Position.z != EndNode->Position.z))
		{
			Path.emplace_back(CurrentNode);
			Path.emplace_back(EndNode);
		}*/
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

		}while((CurrentNode->Position.x != EndNode->Position.x)||(CurrentNode->Position.z != EndNode->Position.z));


		Path.emplace_back(CurrentNode);

		while((CurrentNode->Parent != nullptr)/*&&(CurrentNode != nullptr)*/){

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
	void update(float _DeltaTime, glm::vec4 _TeamColour)
	{		
		if (Behave != nullptr)
		{
			Behave->Execute(this);
		}
		//std::cout<<this<<"   "<<MyTeam<<"   "<<MyCurrentBehaviour<<'\n';
		glm::vec3 TargetPos = (GetTarget() != nullptr)?GetTarget()->Position:v3Target;
		if((Path.size() > 0)&&(Last == TargetPos))
		{
			FollowPath(_DeltaTime);
		}
		else
		{
				PathFind(TargetPos);
				Last = TargetPos;
		}
		
		if(Position.x < -	20)	{Velocity.x	+=	0.3;}
		if(Position.x >		20)	{Velocity.x	-=	0.3;}
		if(Position.y < -	20)	{Velocity.y	+=	0.3;}
		if(Position.y >		20)	{Velocity.y	-=	0.3;}

		if(Path.size() > 2)
			{			
				glm::vec3 Pos1 = Position, Pos2 = Path[0]->Position;
				Gizmos::addLine(Pos1, Pos2, RandColour * _TeamColour);
				Gizmos::addAABBFilled(Pos2, glm::vec3(0.1), RandColour * _TeamColour);
				for(int a=0;a<Path.size()-1;a++)
				{
					Pos1 = Path[a]->Position;
					Pos2 = Path[a+1]->Position;
					Gizmos::addLine(Pos1, Pos2, RandColour * _TeamColour);
					Gizmos::addAABBFilled(Pos2, glm::vec3(0.1), RandColour * _TeamColour);
					PathTimer ++;
					if(Path.size()>0)
					{
						if(PathTimer>=Path.size() * 1000)
						{
							PathTimer = 0;
						}
					}
					glm::vec3 Pos3 = Path[(int)PathTimer / 1000]->Position;
					Gizmos::addAABBFilled(Pos3, glm::vec3(0.2), RandColour * _TeamColour);

				}
			}
		Velocity = glm::vec3(0);
		
	}
	void Draw(glm::vec4 _TeamColour)
	{
		Gizmos::addAABBFilled(Position, glm::vec3(0.5f), (RandColour + _TeamColour) / 2);

	}
	Behaviour* Behave;

	NavNode *CurrentNode, *EndNode, *StartNode;

	std::vector <NavNode*> Path, Nodes, Open, Closed;
	std::vector <Agent*> Enemies;
	std::vector <Flag*> Flags;
	Agent *Target;
	int MoveTimer, MoveVal;
	glm::vec3 Position, Velocity, v3Target, Last;
	glm::vec4 RandColour;
	char *MyTeam, *MyCurrentBehaviour;
	float PathTimer;
};

class Team
{
public:
	Team(){}
	virtual ~Team(){}
	void AddMember(std::vector <NavNode*> _Nodes, char *_Team){Members.emplace_back(new Agent(_Nodes));Members.back()->SetTeam(_Team);}
	void DelMember(){Members.erase(Members.begin());}
	
	
	std::vector <Agent*> Members;

};

class Flag
{
public:
	Flag(){fRed = 0, fBlue = 0; Controller = nullptr;LifeTime = 6000;Age = 0;}
	virtual ~Flag(){}
	glm::vec3 GetPos(){return Position;}
	bool Update(Team *_Red, Team *_Blue)
	{
		
		float Rdist = 40, Rdisttemp = 0, Bdist = 40, Bdisttemp = 0;
		for(auto Guy : _Red->Members)
		{
			Rdisttemp = glm::distance(Guy->GetPos(), Position);

			if(Rdist > Rdisttemp)
			{
				Rdist = Rdisttemp;
			}
		}
		for(auto Dude : _Blue->Members)
		{
			Bdisttemp = glm::distance(Dude->GetPos(), Position);

			if(Bdist > Bdisttemp)
			{
				Bdist = Bdisttemp;
			}
		}
			if((Rdist > Bdist)&&(Bdist < 1))
			{
				if(fBlue < 1.00000f)
				{
					fBlue += 0.01;
					//																			printf("blue team \n");
				}
				if(fRed > 0.00000f)
				{
					fRed -= 0.01;
				}
			}
			else if((Bdist > Rdist)&&(Rdist < 1))
			{
				if(fBlue > 0.00000f)
				{
					fBlue -= 0.01;
					//																			printf("red team \n");

				}
				if(fRed < 1.00000f)
				{
					fRed += 0.01;
				}
			}
			char* tempController = GetController();
			if((tempController != nullptr)&&(tempController != Controller))
			{
				//std::cout<<tempController<<'\n';
				Controller = tempController;
			}
			//																			draw
		Gizmos::addAABBFilled(Position, glm::vec3(0.1f, 2, 0.1f), glm::vec4(fRed, 0, fBlue, 1));	
		//																			move the flag every so often
		if(!Age)
		{
			Age = LifeTime;
			return true;
		}
		else 
		{
			Age--;
		}
		return false;
	}
	
	char* GetController(){if(fRed>fBlue){return "Red";}else if(fRed==fBlue){return "Blue";}else{return nullptr;}}

	float fRed, fBlue;
	char* Controller;
	int LifeTime, Age;

	glm::vec3 Position;
};