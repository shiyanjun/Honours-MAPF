#pragma once
#include <vector>
#include <queue>
#include <map>
#include <list>
#include "Timer.h"
#include "Helper.h"
#include <set>
#include "MAPF.h"
#include "PriorityQueue.h"
#include <iostream>

class Tile;
class GridMap;

struct AStarTileTime
{
private:
	AStarTileTime()
	{
		Reset();
	}

public:
	static std::vector<AStarTileTime*> TILE_TIME_POOL;
	static AStarTileTime* Make(std::vector<AStarTileTime*>& usedTileTimes);
	static void FillPool(int num)
	{
		for (int i = 0; i < num; i++)
			TILE_TIME_POOL.push_back(new AStarTileTime());
	};

	bool bClosed = false;
	bool bIsInOpen = false;

	AStarTileTime* parent;

	int timestep = 0;
	Tile* tile;
	float f = 0;
	float g = 0;
	float penalty = 0;
	float h = 0;

	void SetParent(AStarTileTime* parent)
	{
		this->parent = parent;
		this->g = parent ? parent->g + 1 : 0;
	}

	void UpdateCosts()
	{
		this->f = g + h + penalty;
	}

	void SetInfo(AStarTileTime* parent, int timestep, Tile* tile, float heuristic, float customCost)
	{
		SetParent(parent);

		this->timestep = timestep;
		this->tile = tile;

		this->h = heuristic;
		this->penalty = customCost;

		UpdateCosts();
	}

	void Reset()
	{
		bClosed = false;
		parent = nullptr;
		bIsInOpen = false;

		parent = nullptr;
		tile = nullptr;
		timestep = f = g = penalty = h = 0;
	}


	int priority = INT_MAX;
	int GetPriority() { return priority; }
	void SetPriority(int priority)
	{
		this->priority = priority;
	}

	bool operator<(const AStarTileTime& other)
	{
		if (f == other.f)
		{
			if (penalty == other.penalty)
			{
				return g < other.g;
			}

			return penalty < other.penalty;
		}

		return f < other.f;
	}

	bool operator>(const AStarTileTime& other)
	{
		if (f == other.f)
		{
			if (penalty == other.penalty)
			{
				return g > other.g;
			}

			return penalty > other.penalty;
		}

		return f > other.f;
	}

	friend std::ostream& operator<<(std::ostream& os, AStarTileTime& tileTime)
	{
		os << *tileTime.tile <<
			" | time: " << tileTime.timestep <<
			" | f: " << tileTime.f <<
			" | g: " << tileTime.g <<
			" | h: " << tileTime.h <<
			" | penalty: " << tileTime.penalty;

		return os;
	}
};

struct BaseHeuristic
{
public:
	BaseHeuristic() = default;
	bool operator()(AStarTileTime* A, AStarTileTime* B);
	friend bool operator==(const AStarTileTime &a, const AStarTileTime &b)
	{
		return a.timestep == b.timestep && a.tile == b.tile;
	}
};

class TemporalAStar
{
private:
	std::vector<AStarTileTime*> usedTileTimes;
	std::vector<AStarTileTime*> closed;
	bool needsSorting;

public:


	static int GLOBAL_TILES_EXPANDED;
	int LOCAL_TILES_EXPANDED;

public:
	using TileCosts = std::map<int, std::map<Tile*, float>>;

private:
	Timer timer;
	Timer sortTimer;

	GridMap* gridMap;

public:
	TemporalAStar(GridMap* gridMap);
	void SetGridMap(GridMap* gridMap) { this->gridMap = gridMap; }
	~TemporalAStar();

public:
	using OpenQueue = PriorityQueue<AStarTileTime*>;

	std::vector<Tile*> modifiedTiles;
	std::vector<AStarTileTime*> modifiedTileTimes;

	std::map<Tile*, std::map<int, AStarTileTime*>> spatialGridMap;

	MAPF::Path FindPath(Tile* start, Tile* goal, TileCosts& customCosts = TileCosts());

	void ExpandNeighbor(OpenQueue& open, AStarTileTime* current, Tile* neighborTile, Tile* start, Tile* goal, TileCosts& customCosts);

	int GetCustomCosts(int timestep, Tile* tile, TileCosts& customCosts);
};

