#pragma once
#include <string>
#include <glm/glm.hpp>
#include "rapidjson/document.h"
#include "Actor.h"
#include "Scene.h"
#include "unordered_map"
#include "Renderer.h"
#include "AudioManager.h"
#include "box2d/box2d.h"

class RaycastDetection : public b2RayCastCallback {
	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction);
};

struct HitResult {
	Actor* actor;
	b2Vec2 point;
	b2Vec2 normal;
	bool is_trigger;
};

class Engine
{
public:
	void GameLoop();
	void Input();
	void Update();
	void Render(bool initial = false);
	void loadScene(std::string scene_to_load);
	Engine(std::string start_message, std::string bad_message, std::string good_message, int x_res, int y_res, Scene& first_scene, Renderer rend);

	static void QueueScene(std::string scene);
	static std::string GetScene();
	static void DontDestroy(Actor* a);
	static inline std::string current_scene_name;

	static inline b2World* world;
	static inline bool worldExists = false;
	static void createWorld();
	static inline CollisionDetection collision_detector;

	static luabridge::LuaRef ActorRaycast(b2Vec2 pos, b2Vec2 dir, float dist);
	static luabridge::LuaRef ActorRaycastAll(b2Vec2 pos, b2Vec2 dir, float dist);
	static inline RaycastDetection* raycast_detector;
	static inline b2Vec2 raycast_origin;
	static inline std::optional<HitResult> closest_raycast_hit;
	static inline std::vector<HitResult> all_raycast_hits;

private:
	bool win = false;
	bool lose = false;
	bool manual_exit = false;
	bool victory = false;
	bool running = true;
	std::vector<int> contacted;
	std::string user_input = "";
	glm::ivec2 player_position = glm::ivec2(0,0);
	int health = 3;
	int score = 0;
	std::string game_start_message;
	std::string game_over_bad_message;
	std::string game_over_good_message;
	int x_resolution = 13;
	int y_resolution = 9;
	Scene currentScene;
	static inline bool proceed_to_next_scene = false;
	static inline std::string next_scene_name;
	Renderer visualRenderer;
	int cooldown = 0;
};