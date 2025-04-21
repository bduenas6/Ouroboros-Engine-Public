#include "Scene.h"
#include <iostream>
#include <vector>
#include "Helper.h"
#include "AudioManager.h"
#include "rapidjson/document.h"
#include "EngineUtils.h"
#include "Rigidbody.h"
#include "ParticleSystem.h"

Actor* Scene::Find(std::string name)
{
	for (auto c : staticActorList) {
		if (c->actor_name == name && (std::find(staticActorsToDestroy.begin(), staticActorsToDestroy.end(), c) == staticActorsToDestroy.end()))
		{
			//std::cout << "here" << std::endl;
			//if (std::find(staticActorsToDestroy.begin(), staticActorsToDestroy.end(), c) == staticActorsToDestroy.end()) std::cout << "here:" << staticActorsToDestroy.size() << std::endl;
			return c;
		}
	}
	return luabridge::LuaRef(ComponentDB::lua_state); //dont work
}

luabridge::LuaRef Scene::FindAll(std::string name)
{
	luabridge::LuaRef table = luabridge::newTable(ComponentDB::lua_state);
	int index = 1;
	for (auto c : staticActorList) {
		if (c->actor_name == name && (std::find(staticActorsToDestroy.begin(), staticActorsToDestroy.end(), c) == staticActorsToDestroy.end())) table[index++] = c;
	}
	return table;
}

