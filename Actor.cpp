#include "Actor.h"
#include "Rigidbody.h"

std::string Actor::getName()
{
    return actor_name;
}

int Actor::getID()
{
    return actorID;
}

void Actor::InjectConvenienceReferences(std::shared_ptr<luabridge::LuaRef> component_ref)
{
    (*component_ref)["actor"] = this;
}

luabridge::LuaRef Actor::getComponentByKey(std::string key)
{
    if(componentMap.find(key) != componentMap.end())
	{
		if (!componentMap[key]->isEnabled()) return luabridge::LuaRef(ComponentDB::lua_state);
		return *componentMap[key]->componentRef;
	}
    return luabridge::LuaRef(ComponentDB::lua_state);
}

luabridge::LuaRef Actor::getComponent(std::string type)
{
	//std::cout << "Attempting to grab component " << type << std::endl;
    for (auto c: componentMap)
        if(c.second->type == type) 
		{
			//std::cout << "Component found, checking if enabled..." << std::endl;
			if (!c.second->isEnabled()) return luabridge::LuaRef(ComponentDB::lua_state);
			//std::cout << "Component is enabled, returning reference" << std::endl;
			return *c.second->componentRef;
		}
    return luabridge::LuaRef(ComponentDB::lua_state);
}

luabridge::LuaRef Actor::getComponents(std::string type)
{
    luabridge::LuaRef table = luabridge::newTable(ComponentDB::lua_state);
    int index = 1;
    for (auto c : componentMap)
        if (c.second->type == type) 
		{
			if (!c.second->isEnabled()) continue;
			table[index++] = *c.second->componentRef;
		}
    return table;
}

luabridge::LuaRef Actor::AddComponent(std::string type_name)
{
	//std::cout << "Component: " << itr->name.GetString() << ", Type: " << itr->value["type"].GetString() << std::endl;
	if (type_name != "Rigidbody") { // Standard component
		std::string component_name = "r" + std::to_string(componentsAdded);
		std::string component_type = type_name;
		componentsAdded++;
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
		instance_table["actor"] = this;
		instance_table["enabled"] = true;
		Component* c = new Component;
		c->componentRef = std::make_shared<luabridge::LuaRef>(instance_table);
		c->type = component_type;
		if (instance_table.isTable()) {
			luabridge::LuaRef onStartFunc = instance_table["OnStart"];
			if (!onStartFunc.isNil()) {
				c->hasStart = true;
			}
			luabridge::LuaRef onUpdateFunc = instance_table["OnUpdate"];
			if (!onUpdateFunc.isNil()) {
				c->hasUpdate = true;
			}
			luabridge::LuaRef onLateUpdateFunc = instance_table["OnLateUpdate"];
			if (!onLateUpdateFunc.isNil()) {
				c->hasLateUpdate = true;
			}
		}
		else {
			std::cout << "conductor we have a problem";
			exit(0);
		}
		componentsToAdd.push_back(c);
		return instance_table;
	}
	else { // Rigidbody component
		Component* c = new Component;
		Rigidbody* rigidbody = new Rigidbody();
		luabridge::LuaRef ref(ComponentDB::lua_state, rigidbody);
		c->componentRef = std::make_shared<luabridge::LuaRef>(ref);

		c->type = "Rigidbody";
		c->hasLateUpdate = false;
		c->hasStart = true;
		c->hasUpdate = false;

		ref["actor"] = this;
		ref["enabled"] = true;
		componentsToAdd.push_back(c);
		
		return ref;
	}
}

void Actor::RemoveComponent(luabridge::LuaRef component_ref)
{
	for (auto c : componentMap) {
		if (*c.second->componentRef == component_ref) {
			(*c.second->componentRef)["enabled"] = false;
//            std::cout << c.first << std::endl;
		}
	}
}

