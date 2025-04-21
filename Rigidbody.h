#pragma once
#include <vector>
#include <string>
#include "box2d/box2d.h"
#include "Actor.h"
#include "Lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "Engine.h"
#include "box2d/box2d.h"

struct Rigidbody
{
	std::string type = "Rigidbody";
	std::string key = "???";
	Actor* actor = nullptr;
	bool enabled = true;

	float x = 0.0f;
	float y = 0.0f;
	std::string body_type = "dynamic";
	bool precise = true;
	float gravity_scale = 1.0f;
	float density = 1.0f;
	float angular_friction = 0.3f;
	float rotation = 0.0f;
	
	bool has_collider = true;
	std::string collider_type = "box";
	float width = 1.0f;
	float height = 1.0f;
	float radius = 0.5f;
	float friction = 0.3f;
	float bounciness = 0.3f;

	bool has_trigger = true;
	std::string trigger_type = "box";
	float trigger_width = 1.0f;
	float trigger_height = 1.0f;
	float trigger_radius = 0.5f;

	b2Vec2 GetPosition() {
		//std::cout << actor->actor_name << std::endl;
		if (body == nullptr) {
			return b2Vec2(x, y);
		}
		return body->GetPosition();
	};
	float GetRotation() {
		if (body == nullptr) {
			return rotation;
		}
		return body->GetAngle() * (180.0f / b2_pi);
	};

	b2Body* body = nullptr;

	void Ready() {
		if (!Engine::worldExists) {
			Engine::createWorld();
		}
		b2BodyDef body_def;
		if (body_type == "dynamic") body_def.type = b2_dynamicBody;
		else if (body_type == "kinematic") body_def.type = b2_kinematicBody;
		else if (body_type == "static") body_def.type = b2_staticBody;
		body_def.position = b2Vec2(x, y);
		body_def.bullet = precise;
		body_def.angularDamping = angular_friction;
		body_def.gravityScale = gravity_scale;
		body_def.angle = rotation * (b2_pi / 180.0f);

		body = Engine::world->CreateBody(&body_def);
		  

		if (has_collider) {
			b2FixtureDef my_fixture;
			my_fixture.density = density;
			my_fixture.isSensor = false;
			my_fixture.restitution = bounciness;
			my_fixture.friction = friction;
			my_fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);
			b2PolygonShape my_shape;
			b2CircleShape my_circle;
			if (collider_type == "box") {
				my_shape.SetAsBox(width * 0.5f, height * 0.5f);
				my_fixture.shape = &my_shape;
			}
			else if (collider_type == "circle") {
				my_circle.m_radius = radius;
				my_fixture.shape = &my_circle;
			}
			else {
				my_shape.SetAsBox(0.5f, 0.5f);
				my_fixture.shape = &my_shape;
			}
			body->CreateFixture(&my_fixture);
		}
		if (has_trigger) {
			b2FixtureDef my_fixture;
			my_fixture.density = density;
			my_fixture.isSensor = true;
			my_fixture.restitution = bounciness;
			my_fixture.friction = friction;
			my_fixture.userData.pointer = reinterpret_cast<uintptr_t>(actor);
			b2PolygonShape my_shape;
			b2CircleShape my_circle;
			if (trigger_type == "box") {
				my_shape.SetAsBox(trigger_width * 0.5f, trigger_height * 0.5f);
				my_fixture.shape = &my_shape;
			}
			else if (trigger_type == "circle") {
				my_circle.m_radius = trigger_radius;
				my_fixture.shape = &my_circle;
			}
			else {
				my_shape.SetAsBox(0.5f, 0.5f);
				my_fixture.shape = &my_shape;
			}
			body->CreateFixture(&my_fixture);
		}
		if (!has_collider && !has_trigger) {
			b2PolygonShape phantom_shape;
			phantom_shape.SetAsBox(width * 0.5f, height * 0.5f);
			b2FixtureDef phantom_fixture_def;
			phantom_fixture_def.shape = &phantom_shape;
			phantom_fixture_def.density = density;
			phantom_fixture_def.friction = -999.0f; //shouldnt matter?
			phantom_fixture_def.isSensor = true;
			phantom_fixture_def.userData.pointer = reinterpret_cast<uintptr_t>(actor);
			body->CreateFixture(&phantom_fixture_def);
		}

		
	};

	void AddForce(b2Vec2 vec2) {
		body->ApplyForceToCenter(vec2, true);
	};
	
	void SetVelocity(b2Vec2 vec2) {
		body->SetLinearVelocity(vec2);
	};

	void SetPosition(b2Vec2 vec2) {
		if (body == nullptr) {
			x = vec2.x;
			y = vec2.y;
			return;
		}

		float current_rot = body->GetTransform().q.GetAngle();
		body->SetTransform(vec2, current_rot);
	};
	
	void SetRotation(float degrees_cc) {
		if (body == nullptr) {
			rotation = degrees_cc;
			return;
		}

		b2Vec2 current_pos = body->GetTransform().p;
		body->SetTransform(current_pos, degrees_cc * (b2_pi / 180.0f));
	};

	void SetAngularVelocity(float degrees_cc) {
		body->SetAngularVelocity(degrees_cc * (b2_pi / 180.0f));
	};

	void SetGravityScale(float scale) {
		if (body == nullptr) {
			gravity_scale = scale;
			return;
		}

		body->SetGravityScale(scale);
	};

	void SetUpDirection(b2Vec2 direction) {
		direction.Normalize();
		float angle = (glm::atan(direction.x, -direction.y)) * (180.0f / b2_pi);
		SetRotation(angle);
	}

	void SetRightDirection(b2Vec2 direction) {
		direction.Normalize();
		float angle = (glm::atan(direction.x, -direction.y) - (b2_pi / 2.0f)) * (180.0f / b2_pi);
		SetRotation(angle);
	}

	b2Vec2 GetVelocity() {
		if (body == nullptr) {
			return b2Vec2(x, y);
		}

		return body->GetLinearVelocity();
	}

	float GetAngularVelocity() {
		return body->GetAngularVelocity() * (180.0f / b2_pi);
	}

	float GetGravityScale() {
		if (body == nullptr) {
			return gravity_scale;
		}
		return body->GetGravityScale();
	}

	b2Vec2 GetUpDirection() {
		float angle = body->GetAngle();
		b2Vec2 direction = b2Vec2(glm::sin(angle), -glm::cos(angle));
		direction.Normalize();
		return direction;
	}

	b2Vec2 GetRightDirection() {
		float angle = body->GetAngle();
		b2Vec2 direction = b2Vec2(glm::cos(angle), glm::sin(angle));
		direction.Normalize();
		return direction;
	}
};

