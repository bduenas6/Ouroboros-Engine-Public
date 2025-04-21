#include "Engine.h"
#include <iostream>
#include <sstream>
#include <string>
#include <filesystem>
#include "rapidjson/document.h"
#include "EngineUtils.h"
#include "Scene.h"
#include "Actor.h"
#include "unordered_map"
#include <vector>
#include "Input.h"
#include "box2d/box2d.h"
#include "EventBus.h"
#include "Rigidbody.h"
#include "ParticleSystem.h"

static std::string obtain_word_after_phrase(const std::string& input, const std::string& phrase) {
	size_t pos = input.find(phrase);
	if (pos == std::string::npos) return "";

	pos += phrase.length();
	while (pos < input.size() && std::isspace(input[pos])) {
		++pos;
	}

	if (pos == input.size()) return "";

	size_t endPos = pos;
	while (endPos < input.size() && !std::isspace(input[endPos])) {
		++endPos;
	}

	return input.substr(pos, endPos - pos);
}

void Engine::GameLoop() 
{
	int stoppedIntro = 0;
	visualRenderer.initialize(Scene::actors.size());
	AudioManager::initialize();
	Input::Init();

	int start_size = Scene::actors.size(); //whyyyyyyyyy

	for (int i = 0; i < start_size; i++) {
		Actor* a = Scene::actors[i];
		for (const std::string c : a->onStartComponents) {
			luabridge::LuaRef ref = (*a->componentMap[c]->componentRef);
			if(ref["enabled"] == true)
			{
				try
				{
					ref["OnStart"](ref);
				}
				catch (luabridge::LuaException e) {
					ComponentDB::ReportError(a->actor_name, e);
				}
				a->componentMap[c]->hasStart = false;
			}
		}
	}

	for (int i = 0; i < Input::GetNumControllers(); i++) {
		//Input::PrintControllerMapping(i);
	}
	
	while (running) 
	{
		//ComponentDB::Sleep(1000);

		//Controller support debug messages:
		//std::cout << "Controller 0 LeftX axis: " << Input::GetControllerAxisS("LeftX", 0) << std::endl;
		//std::cout << "Controller 1 LeftX axis: " << Input::GetControllerAxisS("LeftX", 1) << std::endl;
		//std::cout << "Controller 2 LeftX axis: " << Input::GetControllerAxisS("LeftX", 2) << std::endl;
		//std::cout << "Controller 3 LeftX axis: " << Input::GetControllerAxisS("LeftX", 3) << std::endl;
		//std::cout << "Controller 0 touchpad (X:Y):    " << Input::GetControllerTouchpad(0).x << ":" << Input::GetControllerTouchpad(0).y << std::endl;
		//std::cout << "Controller 0 touchpad (X:Y):    " << Input::GetControllerButton(SDL_CONTROLLER_BUTTON_TOUCHPAD,0) << std::endl;
		//if(Helper::GetFrameNumber() % 60 == 0) std::cout << "Num controllers: " << Input::GetNumControllers() << std::endl;


		if (proceed_to_next_scene) {
			loadScene(next_scene_name);
			proceed_to_next_scene = false;
		}

		Input();

		Update();

		Render();

		Input::LateUpdate();
	}
	return;
}

void Engine::Input()
{
	Input::CheckControllers();

	SDL_Event e;
	while (Helper::SDL_PollEvent(&e)) {
		Input::ProcessEvent(e);
		if (e.type == SDL_QUIT) {
			running = false;
			manual_exit = true;
		}
		//if (e.type == SDL_MOUSEBUTTONDOWN) {
		//	visualRenderer.intro_frame++;
		//}
	}
	//visualRenderer.input();
}

void Engine::Update()
{
	currentScene.ProcessActors();
//    std::cout << "out here" << std::endl;
	EventBus::manageSubscribers(); //please work
//    std::cout << "out there" << std::endl;
	if (Engine::worldExists)
	{
		Engine::world->Step(1.0f / 60.0f, 8, 3);
	}
}

void Engine::Render(bool initial)
{
	visualRenderer.update();
	visualRenderer.present();
	//visualRenderer.clearScreen();
}

