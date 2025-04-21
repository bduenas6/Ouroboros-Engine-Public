#pragma once
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include <optional>
#include <unordered_map>
#include "ComponentDB.h"
#include "box2d/box2d.h"
#include <tuple>

class EventBus
{
public:
	static inline std::unordered_map < std::string, std::vector < std::pair < luabridge::LuaRef, luabridge::LuaRef>>> current_subscribers;
    //static inline std::vector < std::pair < std::string, std::pair< luabridge::LuaRef, luabridge::LuaRef >>> new_subscribers; //event_type, component, function
	static inline std::vector < std::tuple < std::string, luabridge::LuaRef, luabridge::LuaRef >> new_subscribers; //event_type, component, function
    //static inline std::vector < std::pair < std::string, std::pair< luabridge::LuaRef, luabridge::LuaRef >>> new_unsubscribers; //event_type, component, function
	static inline std::vector< std::tuple < std::string, luabridge::LuaRef, luabridge::LuaRef >> new_unsubscribers; //event_type, component, function

	static void Publish(std::string event_type, luabridge::LuaRef event_object);
	static void Subscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function);
	static void Unsubscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function);
    
	static void manageSubscribers();
};

