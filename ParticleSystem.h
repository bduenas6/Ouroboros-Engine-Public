#pragma once
#include <vector>
#include <string>
#include "box2d/box2d.h"
#include "Actor.h"
#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "Engine.h"
#include "box2d/box2d.h"

struct ParticleSystem
{
	std::string type = "ParticleSystem";
	std::string key = "???";
	Actor* actor = nullptr;
	bool enabled = true;

	std::string image = "";
	float x = 0;
	float y = 0;
	float radius = 0.5f;
	int local_frame = 0;
	int frames_between_bursts = 1;
	int burst_quantity = 1;
	bool emitter_active = true;

	// Data Oriented Programming stuff
	int number_of_particle_slots = 0;
	int number_of_particles = 0;
	std::vector<float> particle_x;
	std::vector<float> particle_y;
	std::vector<float> particle_speed_x;
	std::vector<float> particle_speed_y;
	std::vector<float> particle_scale;
	std::vector<float> particle_initial_scale;
	std::vector<float> particle_rotation;
	std::vector<float> particle_speed_rotation;
	std::vector<float> particle_r;
	std::vector<float> particle_g;
	std::vector<float> particle_b;
	std::vector<float> particle_a;
	std::vector<bool> particle_active;
	std::vector<int> particle_frame;
	std::queue<int> free_list;

	// suite 0
	RandomEngine emit_angle_distribution;
	RandomEngine emit_radius_distribution;
	// suite 1
	RandomEngine rotation_distribution;
	RandomEngine scale_distribution;
	// suite 2
	RandomEngine speed_distribution;
	RandomEngine rotation_speed_distribution;
	//defaults
	float emit_angle_min = 0.0f;
	float emit_angle_max = 360.0f;
	float emit_radius_min = 0.0f;
	float emit_radius_max = 0.5f;
	float start_scale_min = 1.0f;
	float start_scale_max = 1.0f;
	float end_scale = -1.0f;
	float rotation_min = 0.0f;
	float rotation_max = 0.0f;
	float start_speed_min = 0.0f;
	float start_speed_max = 0.0f;
	float rotation_speed_min = 0.0f;
	float rotation_speed_max = 0.0f;
	float gravity_scale_x = 0.0f;
	float gravity_scale_y = 0.0f;
	float start_color_r = 255.0f; //assuming these can only be in the range
	float start_color_g = 255.0f;
	float start_color_b = 255.0f;
	float start_color_a = 255.0f;
	float end_color_r = -255.0f; //assuming these can only be in the range
	float end_color_g = -255.0f;
	float end_color_b = -255.0f;
	float end_color_a = -255.0f;
	int sorting_order = 9999;
	int duration_frames = 300;
	float drag_factor = 1.0f;
	float angular_drag_factor = 1.0f;

	void OnStart(luabridge::LuaRef ref) {
		if(image == "") Renderer::CreateDefaultParticleTextureWithName(image);
		local_frame = 0.0f;
		emit_angle_distribution = RandomEngine(emit_angle_min, emit_angle_max, 298);
		emit_radius_distribution = RandomEngine(emit_radius_min, emit_radius_max, 404);
		rotation_distribution = RandomEngine(rotation_min, rotation_max, 440);
		scale_distribution = RandomEngine(start_scale_min, start_scale_max, 494);
		speed_distribution = RandomEngine(start_speed_min, start_speed_max, 498);
		rotation_speed_distribution = RandomEngine(rotation_speed_min, rotation_speed_max, 305);
	}

