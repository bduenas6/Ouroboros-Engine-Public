#include "EventBus.h"

void EventBus::Publish(std::string event_type, luabridge::LuaRef event_object)
{
	for (auto& sub : current_subscribers[event_type]) {
		luabridge::LuaRef func = sub.second;
		func(sub.first, event_object);
	}
}

void EventBus::Subscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function)
{
	new_subscribers.push_back(std::tuple<std::string, luabridge::LuaRef, luabridge::LuaRef>(event_type, component, function));
	//new_subscribers.push_back(std::pair<std::string, std::pair<luabridge::LuaRef, luabridge::LuaRef>>(event_type, (component, function)));
}

void EventBus::Unsubscribe(std::string event_type, luabridge::LuaRef component, luabridge::LuaRef function)
{
	new_unsubscribers.push_back(std::tuple<std::string, luabridge::LuaRef, luabridge::LuaRef>(event_type, component, function));
	//new_unsubscribers.push_back(std::pair<std::string, std::pair< luabridge::LuaRef, luabridge::LuaRef>>(event_type, (component, function)));
}
 
void EventBus::manageSubscribers()
{
	for (auto sub : new_subscribers) {
		current_subscribers[std::get<0>(sub)].push_back(std::pair<luabridge::LuaRef, luabridge::LuaRef>(std::get<1>(sub), std::get<2>(sub)));
		//current_subscribers[sub.first].push_back(std::pair<luabridge::LuaRef, luabridge::LuaRef>(sub.second.first, sub.second.second));
	}
	new_subscribers.clear();
	for (auto sub : new_unsubscribers) {
		// abomination formed from an addled mind
		current_subscribers[std::get<0>(sub)].erase(std::find(current_subscribers[std::get<0>(sub)].begin(), current_subscribers[std::get<0>(sub)].end(), std::pair<luabridge::LuaRef, luabridge::LuaRef>(std::get<1>(sub), std::get<2>(sub))));
		
//        std::vector < std::pair < luabridge::LuaRef, luabridge::LuaRef>> temp;
//        temp = current_subscribers[sub.first];
//        std::pair<luabridge::LuaRef, luabridge::LuaRef> target = std::pair<luabridge::LuaRef, luabridge::LuaRef>(sub.second.first, sub.second.second);
////        std::cout << "here" << std::endl;
//        current_subscribers[sub.first].clear();
////        std::cout << "there" << std::endl;
//        for(auto pair : temp){
//            if(pair != target){
//                current_subscribers[sub.first].push_back(pair);
//            }
//        }
////		current_subscribers[sub.first].erase(std::find(current_subscribers[sub.first].begin(), current_subscribers[sub.first].end(), std::pair<luabridge::LuaRef, luabridge::LuaRef>(sub.second.first, sub.second.second)));
	}
	new_unsubscribers.clear();
}
