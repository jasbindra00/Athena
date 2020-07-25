#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Utility.h"
#include "GUIEventData.h"
#include "StreamAttributes.h"
#include <type_traits>
#include "GameEventData.h"

class Window;
class Manager_GUI;

using GameEventData::StandardBinding;
using GameEventData::GUIBinding;

namespace BindingTypes {
	using StandardBindingPtr = std::unique_ptr<StandardBinding>;
	using GUIBindingPtr = std::unique_ptr<GUIBinding>;
	using BindingCallable = std::function<void(GameEventData::EventDetails*)>;
	template<typename T>
	using BindingData = std::vector<std::pair<std::string, T>>;
	using BindingCallables = BindingData <BindingCallable>;
	template<typename T, typename = typename std::enable_if_t<std::is_same_v<typename std::decay_t<T>, StandardBindingPtr> || std::is_same_v<typename std::decay_t<T>, GUIBindingPtr>>>
	using BindingObjects = BindingData<T>;
	using StandardBindings = BindingObjects<StandardBindingPtr>;
	using GUIBindings = BindingObjects<GUIBindingPtr>;
}



template<typename T, typename = typename std::enable_if_t < std::is_same_v<typename std::decay_t<T>, BindingTypes::BindingCallables> || std::is_same_v<typename std::decay_t<T>, BindingTypes::StandardBindings> || std::is_same_v<typename std::decay_t<T>, BindingTypes::GUIBindings>>>
using StateBindingData = std::unordered_map<GameStateType, T>;
class Manager_Event{
protected:
	using GUIStateBindings = StateBindingData<BindingTypes::GUIBindings>;
	using StandardStateBindings = StateBindingData<BindingTypes::StandardBindings>;
	using StateBindingCallables = StateBindingData<BindingTypes::BindingCallables>;
	
	StateBindingCallables statebindingcallables;
	StandardStateBindings statebindingobjects;
	GUIStateBindings guistatebindingobjects;

	mutable GameStateType activestate; //only the bindings for the active game state will be executed.
	Manager_GUI* guimgr;
	template<typename T>
	auto FindBinding(StateBindingData<T>& container, const GameStateType& state, const std::string& bindingname)->std::pair<bool, typename std::decay_t<T>::iterator> {
		auto& statebindings = container.at(state);
		auto foundbinding = std::find_if(statebindings.begin(), statebindings.end(), [bindingname](const auto& p) {
			return p.first == bindingname;
			});
		return (foundbinding == statebindings.end()) ? std::make_pair(false, foundbinding) : std::make_pair(true, foundbinding);
	}
	void ProcessGUIEvents();
public:
	Manager_Event(Manager_GUI* guimgr) noexcept;
	bool RegisterBindingCallable(const GameStateType& state, const std::string& bindingname, const BindingTypes::BindingCallable& action); //assigns the callable to the already existing state binding
	inline void SwitchToState(const GameStateType& state)const noexcept { activestate = state; } //the bindings of the active state are only processed
	template<typename T, typename  = typename std::enable_if_t<std::is_same_v<typename std::decay_t<T>, BindingTypes::GUIBindingPtr> || std::is_same_v<typename std::decay_t<T>, BindingTypes::StandardBindingPtr>>>
	auto RegisterBindingObject(const GameStateType& state, T& ptr) {
		auto bindingname = ptr->bindingname;
		auto intstate = Utility::ConvertToUnderlyingType(state);
		if constexpr (std::is_same_v<typename std::decay_t<T>, BindingTypes::GUIBindingPtr>) {
			auto guibindingexists = FindBinding(guistatebindingobjects, state, bindingname);
			if (guibindingexists.first) {
				LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to register GUIBinding object - binding of name " + bindingname + " already exists in state " + std::to_string(intstate));
				return false;
			}
			guistatebindingobjects[state].emplace_back(std::move(bindingname), std::move(ptr));
			return true;
		}
		auto standardbindingexists = FindBinding(statebindingobjects, state, bindingname);
		if (standardbindingexists.first) {
			LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to register StandardBinding object - binding of name " + bindingname + " already exists in state " + std::to_string(intstate));
			return false;
		}
		statebindingobjects[state].emplace_back(std::move(bindingname), std::move(ptr));
		return true;	
	}
	template<typename T>
	void RemoveBindingData(StateBindingData<T>& container, const GameStateType& state, const std::string& bindingname) {
		{auto foundbinding = FindBinding(container, state, bindingname);
		if (foundbinding.first) {
			container.erase(foundbinding);//raii
		}
		}
		auto foundcallable = FindBinding(statebindingcallables, state, bindingname);
		if (foundcallable.first) {
			statebindingcallables.erase(foundcallable);
		}
	}
	void HandleEvent(const GUIEventData::GUIEventInfo& evnt);
	void HandleEvent(const sf::Event& evnt, sf::RenderWindow* winptr); //handles all incoming events dispatched from window.
	void Update(sf::RenderWindow* winptr); //handles all live input events - keyboard and mouse.
	void LoadBindings(const std::string& filename); //automatic loading and assigning of each binding object to a given state from a txt file. binding callables must be registered seperately.
};




#endif