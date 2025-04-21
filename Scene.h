#pragma once
#include <string>
#include <glm/glm.hpp>
#include "unordered_map"
#include "Actor.h"

struct KeyFuncs
{
	size_t operator()(const glm::ivec2& k)const
	{
		return std::hash<int>()(k.x) ^ std::hash<int>()(k.y);
	}

	bool operator()(const glm::ivec2& a, const glm::ivec2& b)const
	{
		return a.x == b.x && a.y == b.y;
	}
};

class Scene
{
public:
	static inline std::vector<Actor*> actors; // Actors available for function calls
	static inline std::vector<Actor*> staticActorList; // Actors available for lookup
	static inline std::vector<Actor*> staticActorsToAdd;
	static inline std::set<Actor*> staticActorsToDestroy;
	static inline std::set<std::string> validTemplates;
	static Actor* Find(std::string name);
	static luabridge::LuaRef FindAll(std::string name);
	static Actor* Instantiate(std::string actor_template_name);
	static void Destroy(Actor* actor);

	void ProcessActors();
	static inline int uuid;
};