	void OnUpdate(luabridge::LuaRef ref) {
		if (burst_quantity < 1) burst_quantity = 1;
		if (frames_between_bursts < 1) frames_between_bursts = 1;
		if (duration_frames < 1) duration_frames = 1;
		if (local_frame % frames_between_bursts == 0 && emitter_active) {
			GenerateNewParticles(burst_quantity);
		}
		for (int i = 0; i < number_of_particle_slots; i++) {
			if (particle_active[i] == false)
				continue;
			int frames_alive = local_frame - particle_frame[i];
			if (frames_alive >= duration_frames) {
				particle_active[i] = false;
				free_list.push(i);
				number_of_particles--;
				continue;
			}
			particle_speed_x[i] += gravity_scale_x;
			particle_speed_y[i] += gravity_scale_y;
			particle_speed_x[i] *= drag_factor;
			particle_speed_y[i] *= drag_factor;
			particle_speed_rotation[i] *= angular_drag_factor;
			particle_x[i] += particle_speed_x[i];
			particle_y[i] += particle_speed_y[i];
			particle_rotation[i] += particle_speed_rotation[i];
			float lifetime_progress = static_cast<float>(frames_alive) / duration_frames;
			if (end_color_r != -255.0f) particle_r[i] = glm::mix(start_color_r, end_color_r, lifetime_progress);
			if (end_color_g != -255.0f) particle_g[i] = glm::mix(start_color_g, end_color_g, lifetime_progress);
			if (end_color_b != -255.0f) particle_b[i] = glm::mix(start_color_b, end_color_b, lifetime_progress);
			if (end_color_a != -255.0f) particle_a[i] = glm::mix(start_color_a, end_color_a, lifetime_progress);
			if (end_scale != -1.0f) particle_scale[i] = glm::mix(particle_initial_scale[i], end_scale, lifetime_progress);
			Renderer::ImageDrawEX(image, particle_x[i], particle_y[i], particle_rotation[i], particle_scale[i], particle_scale[i], 0.5f, 0.5f, particle_r[i], particle_g[i], particle_b[i], particle_a[i], sorting_order);
		}

		local_frame++;
	}

	void GenerateNewParticles(int burst_quantity) {
		for (int i = 0; i < burst_quantity; i++) {
			
			if (free_list.size() > 0) {
				int index = free_list.front();
				free_list.pop();
				float angle_radians = glm::radians(emit_angle_distribution.Sample());
				float radius = emit_radius_distribution.Sample();

				float cos_angle = glm::cos(angle_radians);
				float sin_angle = glm::sin(angle_radians);

				particle_x[index] = (x + cos_angle * radius);
				particle_y[index] = (y + sin_angle * radius);

				float speed = speed_distribution.Sample();
				particle_speed_x[index] = (cos_angle * speed);
				particle_speed_y[index] = (sin_angle * speed);

				particle_scale[index] = (scale_distribution.Sample());
				particle_initial_scale[index] = (particle_scale[index]);

				particle_rotation[index] = (rotation_distribution.Sample());
				particle_speed_rotation[index] = (rotation_speed_distribution.Sample());

				particle_r[index] = start_color_r;
				particle_g[index] = start_color_g;
				particle_b[index] = start_color_b;
				particle_a[index] = start_color_a;

				particle_active[index] = (true);
				particle_frame[index] = (local_frame);

				number_of_particles++;
			}
			else {
				float angle_radians = glm::radians(emit_angle_distribution.Sample());
				float radius = emit_radius_distribution.Sample();

				float cos_angle = glm::cos(angle_radians);
				float sin_angle = glm::sin(angle_radians);

				particle_x.push_back(x + cos_angle * radius);
				particle_y.push_back(y + sin_angle * radius);

				float speed = speed_distribution.Sample();
				particle_speed_x.push_back(cos_angle * speed);
				particle_speed_y.push_back(sin_angle * speed);

				particle_scale.push_back(scale_distribution.Sample());
				particle_initial_scale.push_back(particle_scale.back());

				particle_rotation.push_back(rotation_distribution.Sample());
				particle_speed_rotation.push_back(rotation_speed_distribution.Sample());

				particle_r.push_back(start_color_r);
				particle_g.push_back(start_color_g);
				particle_b.push_back(start_color_b);
				particle_a.push_back(start_color_a);

				particle_active.push_back(true);
				particle_frame.push_back(local_frame);

				number_of_particle_slots++;
				number_of_particles++;
			}
		}
	}

	void Stop() {
		emitter_active = false;
	}

	void Play() {
		emitter_active = true;
	}

	void Burst() {
		GenerateNewParticles(burst_quantity);
	}
};