void Engine::loadScene(std::string scene_to_load)
{
	// Load scene components
	rapidjson::Document nextSceneDoc;
	if (std::filesystem::exists("resources/scenes/" + scene_to_load + ".scene")) {
		//std::cout << "here" << std::endl;
		EngineUtils::ReadJsonFile("resources/scenes/" + scene_to_load + ".scene", nextSceneDoc);
	}
	else {
		std::cout << "error: scene " + scene_to_load + " is missing";
		exit(0);
	}
	auto actor_list = nextSceneDoc["actors"].GetArray();

	// Delete old actors
	for (auto a : Scene::actors) {
		if (!a->dontDestroy) {
			Scene::staticActorsToDestroy.insert(a);
		}
	}

	for (auto it = Scene::actors.begin(); it != Scene::actors.end();) {
		Actor* ptr = *it;
		if (Scene::staticActorsToDestroy.find(ptr) != Scene::staticActorsToDestroy.end()) {  // Example condition: even values
			delete ptr;              // Delete the object
			it = Scene::actors.erase(it); // Remove pointer from the vector
		}
		else {
			++it; // Only increment the iterator if no deletion was performed
		}
	}
	for (auto it = Scene::staticActorList.begin(); it != Scene::staticActorList.end();) {
		Actor* ptr = *it;
		if (Scene::staticActorsToDestroy.find(ptr) != Scene::staticActorsToDestroy.end()) {  // Example condition: even values
			it = Scene::staticActorList.erase(it); // Remove pointer from the vector
		}
		else {
			++it; // Only increment the iterator if no deletion was performed
		}
	}
	Scene::staticActorsToDestroy.clear();

	// Load new actors
	Scene::actors.reserve(actor_list.Size()); // SHOULDNT break old pointers hopefully but i should be deleting them first anyway

	std::vector<Component> masterComponentList;

	for (const auto& value : actor_list) {
		Actor* temp = new Actor();
		rapidjson::Document templateDoc;
		if (value.HasMember("template")) {
			std::string templateString = value["template"].GetString();
			if (Scene::validTemplates.find(templateString) == Scene::validTemplates.end())
			{
				if (std::filesystem::exists("resources/actor_templates/" + templateString + ".template"))
				{
					EngineUtils::ReadJsonFile("resources/actor_templates/" + templateString + ".template", templateDoc);
				}
				else {
					std::cout << "error: template " + templateString + " is missing";
					exit(0);
				}
				Scene::validTemplates.insert(templateString);
			}
			else {
				EngineUtils::ReadJsonFile("resources/actor_templates/" + templateString + ".template", templateDoc);
			}
		}
		if (value.HasMember("name")) {
			temp->actor_name = value["name"].GetString();
		}
		else {
			if (value.HasMember("template") && templateDoc.HasMember("name")) temp->actor_name = templateDoc["name"].GetString();
			else temp->actor_name = "";
		}
		temp->actorID = Scene::uuid;

		// LUA COMPONENT STUFF HERE ON DOWN

		// Actor has template
		if (value.HasMember("template") && templateDoc.HasMember("components") && templateDoc["components"].IsObject()) {
			const rapidjson::Value& components = templateDoc["components"];
			for (rapidjson::Value::ConstMemberIterator itr = components.MemberBegin(); itr != components.MemberEnd(); ++itr) {
				if (itr->value.IsObject() && itr->value.HasMember("type")) {
					//std::cout << "Component: " << itr->name.GetString() << ", Type: " << itr->value["type"].GetString() << std::endl;
					std::string component_name = itr->name.GetString();
					std::string component_type = itr->value["type"].GetString();
					if (component_type != "Rigidbody" && component_type != "ParticleSystem") { // Standard component
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
					else if (component_type == "Rigidbody") { // Rigidbody component
						Component* c = new Component;
						Rigidbody* rigidbody = new Rigidbody();
						luabridge::LuaRef ref(ComponentDB::lua_state, rigidbody);
						c->componentRef = std::make_shared<luabridge::LuaRef>(ref);

						c->type = "Rigidbody";
						c->hasLateUpdate = false;
						c->hasStart = false;
						c->hasUpdate = false;
						c->hasDestroy = true;

						ref["actor"] = temp;
						ref["enabled"] = true;

						temp->componentMap[component_name] = c;

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
						//something
						if (ref["Ready"].isFunction()) {
							ref["Ready"](ref);
						}
					}
					else { //particle system
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

			if (value.HasMember("components"))
			{
				const rapidjson::Value& componentsToOverride = value["components"];
				for (rapidjson::Value::ConstMemberIterator compItr = componentsToOverride.MemberBegin(); compItr != componentsToOverride.MemberEnd(); ++compItr) {
					if (temp->componentMap[compItr->name.GetString()] != nullptr)
					{
						luabridge::LuaRef componentRef = *temp->componentMap[compItr->name.GetString()]->componentRef;
						const rapidjson::Value& overrides = compItr->value;
						for (rapidjson::Value::ConstMemberIterator itr2 = overrides.MemberBegin(); itr2 != overrides.MemberEnd(); ++itr2) {
							if (itr2->value.IsBool()) {
								componentRef[itr2->name.GetString()] = itr2->value.GetBool();
							}
							else if (itr2->value.IsInt()) {
								componentRef[itr2->name.GetString()] = itr2->value.GetInt();
							}
							else if (itr2->value.IsString()) {
								componentRef[itr2->name.GetString()] = itr2->value.GetString();
							}
							else if (itr2->value.IsFloat()) {
								componentRef[itr2->name.GetString()] = itr2->value.GetFloat();
							}
							else {
								std::cout << "conductor we have a problem";
								exit(0);
							}
						}
					}
					else {
						std::string component_name = compItr->name.GetString();
						std::string component_type = compItr->value["type"].GetString();
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
						luabridge::LuaRef parent_table = luabridge::getGlobal(ComponentDB::lua_state, component_type.c_str());
						luabridge::LuaRef instance_table = luabridge::newTable(ComponentDB::lua_state);
						ComponentDB::EstablishInheritance(instance_table, parent_table);
						instance_table["key"] = component_name;
						instance_table["actor"] = temp;
						instance_table["enabled"] = true;
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

						const rapidjson::Value& overrides = compItr->value;
						for (rapidjson::Value::ConstMemberIterator itr2 = overrides.MemberBegin(); itr2 != overrides.MemberEnd(); ++itr2) {
							if (itr2->name != "type") {
								if (itr2->value.IsBool()) {
									instance_table[itr2->name.GetString()] = itr2->value.GetBool();
								}
								else if (itr2->value.IsInt()) {
									instance_table[itr2->name.GetString()] = itr2->value.GetInt();
								}
								else if (itr2->value.IsString()) {
									instance_table[itr2->name.GetString()] = itr2->value.GetString();
								}
								else if (itr2->value.IsDouble()) {
									instance_table[itr2->name.GetString()] = itr2->value.GetDouble();
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
		// Actor has no template
		else if (value.HasMember("components") && value["components"].IsObject()) {
			const rapidjson::Value& components = value["components"];
			for (rapidjson::Value::ConstMemberIterator itr = components.MemberBegin(); itr != components.MemberEnd(); ++itr) {
				if (itr->value.IsObject() && itr->value.HasMember("type")) {
					//std::cout << "Component: " << itr->name.GetString() << ", Type: " << itr->value["type"].GetString() << std::endl;
					std::string component_name = itr->name.GetString();
					std::string component_type = itr->value["type"].GetString();
					if (component_type != "Rigidbody" && component_type != "ParticleSystem") { // Standard component
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
						luabridge::LuaRef parent_table = luabridge::getGlobal(ComponentDB::lua_state, component_type.c_str());
						luabridge::LuaRef instance_table = luabridge::newTable(ComponentDB::lua_state);
						ComponentDB::EstablishInheritance(instance_table, parent_table);
						instance_table["key"] = component_name;
						instance_table["actor"] = temp;
						instance_table["enabled"] = true;
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
									instance_table[itr2->name.GetString()] = itr2->value.GetString();
								}
								else if (itr2->value.IsDouble()) {
									instance_table[itr2->name.GetString()] = itr2->value.GetDouble();
								}
								else {
									std::cout << "conductor we have a problem";
									exit(0);
								}
							}
						}
					}
					else if (component_type == "Rigidbody") { // Rigidbody component
						Component* c = new Component;
						Rigidbody* rigidbody = new Rigidbody();
						luabridge::LuaRef ref(ComponentDB::lua_state, rigidbody);
						c->componentRef = std::make_shared<luabridge::LuaRef>(ref);

						c->type = "Rigidbody";
						c->hasLateUpdate = false;
						c->hasStart = false;
						c->hasUpdate = false;
						c->hasDestroy = true;

						ref["actor"] = temp;
						ref["enabled"] = true;

						temp->componentMap[component_name] = c;

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

						if (ref["Ready"].isFunction()) {
							ref["Ready"](ref);
						}
					}
					else { //particle system
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


		Scene::uuid++;
		Scene::actors.push_back(temp);
		Scene::staticActorList.push_back(temp);
	}
	proceed_to_next_scene = false;
	current_scene_name = scene_to_load;
}

Engine::Engine(std::string start_message, std::string bad_message, std::string good_message, int x_res, int y_res, Scene& first_scene, Renderer rend)
{
	if (start_message != "") game_start_message = start_message;
	else game_start_message = "";
	if (bad_message != "") game_over_bad_message = bad_message;
	else game_over_bad_message = "";
	if (good_message != "") game_over_good_message = good_message;
	else game_over_good_message = "";
	x_resolution = x_res;
	y_resolution = y_res;
	next_scene_name = "";
	currentScene = first_scene;
	visualRenderer = rend;
}

void Engine::QueueScene(std::string scene)
{
	proceed_to_next_scene = true;
	next_scene_name = scene;
}

std::string Engine::GetScene()
{
	return current_scene_name;
}

void Engine::DontDestroy(Actor* a)
{
	a->dontDestroy = true;
}

void Engine::createWorld()
{
	if (!worldExists) {
		Engine::world = new b2World(b2Vec2(0.0f, 9.8f));
		worldExists = true;
		Engine::world->SetContactListener(&collision_detector);
		raycast_detector = new RaycastDetection();
	}
	else {
		return;
	}
}

float RaycastDetection::ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction)
{
	if (fixture->IsSensor() && fixture->GetFriction() == -999.0f) return 1.0f; //phantom check (might have to be -1?)
	if (reinterpret_cast<Actor*>(fixture->GetUserData().pointer) == nullptr) return -1.0f;
	//std::cout << "passed prelim checks" << std::endl;
	HitResult result;
	result.actor = reinterpret_cast<Actor*>(fixture->GetUserData().pointer);
	result.is_trigger = fixture->IsSensor();
	result.point = point;
	result.normal = normal;
	if (Engine::closest_raycast_hit.has_value() && b2Distance(Engine::raycast_origin, point) < b2Distance(Engine::raycast_origin, Engine::closest_raycast_hit->point)) {
		Engine::closest_raycast_hit = result; //shallow copy fine?
	}
	else if (!Engine::closest_raycast_hit.has_value()) {
		Engine::closest_raycast_hit = result;
	}
	Engine::all_raycast_hits.push_back(result);
	return 1.0f;
}


luabridge::LuaRef Engine::ActorRaycast(b2Vec2 pos, b2Vec2 dir, float dist) //must be changed to return a luaref so that we can return nil if needed
{
	if (!worldExists) return luabridge::LuaRef(ComponentDB::lua_state);
	if (dist <= 0.0f) return luabridge::LuaRef(ComponentDB::lua_state);
	closest_raycast_hit.reset();
	all_raycast_hits.clear();
	raycast_origin = pos;
	dir.Normalize();
	b2Vec2 point2 = pos + (dist * dir);
	//std::cout << raycast_detector << std::endl;
	world->RayCast(raycast_detector, pos, point2);
	if (!Engine::closest_raycast_hit.has_value()) return luabridge::LuaRef(ComponentDB::lua_state);
	HitResult result = Engine::closest_raycast_hit.value();
	luabridge::LuaRef ref(ComponentDB::lua_state, result);
	return ref; //make a hitresult and fill it with stuff
}

inline static bool resultComp(HitResult a, HitResult b) {
	if (b2Distance(Engine::raycast_origin, a.point) < b2Distance(Engine::raycast_origin, b.point)) return true;
	return false;
}

luabridge::LuaRef Engine::ActorRaycastAll(b2Vec2 pos, b2Vec2 dir, float dist)
{
	if (!worldExists) return luabridge::LuaRef(ComponentDB::lua_state);
	if (dist <= 0.0f) return luabridge::LuaRef(ComponentDB::lua_state);
	closest_raycast_hit.reset();
	all_raycast_hits.clear();
	raycast_origin = pos;
	dir.Normalize();
	b2Vec2 point2 = pos + (dist * dir);
	world->RayCast(raycast_detector, pos, point2);
	if (all_raycast_hits.size() > 1) {
		std::stable_sort(all_raycast_hits.begin(), all_raycast_hits.end(), resultComp);
	}
	luabridge::LuaRef resultTable = luabridge::newTable(ComponentDB::lua_state);
	int index = 1;
	for (auto& result : all_raycast_hits) {
		resultTable[index++] = result;
	}
	return resultTable; //make a hitresult and fill it with stuff
}
