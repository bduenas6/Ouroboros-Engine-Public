#pragma once
#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include <iostream>
#include <algorithm>
#include <thread>
#include "Helper.h"

class ComponentDB
{
public:
	static void EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table);

	static bool TableExists(const std::string& table_name);

	static inline lua_State* lua_state;

	static void CppLog(std::string message);

	static void ReportError(const std::string& actor_name, const luabridge::LuaException& e);

	static void Quit();

	static void Sleep(int milliseconds);

	static int GetFrame();

	static void OpenURL(std::string url);

};

class Component {
public:
	explicit Component();

	bool isEnabled();

	std::shared_ptr<luabridge::LuaRef> componentRef;
	std::string type;

	bool hasStart;
	bool hasUpdate;
	bool hasLateUpdate;
	bool hasDestroy;
};

