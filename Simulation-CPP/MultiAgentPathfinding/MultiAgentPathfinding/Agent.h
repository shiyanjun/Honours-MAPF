#pragma once
#include "EObject.h"
#include <vector>
#include <deque>
#include <iostream>
#include "SpatialAStar.h"
#include "Graphics.h"

class Tile;
class GridMap;

class Agent : public EObject
{
private:
	SpatialAStar* search;
	GridMap* map;

	int agentId;

public:
	Tile* goal;

	vec3 color;
	Agent(Tile* startTile, Tile* goalTile = nullptr);
	virtual void step();
	void setPath(SpatialAStar::Path& path);

	std::vector<SpatialAStar::Path> allPaths;
	SpatialAStar::Path currentPath;

	std::map<Tile*, int> customWeights;

	int getAgentId() { return agentId; }

	vec3 renderPos;
	void update(float dt);

	void drawPaths(Graphics* graphics);

	friend std::ostream& operator<<(std::ostream& os, Agent& agent);
};

