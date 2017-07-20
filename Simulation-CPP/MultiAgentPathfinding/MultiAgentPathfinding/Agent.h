#pragma once
#include "EObject.h"
#include <vector>
#include <deque>
#include <iostream>
#include "AStar.h"
#include "Graphics.h"

class Tile;
class GridMap;

class Agent : public EObject
{
private:
	AStar* search;
	GridMap* map;

	int agentId;

public:
	Tile* goal;

	vec3 color;
	Agent(int x, int y);
	virtual void step();
	void setPath(AStar::Path& path);

	std::vector<AStar::Path> allPaths;
	AStar::Path path;

	int getAgentId() { return agentId; }

	vec3 renderPos;
	void update(float dt);

	void drawPaths(Graphics* graphics);

	friend std::ostream& operator<<(std::ostream& os, Agent& agent);
};