void Actor::OnCollisionEnter(Collision col)
{
	for (auto c : componentMap) {
		//std::cout << "here" << std::endl;
		luabridge::LuaRef ref = *c.second->componentRef;
		if (ref["OnCollisionEnter"].isFunction()) {
			//std::cout << "there" << std::endl;
			ref["OnCollisionEnter"](ref, col);
		}
	}
}

void Actor::OnCollisionExit(Collision col)
{
	for (auto c : componentMap) {
		luabridge::LuaRef ref = *c.second->componentRef;
		if (ref["OnCollisionExit"].isFunction()) {
			ref["OnCollisionExit"](ref, col);
		}
	}
}

void Actor::OnTriggerEnter(Collision col)
{
	for (auto c : componentMap) {
		//std::cout << "here" << std::endl;
		luabridge::LuaRef ref = *c.second->componentRef;
		if (ref["OnTriggerEnter"].isFunction()) {
			//std::cout << "there" << std::endl;
			ref["OnTriggerEnter"](ref, col);
		}
	}
}

void Actor::OnTriggerExit(Collision col)
{
	for (auto c : componentMap) {
		luabridge::LuaRef ref = *c.second->componentRef;
		if (ref["OnTriggerExit"].isFunction()) {
			ref["OnTriggerExit"](ref, col);
		}
	}
}

void CollisionDetection::BeginContact(b2Contact* contact) {
	//std::cout << "here" << std::endl;
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	Actor* actorA = reinterpret_cast<Actor*>(fixtureA->GetUserData().pointer); //might break idk how special casting works
	Actor* actorB = reinterpret_cast<Actor*>(fixtureB->GetUserData().pointer);
	Collision collision;
	collision.other = actorB;
	b2WorldManifold world_manifold;
	contact->GetWorldManifold(&world_manifold);
	collision.normal = world_manifold.normal;
	collision.point = world_manifold.points[0]; // genuinely no clue
	collision.relative_velocity = fixtureA->GetBody()->GetLinearVelocity() - fixtureB->GetBody()->GetLinearVelocity();
	if (fixtureA->IsSensor() && fixtureB->IsSensor() && fixtureA->GetFriction() != -999.0f && fixtureB->GetFriction() != -999.0f) { //trigger
		collision.normal = b2Vec2(-999.0f, -999.0f);
		collision.point = b2Vec2(-999.0f, -999.0f);
		actorA->OnTriggerEnter(collision);
		collision.other = actorA;
		actorB->OnTriggerEnter(collision);
	}
	else if (!fixtureA->IsSensor() && !fixtureB->IsSensor()) { //collision
		actorA->OnCollisionEnter(collision);
		collision.other = actorA;
		actorB->OnCollisionEnter(collision);
	}
}

void CollisionDetection::EndContact(b2Contact* contact) {
	//std::cout << "there" << std::endl;
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	Actor* actorA = reinterpret_cast<Actor*>(fixtureA->GetUserData().pointer); //might break idk how special casting works
	Actor* actorB = reinterpret_cast<Actor*>(fixtureB->GetUserData().pointer);
	Collision collision;
	collision.other = actorB;
	b2WorldManifold world_manifold;
	contact->GetWorldManifold(&world_manifold);
	collision.normal = b2Vec2(-999.0f, -999.0f);
	collision.point = b2Vec2(-999.0f, -999.0f);
	collision.relative_velocity = fixtureA->GetBody()->GetLinearVelocity() - fixtureB->GetBody()->GetLinearVelocity();
	if (fixtureA->IsSensor() && fixtureB->IsSensor() && fixtureA->GetFriction() != -999.0f && fixtureB->GetFriction() != -999.0f) { //trigger
		collision.normal = b2Vec2(-999.0f, -999.0f);
		collision.point = b2Vec2(-999.0f, -999.0f);
		actorA->OnTriggerExit(collision);
		collision.other = actorA;
		actorB->OnTriggerExit(collision);
	}
	else if (!fixtureA->IsSensor() && !fixtureB->IsSensor()) { //collision
		actorA->OnCollisionExit(collision);
		collision.other = actorA;
		actorB->OnCollisionExit(collision);
	}
}
