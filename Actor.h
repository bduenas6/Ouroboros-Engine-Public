#pragma once
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include <optional>
#include <unordered_set>
#include "ComponentDB.h"
#include "set"
#include "box2d/box2d.h"

class Collision;

struct Actor
{
public:
	std::string actor_name;
	int actorID = -1;
	bool dontDestroy = false;

	std::set<std::string> onStartComponents;
	std::set<std::string> onUpdateComponents;
	std::set<std::string> onLateUpdateComponents;
	std::set<std::string> onDestroyComponents;
	std::map<std::string, Component*> componentMap;

	std::string getName();
	int getID();
	void InjectConvenienceReferences(std::shared_ptr<luabridge::LuaRef> component_ref);
	luabridge::LuaRef getComponentByKey(std::string key);
	luabridge::LuaRef getComponent(std::string type);
	luabridge::LuaRef getComponents(std::string type);
	luabridge::LuaRef AddComponent(std::string type_name);
	void RemoveComponent(luabridge::LuaRef component_ref);

	std::vector<Component*> componentsToAdd;

	static inline int componentsAdded;

	void OnCollisionEnter(Collision col);
	void OnCollisionExit(Collision col);

	void OnTriggerEnter(Collision col);
	void OnTriggerExit(Collision col);

	Actor(std::string actor_name)
		: actor_name(actor_name){
	}

	Actor() {}
};

class Collision {
public:
	Actor* other;
	b2Vec2 point;
	b2Vec2 relative_velocity;
	b2Vec2 normal;
};

class CollisionDetection : public b2ContactListener {
public:
	void BeginContact(b2Contact* contact);

	void EndContact(b2Contact* contact);
};