Actor* Scene::Instantiate(std::string actor_template_name)
{
	Actor* temp = new Actor();
	rapidjson::Document templateDoc;
	if (Scene::validTemplates.find(actor_template_name) == Scene::validTemplates.end())
	{
		if (std::filesystem::exists("resources/actor_templates/" + actor_template_name + ".template"))
		{
			EngineUtils::ReadJsonFile("resources/actor_templates/" + actor_template_name + ".template", templateDoc);
		}
		else {
			std::cout << "error: template " + actor_template_name + " is missing";
			exit(0);
		}
		Scene::validTemplates.insert(actor_template_name);
	}
	else {
		EngineUtils::ReadJsonFile("resources/actor_templates/" + actor_template_name + ".template", templateDoc);
	}
	if (templateDoc.HasMember("name")) temp->actor_name = templateDoc["name"].GetString();
	else temp->actor_name = "";
	temp->actorID = uuid++;

	// LUA COMPONENT STUFF HERE ON DOWN

	// Actor has template
	if (templateDoc.HasMember("components") && templateDoc["components"].IsObject()) {
		const rapidjson::Value& components = templateDoc["components"];
		for (rapidjson::Value::ConstMemberIterator itr = components.MemberBegin(); itr != components.MemberEnd(); ++itr) {
			if (itr->value.IsObject() && itr->value.HasMember("type")) {
				//std::cout << "Component: " << itr->name.GetString() << ", Type: " << itr->value["type"].GetString() << std::endl;
				std::string component_name = itr->name.GetString();
				std::string component_type = itr->value["type"].GetString();
				if (!ComponentDB::TableExists(component_type)) {
					if (!std::filesystem::exists("resources/component_types/" + component_type + ".lua"))
					{
						std::cout << "error: failed to locate component " + component_type;
						exit(0);
					}
					if (luaL_dofile(ComponentDB::lua_state, ("resources/component_types/" + component_type + ".lua").c_str()) != LUA_OK) {
						std::cout << "problem with lua file " + component_type;
						exit(0);
					}
				}
				if(component_type != "Rigidbody" && component_type != "ParticleSystem")
				{
					luabridge::LuaRef parent_table = luabridge::getGlobal(ComponentDB::lua_state, component_type.c_str());
					luabridge::LuaRef instance_table = luabridge::newTable(ComponentDB::lua_state);
					ComponentDB::EstablishInheritance(instance_table, parent_table);
					instance_table["key"] = component_name;
					instance_table["actor"] = temp;
					instance_table["enabled"] = true;
					//temp->InjectConvenienceReferences(instance_table);

					Component* c = new Component;
					c->componentRef = std::make_shared<luabridge::LuaRef>(instance_table);
					c->type = component_type;
					temp->componentMap[component_name] = c;
					if (instance_table.isTable()) {
						luabridge::LuaRef onStartFunc = instance_table["OnStart"];
						if (!onStartFunc.isNil()) {
							c->hasStart = true;
							temp->onStartComponents.insert(component_name);
						}
						luabridge::LuaRef onUpdateFunc = instance_table["OnUpdate"];
						if (!onUpdateFunc.isNil()) {
							c->hasUpdate = true;
							temp->onUpdateComponents.insert(component_name);
						}
						luabridge::LuaRef onLateUpdateFunc = instance_table["OnLateUpdate"];
						if (!onLateUpdateFunc.isNil()) {
							c->hasLateUpdate = true;
							temp->onLateUpdateComponents.insert(component_name);
						}
						luabridge::LuaRef onDestroyFunc = instance_table["OnDestroy"];
						if (!onDestroyFunc.isNil()) {
							c->hasDestroy = true;
							temp->onDestroyComponents.insert(component_name);
						}
					}
					else {
						std::cout << "conductor we have a problem";
						exit(0);
					}
					//std::cout << temp->actor_name << std::endl;
					const rapidjson::Value& overrides = itr->value;
					for (rapidjson::Value::ConstMemberIterator itr2 = overrides.MemberBegin(); itr2 != overrides.MemberEnd(); ++itr2) {
						if (itr2->name != "type") {
							if (itr2->value.IsBool()) {
								instance_table[itr2->name.GetString()] = itr2->value.GetBool();
							}
							else if (itr2->value.IsInt()) {
								instance_table[itr2->name.GetString()] = itr2->value.GetInt();
							}
							else if (itr2->value.IsString()) {
								//std::cout << itr2->name.GetString() << ":" << itr2->value.GetString() << std::endl;
								instance_table[itr2->name.GetString()] = itr2->value.GetString();
							}
							else if (itr2->value.IsFloat()) {
								instance_table[itr2->name.GetString()] = itr2->value.GetFloat();
							}
							else {
								std::cout << "conductor we have a problem";
								exit(0);
							}
						}
					}
				}
				else if (component_type == "Rigidbody") {
					Component* c = new Component;
					Rigidbody* rigidbody = new Rigidbody();
					luabridge::LuaRef ref(ComponentDB::lua_state, rigidbody);
					c->componentRef = std::make_shared<luabridge::LuaRef>(ref);

					c->type = "Rigidbody";
					c->hasLateUpdate = false;
					c->hasStart = true;
					c->hasUpdate = false;
					c->hasDestroy = true;

					ref["actor"] = temp;
					ref["enabled"] = true;

					temp->componentMap[component_name] = c;
					temp->onStartComponents.insert(component_name);

					const rapidjson::Value& overrides = itr->value;
					for (rapidjson::Value::ConstMemberIterator itr2 = overrides.MemberBegin(); itr2 != overrides.MemberEnd(); ++itr2) {
						if (itr2->name != "type") {
							if (itr2->value.IsBool()) {
								ref[itr2->name.GetString()] = itr2->value.GetBool();
							}
							else if (itr2->value.IsInt()) {
								ref[itr2->name.GetString()] = itr2->value.GetInt();
							}
							else if (itr2->value.IsString()) {
								//std::cout << itr2->name.GetString() << ":" << itr2->value.GetString() << std::endl;
								ref[itr2->name.GetString()] = itr2->value.GetString();
							}
							else if (itr2->value.IsFloat()) {
								ref[itr2->name.GetString()] = itr2->value.GetFloat();
							}
							else {
								std::cout << "conductor we have a problem";
								exit(0);
							}
						}
					}
				}
				else {
					Component* c = new Component;
					ParticleSystem* psystem = new ParticleSystem();
					luabridge::LuaRef ref(ComponentDB::lua_state, psystem);
					c->componentRef = std::make_shared<luabridge::LuaRef>(ref);

					c->type = "ParticleSystem";
					c->hasLateUpdate = false;
					c->hasStart = true; //might need to change
					c->hasUpdate = true;
					c->hasDestroy = false;

					ref["actor"] = temp;
					ref["enabled"] = true;

					temp->componentMap[component_name] = c;
					temp->onStartComponents.insert(component_name);
					temp->onUpdateComponents.insert(component_name);

					const rapidjson::Value& overrides = itr->value;
					for (rapidjson::Value::ConstMemberIterator itr2 = overrides.MemberBegin(); itr2 != overrides.MemberEnd(); ++itr2) {
						if (itr2->name != "type") {
							if (itr2->value.IsBool()) {
								ref[itr2->name.GetString()] = itr2->value.GetBool();
							}
							else if (itr2->value.IsInt()) {
								ref[itr2->name.GetString()] = itr2->value.GetInt();
							}
							else if (itr2->value.IsString()) {
								//std::cout << itr2->name.GetString() << ":" << itr2->value.GetString() << std::endl;
								ref[itr2->name.GetString()] = itr2->value.GetString();
							}
							else if (itr2->value.IsFloat()) {
								ref[itr2->name.GetString()] = itr2->value.GetFloat();
							}
							else {
								std::cout << "conductor we have a problem";
								exit(0);
							}
						}
					}
				}
			}
		}
	}
	Scene::staticActorList.push_back(temp);
	Scene::staticActorsToAdd.push_back(temp);
	return temp;
}

