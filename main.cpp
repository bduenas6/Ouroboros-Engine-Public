#include <iostream>
#include <filesystem>
#include "glm/glm.hpp"
#include "Engine.h"
#include "rapidjson/document.h"
#include "EngineUtils.h"
#include "Scene.h"
#include "Actor.h"
#include "SDL2/SDL.h"
#include "SDL2_image/SDL_image.h"
#include "AudioManager.h"
#include "Helper.h"
#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "ComponentDB.h"
#include "Input.h"
#include "box2d/box2d.h"
#include "Rigidbody.h"
#include "EventBus.h"
#include "ParticleSystem.h"

int main(int argc, char* argv[]) {
	
	rapidjson::Document gameconfig;
	rapidjson::Document renderingconfig;
	if (std::filesystem::exists("resources/")) {
		if (std::filesystem::exists("resources/game.config")) {
			EngineUtils::ReadJsonFile("resources/game.config", gameconfig);
		}
		else {
			std::cout << "error: resources/game.config missing";
			exit(0);
		}
	}
	else {
		std::cout << "error: resources/ missing";
		exit(0);
	}
	
	ComponentDB::lua_state = luaL_newstate();
	luaL_openlibs(ComponentDB::lua_state);
	Actor::componentsAdded = 0;

	// Global functions for lua

	luabridge::getGlobalNamespace(ComponentDB::lua_state)
		.beginNamespace("Debug")
		.addFunction("Log", ComponentDB::CppLog)
		.addFunction("LogError", ComponentDB::CppLog)
		.addFunction("PrintControllerMapping", Input::PrintControllerMapping)
		.endNamespace()
		.beginClass<Actor>("Actor")
		.addFunction("GetName", &Actor::getName)
		.addFunction("GetID", &Actor::getID)
		.addFunction("GetComponentByKey", &Actor::getComponentByKey)
		.addFunction("GetComponent", &Actor::getComponent)
		.addFunction("GetComponents", &Actor::getComponents)
		.addFunction("AddComponent", &Actor::AddComponent)
		.addFunction("RemoveComponent", &Actor::RemoveComponent)
		.endClass()
		.beginNamespace("Actor")
		.addFunction("Find", Scene::Find)
		.addFunction("FindAll", Scene::FindAll)
		.addFunction("Instantiate", Scene::Instantiate)
		.addFunction("Destroy", Scene::Destroy)
		.endNamespace()
		.beginNamespace("Application")
		.addFunction("Quit", ComponentDB::Quit)
		.addFunction("Sleep", ComponentDB::Sleep)
		.addFunction("GetFrame", ComponentDB::GetFrame)
		.addFunction("OpenURL", ComponentDB::OpenURL)
		.endNamespace()
		.beginClass<glm::vec2>("vec2")
		.addProperty("x", &glm::vec2::x)
		.addProperty("y", &glm::vec2::y)
		.endClass()
		.beginNamespace("Input")
		.addFunction("GetKey", Input::GetKeyS)
		.addFunction("GetKeyDown", Input::GetKeyDownS)
		.addFunction("GetKeyUp", Input::GetKeyUpS)
		.addFunction("GetMousePosition", Input::GetMousePosition)
		.addFunction("GetMouseButton", Input::GetMouseButton)
		.addFunction("GetMouseButtonDown", Input::GetMouseButtonDown)
		.addFunction("GetMouseButtonUp", Input::GetMouseButtonUp)
		.addFunction("GetMouseScrollDelta", Input::GetMouseScrollDelta)
		.addFunction("HideCursor", Input::HideCursor)
		.addFunction("ShowCursor", Input::ShowCursor)
		.addFunction("GetNumControllers", Input::GetNumControllers)
		.addFunction("GetControllerButton", Input::GetControllerButtonS)
		.addFunction("GetControllerButtonDown", Input::GetControllerButtonDownS)
		.addFunction("GetControllerButtonUp", Input::GetControllerButtonUpS)
		.addFunction("GetControllerAxis", Input::GetControllerAxisS)
		.addFunction("ControllerConnected", Input::ControllerConnected)
		.endNamespace()
		.beginNamespace("Text")
		.addFunction("Draw", Renderer::TextDraw)
		.endNamespace()
		.beginNamespace("Audio")
		.addFunction("Play", AudioManager::playSound)
		.addFunction("Halt", AudioManager::stopChannel)
		.addFunction("SetVolume", AudioManager::setVolume)
		.endNamespace()
		.beginNamespace("Image")
		.addFunction("DrawUI", Renderer::UIDraw)
		.addFunction("DrawUIEx", Renderer::UIDrawEX)
		.addFunction("Draw", Renderer::ImageDraw)
		.addFunction("DrawEx", Renderer::ImageDrawEX)
		.addFunction("DrawPixel", Renderer::PixelDraw)
		.endNamespace()
		.beginNamespace("Camera")
		.addFunction("SetPosition", Renderer::CamSetPos)
		.addFunction("GetPositionX", Renderer::CamGetPosX)
		.addFunction("GetPositionY", Renderer::CamGetPosY)
		.addFunction("SetZoom", Renderer::CamSetZoom)
		.addFunction("GetZoom", Renderer::CamGetZoom)
		.endNamespace()
		.beginNamespace("Scene")
		.addFunction("Load", Engine::QueueScene)
		.addFunction("GetCurrent", Engine::GetScene)
		.addFunction("DontDestroy", Engine::DontDestroy)
		.endNamespace()
		.beginClass<b2Vec2>("Vector2")
		.addConstructor<void(*)(float, float)>()
		.addFunction("Normalize", &b2Vec2::Normalize)
		.addFunction("Length", &b2Vec2::Length)
		.addProperty("x", &b2Vec2::x)
		.addProperty("y", &b2Vec2::y)
		.addFunction("__add", &b2Vec2::operator_add)
		.addFunction("__sub", &b2Vec2::operator_sub)
		.addFunction("__mul", &b2Vec2::operator_mul)
		.addStaticFunction("Distance", &b2Distance)
		.addStaticFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
		.endClass()
		.beginClass<Rigidbody>("Rigidbody")
		.addData("enabled", &Rigidbody::enabled)
		.addData("key", &Rigidbody::key)
		.addData("type", &Rigidbody::type)
		.addData("actor", &Rigidbody::actor)
		.addData("x", &Rigidbody::x)
		.addData("y", &Rigidbody::y)
		.addData("body_type", &Rigidbody::body_type)
		.addData("precise", &Rigidbody::precise)
		.addData("gravity_scale", &Rigidbody::gravity_scale)
		.addData("density", &Rigidbody::density)
		.addData("angular_friction", &Rigidbody::angular_friction)
		.addData("rotation", &Rigidbody::rotation)
		.addData("has_collider", &Rigidbody::has_collider)
		.addData("collider_type", &Rigidbody::collider_type)
		.addData("width", &Rigidbody::width)
		.addData("height", &Rigidbody::height)
		.addData("radius", &Rigidbody::radius)
		.addData("friction", &Rigidbody::friction)
		.addData("bounciness", &Rigidbody::bounciness)
		.addData("has_trigger", &Rigidbody::has_trigger)
		.addData("trigger_type", &Rigidbody::trigger_type)
		.addData("trigger_width", &Rigidbody::trigger_width)
		.addData("trigger_height", &Rigidbody::trigger_height)
		.addData("trigger_radius", &Rigidbody::trigger_radius)
		.addFunction("GetPosition", &Rigidbody::GetPosition)
		.addFunction("GetRotation", &Rigidbody::GetRotation)
		.addFunction("Ready", &Rigidbody::Ready)
		.addFunction("AddForce", &Rigidbody::AddForce)
		.addFunction("SetVelocity", &Rigidbody::SetVelocity)
		.addFunction("SetPosition", &Rigidbody::SetPosition)
		.addFunction("SetRotation", &Rigidbody::SetRotation)
		.addFunction("SetAngularVelocity", &Rigidbody::SetAngularVelocity)
		.addFunction("SetGravityScale", &Rigidbody::SetGravityScale)
		.addFunction("SetUpDirection", &Rigidbody::SetUpDirection)
		.addFunction("SetRightDirection", &Rigidbody::SetRightDirection)
		.addFunction("GetVelocity", &Rigidbody::GetVelocity)
		.addFunction("GetAngularVelocity", &Rigidbody::GetAngularVelocity)
		.addFunction("GetGravityScale", &Rigidbody::GetGravityScale)
		.addFunction("GetUpDirection", &Rigidbody::GetUpDirection)
		.addFunction("GetRightDirection", &Rigidbody::GetRightDirection)
		.endClass()
		.beginClass<Collision>("Collision")
		.addProperty("other", &Collision::other)
		.addProperty("point", &Collision::point)
		.addProperty("relative_velocity", &Collision::relative_velocity)
		.addProperty("normal", &Collision::normal)
		.endClass()
		.beginClass<HitResult>("HitResult")
		.addProperty("actor", &HitResult::actor)
		.addProperty("is_trigger", &HitResult::is_trigger)
		.addProperty("normal", &HitResult::normal)
		.addProperty("point", &HitResult::point)
		.endClass()
		.beginNamespace("Physics")
		.addFunction("Raycast", Engine::ActorRaycast)
		.addFunction("RaycastAll", Engine::ActorRaycastAll)
		.endNamespace()
		.beginNamespace("Event")
		.addFunction("Publish", EventBus::Publish)
		.addFunction("Subscribe", EventBus::Subscribe)
		.addFunction("Unsubscribe", EventBus::Unsubscribe)
		.endNamespace()
		.beginClass<ParticleSystem>("ParticleSystem")
		.addData("enabled", &ParticleSystem::enabled)
		.addData("key", &ParticleSystem::key)
		.addData("type", &ParticleSystem::type)
		.addData("actor", &ParticleSystem::actor)
		.addData("x", &ParticleSystem::x)
		.addData("y", &ParticleSystem::y)
		.addData("radius", &ParticleSystem::radius)
		.addData("image", &ParticleSystem::image)
		.addData("frames_between_bursts", &ParticleSystem::frames_between_bursts)
		.addData("burst_quantity", &ParticleSystem::burst_quantity)
		.addData("start_scale_min", &ParticleSystem::start_scale_min)
		.addData("start_scale_max", &ParticleSystem::start_scale_max)
		.addData("rotation_min", &ParticleSystem::rotation_min)
		.addData("rotation_max", &ParticleSystem::rotation_max)
		.addData("start_color_r", &ParticleSystem::start_color_r)
		.addData("start_color_g", &ParticleSystem::start_color_g)
		.addData("start_color_b", &ParticleSystem::start_color_b)
		.addData("start_color_a", &ParticleSystem::start_color_a)
		.addData("emit_angle_min", &ParticleSystem::emit_angle_min)
		.addData("emit_angle_max", &ParticleSystem::emit_angle_max)
		.addData("emit_radius_min", &ParticleSystem::emit_radius_min)
		.addData("emit_radius_max", &ParticleSystem::emit_radius_max)
		.addData("sorting_order", &ParticleSystem::sorting_order)
		.addData("duration_frames", &ParticleSystem::duration_frames)
		.addData("start_speed_min", &ParticleSystem::start_speed_min)
		.addData("start_speed_max", &ParticleSystem::start_speed_max)
		.addData("rotation_speed_min", &ParticleSystem::rotation_speed_min)
		.addData("rotation_speed_max", &ParticleSystem::rotation_speed_max)
		.addData("gravity_scale_x", &ParticleSystem::gravity_scale_x)
		.addData("gravity_scale_y", &ParticleSystem::gravity_scale_y)
		.addData("drag_factor", &ParticleSystem::drag_factor)
		.addData("angular_drag_factor", &ParticleSystem::angular_drag_factor)
		.addData("end_scale", &ParticleSystem::end_scale)
		.addData("end_color_r", &ParticleSystem::end_color_r)
		.addData("end_color_g", &ParticleSystem::end_color_g)
		.addData("end_color_b", &ParticleSystem::end_color_b)
		.addData("end_color_a", &ParticleSystem::end_color_a)
		.addFunction("OnStart", &ParticleSystem::OnStart)
		.addFunction("OnUpdate", &ParticleSystem::OnUpdate)
		.addFunction("Stop", &ParticleSystem::Stop)
		.addFunction("Play", &ParticleSystem::Play)
		.addFunction("Burst", &ParticleSystem::Burst)
		.endClass();




	std::string start_message, bad_message, good_message, initial_scene = "";
	std::string title = "";
	rapidjson::Document init_scene;
	if (gameconfig.HasMember("game_title")) title = gameconfig["game_title"].GetString();
	if (gameconfig.HasMember("game_start_message")) start_message = gameconfig["game_start_message"].GetString();
	if(gameconfig.HasMember("game_over_bad_message")) bad_message = gameconfig["game_over_bad_message"].GetString();
	if(gameconfig.HasMember("game_over_good_message")) good_message = gameconfig["game_over_good_message"].GetString();
	if (gameconfig.HasMember("initial_scene")) {
		initial_scene = gameconfig["initial_scene"].GetString();
	}
	else {
		std::cout << "error: initial_scene unspecified";
		exit(0);
	}
	if (std::filesystem::exists("resources/scenes/" + initial_scene + ".scene")) {
		//std::cout << "here" << std::endl;
		EngineUtils::ReadJsonFile("resources/scenes/" + initial_scene + ".scene", init_scene);
	}
	else {
		std::cout << "error: scene " + initial_scene + " is missing";
		exit(0);
	}
	Engine::current_scene_name = initial_scene;

	//std::cout << "here" << std::endl;

	Scene first_scene;
	auto actor_list = init_scene["actors"].GetArray();
	first_scene.uuid = 0;
	first_scene.actors.reserve(actor_list.Size());

	std::vector<Component> masterComponentList;

	for (const auto& value : actor_list) {
		Actor* temp = new Actor();
        rapidjson::Document templateDoc;
		if (value.HasMember("template")) {
			std::string templateString = value["template"].GetString();
			if(Scene::validTemplates.find(templateString) == Scene::validTemplates.end())
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
		temp->actorID = first_scene.uuid;
		
		// LUA COMPONENT STUFF HERE ON DOWN ---------------------------------------------------------------------------------------------------------------------------------------

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
					else if(component_type == "Rigidbody") { // Rigidbody component
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
					else { // particle system
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
					if(component_type != "Rigidbody" && component_type != "ParticleSystem"){ // Standard component
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
						if(instance_table.isTable()) {
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
					else if(component_type == "Rigidbody") { // Rigidbody component
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

		
		first_scene.uuid++;
		first_scene.actors.push_back(temp);
		Scene::staticActorList.push_back(temp);
	}

	// Other stuff i guess

	int x_res = 13;
	int y_res = 9;
	int window_x = 640;
	int window_y = 360;
	int clear_r = 255;
	int clear_g = 255;
	int clear_b = 255;
	float camera_zoom = 1.0f;
    glm::vec2 cam_offset = glm::vec2(0,0);
	if (std::filesystem::exists("resources/rendering.config")) {
		EngineUtils::ReadJsonFile("resources/rendering.config", renderingconfig);
		if (renderingconfig.HasMember("x_resolution")) x_res = renderingconfig["x_resolution"].GetInt();
		if (renderingconfig.HasMember("y_resolution")) y_res = renderingconfig["y_resolution"].GetInt();
		if (renderingconfig.HasMember("x_resolution")) window_x = renderingconfig["x_resolution"].GetInt();
		if (renderingconfig.HasMember("y_resolution")) window_y = renderingconfig["y_resolution"].GetInt();
		if (renderingconfig.HasMember("clear_color_r")) clear_r = renderingconfig["clear_color_r"].GetInt();
		if (renderingconfig.HasMember("clear_color_g")) clear_g = renderingconfig["clear_color_g"].GetInt();
		if (renderingconfig.HasMember("clear_color_b")) clear_b = renderingconfig["clear_color_b"].GetInt();
        if (renderingconfig.HasMember("cam_offset_x")) {
            cam_offset.x = renderingconfig["cam_offset_x"].GetFloat();
        }
        if (renderingconfig.HasMember("cam_offset_y")) {
            cam_offset.y = renderingconfig["cam_offset_y"].GetFloat();
        }
		if (renderingconfig.HasMember("zoom_factor")) camera_zoom = renderingconfig["zoom_factor"].GetFloat();
	}

	std::vector<std::string> intro_img;
	if (gameconfig.HasMember("intro_image")) {
		for(int i = 0; i < gameconfig["intro_image"].GetArray().Size(); i++)
		intro_img.push_back(gameconfig["intro_image"].GetArray()[i].GetString());
		if (!std::filesystem::exists("resources/images/" + intro_img.back() + ".png")) {
			std::cout << "error: missing image " << intro_img.back();
			exit(0);
		}
	}
	std::vector<std::string> intro_txt;
	if (gameconfig.HasMember("intro_text")) {
		//std::cout << "here" << std::endl;
		for (int i = 0; i < gameconfig["intro_text"].GetArray().Size(); i++)
		{
			intro_txt.push_back(gameconfig["intro_text"].GetArray()[i].GetString());
			//std::cout << "here" << std::endl;
		}
	}
	std::string font_type = "";
	if (gameconfig.HasMember("font")) {
		font_type = gameconfig["font"].GetString();
		if (!std::filesystem::exists("resources/fonts/" + font_type + ".ttf")) {
			std::cout << "error: font " << font_type << " missing";
			exit(0);
		}
	}
	if (gameconfig.HasMember("intro_text") && !gameconfig.HasMember("font")) {
		std::cout << "error: text render failed. No font configured";
		exit(0);
	}
	float camera_ease_factor = 1.0f;
	if (renderingconfig.HasMember("cam_ease_factor")) camera_ease_factor = renderingconfig["cam_ease_factor"].GetFloat();
	bool move_flip = false;
	if (renderingconfig.HasMember("x_scale_actor_flipping_on_movement")) move_flip = renderingconfig["x_scale_actor_flipping_on_movement"].GetBool();

	Renderer rend = Renderer(window_x, window_y, clear_r, clear_g, clear_b, title, intro_img, intro_txt, font_type, glm::vec2(0, 0), cam_offset, camera_zoom, camera_ease_factor, move_flip);

	Engine engine(start_message, bad_message, good_message, x_res, y_res, first_scene, rend);
	engine.GameLoop();

	lua_close(ComponentDB::lua_state);
	return 0;
	
}
