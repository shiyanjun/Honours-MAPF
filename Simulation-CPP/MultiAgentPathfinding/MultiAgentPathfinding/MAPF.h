#pragma once

#include <deque>
#include <unordered_set>
#include <set>
#include "Tile.h"

class Agent;
class CollisionPenalties;

namespace MAPF
{
	struct CollisionPenalties
	{
		std::map<int, std::map<std::pair<Tile*, Tile*>, float>> edge;
	};


	struct Path
	{
		std::deque<Tile*> tiles;
		int cost;
		CollisionPenalties penalties;

		Path(CollisionPenalties penalties) 
			: cost(0), penalties(penalties)
		{
		}

		Path() : cost(0)
		{
		}

		friend bool operator==(const Path& a, const Path& b)
		{
			/*return a.penalties.size() == rhs.size()
				&& std::equal(lhs.begin(), lhs.end(),
					rhs.begin());*/

			// std::map<int, std::map<std::pair<Tile*, Tile*>, float>> edge;
			return a.tiles == b.tiles && a.cost == b.cost && a.penalties.edge == b.penalties.edge;
		}

		friend std::ostream& operator<<(std::ostream& os, Path& path);
	};

	//using Path = std::deque<Tile*>;
	using TileTime = std::pair<Tile*, int>;

	struct AgentPathRef
	{
		static std::vector<AgentPathRef*> PATH_REF_POOL;

		Agent* agent;
		int pathIndex;

	public:
		AgentPathRef(Agent* agent, int pathIndex) : agent(agent), pathIndex(pathIndex) {}

	public:
		//static AgentPathRef* Make(Agent* agent, int pathIndex, std::vector<AgentPathRef*>& usedPathRefs = std::vector<AgentPathRef*>());
		//static AgentPathRef* MakeNull(Agent* agent);

		Path& GetPath();
		bool IsValid();

		friend bool operator==(const AgentPathRef& a, const AgentPathRef& b)
		{
			return a.agent == b.agent && a.pathIndex == b.pathIndex;
		}

		friend std::ostream& operator<<(std::ostream& os, AgentPathRef& pathRef);
	};

	struct AgentPathRefHash
	{
		std::size_t operator()(const AgentPathRef& pathRef) const
		{
			return 1;// std::hash<int>()((int) pathRef.agent + pathRef.pathIndex);
		}
	};

	using PathConstraint = std::set<AgentPathRef*>;//std::vector<std::set<AgentPathRef*, AgentPathRefHash>>;

	using Edge = std::pair<Tile*, Tile*>;

	struct PathCollision
	{
		AgentPathRef* a;
		AgentPathRef* b;

		int delta;

		PathCollision(AgentPathRef* a, AgentPathRef* b);

		int SmallestPathBankSize();

		friend bool operator==(const PathCollision& a, const PathCollision& b)
		{
			return (a.a == b.a && a.b == b.a) || (a.a == b.b && a.b == b.a);
		}

		friend bool operator<(PathCollision& a, PathCollision& b)
		{
			return (a.a == b.a && a.b == b.a) || (a.a == b.b && a.b == b.a);
		}

		friend std::ostream& operator<<(std::ostream& os, MAPF::PathCollision& collision);
	};

	struct PathCollisionHash
	{
		std::size_t operator()(const PathCollision& collision) const
		{
			return std::hash<int>()(int(collision.a) + int(collision.b));
		}
	};

	struct DeltaComparator
	{
		bool operator() (PathCollision& a, PathCollision& b)
		{
			return a.SmallestPathBankSize() < b.SmallestPathBankSize();

			// if delta is the same, then use the collision which has the smaller alternative paths
			if (a.delta == b.delta)
			{
				return a.SmallestPathBankSize() < b.SmallestPathBankSize();
			}

			return a.delta < b.delta;

		}
	};
}