void Scene::Destroy(Actor* actor)
{
	Scene::staticActorsToDestroy.insert(actor);
	for (auto c : actor->componentMap) {
		(*c.second->componentRef)["enabled"] = false;
	}
}

void Scene::ProcessActors()
{
	//std::cout << "new frame" << std::endl;
	for (int i = 0; i < actors.size(); i++) {
		//std::cout << "actor " << actors[i]->actor_name << " has " << actors[i]->onStartComponents.size() << " start components" << std::endl;
		if (actors[i]->onStartComponents.size() != 0) {
			for (std::string name : actors[i]->onStartComponents) {
				//std::cout << name << std::endl;
				luabridge::LuaRef ref = *actors[i]->componentMap[name]->componentRef;
				if (ref["enabled"] == true && actors[i]->componentMap[name]->hasStart)
				{
					if (actors[i]->componentMap[name]->type == "Rigidbody") {
						//std::cout << "gonna call ready" << std::endl;
						if (ref["Ready"].isFunction()) {
							ref["Ready"](ref);
							//std::cout << "called ready" << std::endl;
						}
					}
					else {
						try
						{
							ref["OnStart"](ref);
						}
						catch (luabridge::LuaException e) {
							ComponentDB::ReportError(actors[i]->actor_name, e);
						}
					}
					actors[i]->componentMap[name]->hasStart = false;
				}
				// might need else here depending on an edge case
			}
		}
	}
	//std::cout << "passed on start" << std::endl;
	for (int i = 0; i < actors.size(); i++) {
		//std::cout << "actor " << actors[i]->actor_name << " has " << actors[i]->onUpdateComponents.size() << " update components" << std::endl;
		if (actors[i]->onUpdateComponents.size() != 0) {
			for (std::string name : actors[i]->onUpdateComponents) {
				//std::cout << actors[i]->actor_name << " " << name << std::endl;
				luabridge::LuaRef ref = *actors[i]->componentMap[name]->componentRef;
				if (ref["enabled"] == true)
				{
					try
					{
						ref["OnUpdate"](ref);
					}
					catch (luabridge::LuaException e) {
						ComponentDB::ReportError(actors[i]->actor_name, e);
					}
				}
				else {

				}
			}
		}	
	}

	for (int i = 0; i < actors.size(); i++) {
		//std::cout << "actor " << actors[i]->actor_name << " has " << actors[i]->onLateUpdateComponents.size() << " late update components" << std::endl;
		if (actors[i]->onLateUpdateComponents.size() != 0) {
			for (std::string name : actors[i]->onLateUpdateComponents) {
				//std::cout << actors[i]->actor_name << " " << name << std::endl;
				luabridge::LuaRef ref = *actors[i]->componentMap[name]->componentRef;
				if (ref["enabled"] == true)
				{
					try
					{
						ref["OnLateUpdate"](ref);
					}
					catch (luabridge::LuaException e) {
						ComponentDB::ReportError(actors[i]->actor_name, e);
					}
				}
				else {

				}
			}
		}
	}

	for (int i = 0; i < actors.size(); i++) {
        std::unordered_set<std::string> elements_to_remove;
		if (actors[i]->onDestroyComponents.size() != 0) {
			//std::cout << "count is right" << std::endl;
			for (std::string name : actors[i]->onDestroyComponents) {
//				std::cout << actors[i]->actor_name << " " << name << std::endl;
				luabridge::LuaRef ref = *actors[i]->componentMap[name]->componentRef;
				if(ref["enabled"].cast<bool>() != true)
				{
//                    std::cout << "here" << std::endl;
					if (actors[i]->componentMap[name]->type == "Rigidbody") {
						Engine::world->DestroyBody((*actors[i]->componentMap[name]->componentRef)["body"]);
					}
					else {
						try
						{
							ref["OnDestroy"](ref);
						}
						catch (luabridge::LuaException e) {
							ComponentDB::ReportError(actors[i]->actor_name, e);
						}
					}
                    elements_to_remove.insert(name);
//					actors[i]->onDestroyComponents.erase(name); // might cause problems on mac, idk
				}
			}
            for(auto name : elements_to_remove){
                actors[i]->onDestroyComponents.erase(name);
            }
		}

		if (actors[i]->componentsToAdd.size() != 0) {
			for (auto c : actors[i]->componentsToAdd)
			{
				luabridge::LuaRef ref = *c->componentRef;
				std::string component_name = ref["key"];
				actors[i]->componentMap[component_name] = c;
				if (c->hasStart) actors[i]->onStartComponents.insert(component_name);
				if (c->hasUpdate) actors[i]->onUpdateComponents.insert(component_name);
				if (c->hasLateUpdate) actors[i]->onLateUpdateComponents.insert(component_name);
			}
		}
		actors[i]->componentsToAdd.clear();
		for (auto a : Scene::staticActorsToAdd) {
			actors.push_back(a);
		}
		Scene::staticActorsToAdd.clear();
	}

	//std::cout << Scene::staticActorsToDestroy.size() << std::endl;

	for (auto it = actors.begin(); it != actors.end();) {
		Actor* ptr = *it;
		if (Scene::staticActorsToDestroy.find(ptr) != Scene::staticActorsToDestroy.end()) {
			//call ondestroy first
			//std::cout << "checking count" << std::endl;
			if (ptr->onDestroyComponents.size() != 0) {
				//std::cout << "count is right" << std::endl;
				for (std::string name : ptr->onDestroyComponents) {
					//std::cout << actors[i]->actor_name << " " << name << std::endl;
					luabridge::LuaRef ref = *ptr->componentMap[name]->componentRef;
					if (ptr->componentMap[name]->type == "Rigidbody") {
						Engine::world->DestroyBody((*ptr->componentMap[name]->componentRef)["body"]);
					}
					else {
						try
						{
							ref["OnDestroy"](ref);
						}
						catch (luabridge::LuaException e) {
							ComponentDB::ReportError(ptr->actor_name, e);
						}
					}
				}
			}

			delete ptr;              // Delete the object
			it = actors.erase(it); // Remove pointer from the vector
		}
		else {
			++it; // Only increment the iterator if no deletion was performed
		}
	}
	for (auto it = Scene::staticActorList.begin(); it != Scene::staticActorList.end();) {
		Actor* ptr = *it;
		if (Scene::staticActorsToDestroy.find(ptr) != Scene::staticActorsToDestroy.end()) {
			it = Scene::staticActorList.erase(it); // Remove pointer from the vector
		}
		else {
			++it; // Only increment the iterator if no deletion was performed
		}
	}
	Scene::staticActorsToDestroy.clear();

	//std::cout << "here" << std::endl;
}

static bool comp(Actor* a, Actor* b) {
	return a->actorID < b->actorID;
}
