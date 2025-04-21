#include "ComponentDB.h"

void ComponentDB::EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table)
{
	luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
	new_metatable["__index"] = parent_table;

	instance_table.push(lua_state);
	new_metatable.push(lua_state);
	lua_setmetatable(lua_state, -2);
	lua_pop(lua_state, 1);
}

bool ComponentDB::TableExists(const std::string& table_name)
{
	lua_getglobal(lua_state, table_name.c_str());
	bool exists = lua_istable(lua_state, -1);
	lua_pop(lua_state, 1);
	return exists;
}

void ComponentDB::CppLog(std::string message)
{
	std::cout << message << std::endl;
}

void ComponentDB::ReportError(const std::string& actor_name, const luabridge::LuaException& e) {
	std::string error_message = e.what();
	std::replace(error_message.begin(), error_message.end(), '\\', '/');
	std::cout << "\033[31m" << actor_name << " : " << error_message << "\033[0m" << std::endl;
}

void ComponentDB::Quit()
{
	exit(0);
}

void ComponentDB::Sleep(int milliseconds)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

int ComponentDB::GetFrame()
{
	return Helper::GetFrameNumber();
}

void ComponentDB::OpenURL(std::string url)
{
#ifdef __linux__ 
	std::string command = "xdg-open " + url;
#elif _WIN32
	std::string command = "start " + url;
#else
	std::string command = "open " + url;
#endif
	std::system(command.c_str());
}

Component::Component()
{
	hasStart = false;
	hasUpdate = false;
	hasLateUpdate = false;
}

bool Component::isEnabled()
{
	return (*componentRef)["enabled"];
}
