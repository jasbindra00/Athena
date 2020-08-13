#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Utility.h"
#include "StreamAttributes.h"
#include <type_traits>
#include <memory>
#include "EventData.h"
#include "GameStateData.h"

class Window;
class Manager_GUI;

using EventData::GameBinding;
using EventData::GUIBinding;
using EventData::EventDetails;
using EventData::Binding;
using GameStateData::GameStateType;

namespace BindingTypes {
	using BindingPtr = std::unique_ptr<Binding>;
	using BindingCallable = std::function<void(EventDetails*)>;
	using BindingCallablePtr = std::unique_ptr<BindingCallable>;

	template<typename T>
	using BindingData = std::vector<std::pair<std::string, T>>;
}
using BindingTypes::BindingData;
using BindingTypes::BindingCallable;

template<typename T>
using ENABLE_IF_CONTAINED = std::enable_if_t<std::is_same_v<typename std::decay_t<T>, Binding> || std::is_same_v<typename std::decay_t<T>, BindingCallable>>;
template<typename T>
using DEDUCE_CONTAINER_TYPE = std::conditional_t<std::is_same_v<typename std::decay_t<T>, Binding>, BindingData<BindingTypes::BindingPtr>, BindingData<BindingTypes::BindingCallable>>;
template<typename T>
using DEDUCE_ARG_TYPE = std::conditional_t<std::is_same_v<typename std::decay_t<T>, Binding>, Attributes*, BindingCallable>;

template<typename T>
using StateBindingData = std::unordered_map<GameStateType, BindingTypes::BindingData<T>>;


class Manager_Event{
protected:
	StateBindingData<BindingTypes::BindingCallable> bindingcallables;
	StateBindingData<BindingTypes::BindingPtr> statebindingobjects;
	mutable GameStateType activestate; //only the bindings for the active game state will be executed.
	Manager_GUI* guimgr;
	template<typename T, typename = typename ENABLE_IF_CONTAINED<T>>
	DEDUCE_CONTAINER_TYPE<T>& GetBindingContainer(const GameStateType& state){
		if constexpr (std::is_same_v<typename std::decay_t<T>, Binding>) return statebindingobjects.at(state);
		else if constexpr(std::is_same_v<typename std::decay_t<T>, BindingCallable>) return bindingcallables.at(state);
	}
	template<typename T, typename = typename ENABLE_IF_CONTAINED<T>>
	auto FindBindingData(const GameStateType& state, const std::string& bindingname)->std::pair<bool, typename DEDUCE_CONTAINER_TYPE<T>::iterator> {
		auto& bindingcontainer = GetBindingContainer<T>(state);
		auto found = std::find_if(bindingcontainer.begin(), bindingcontainer.end(), [bindingname](const auto& p) {
			return p.first == bindingname;
			});
		return (found == bindingcontainer.end()) ? std::make_pair(false, found) : std::make_pair(true, found);
	}
	void ProcessGUIEvents();
	void HandleEvent(const EventData::GUIEventInfo& evnt);
public:
	Manager_Event(Manager_GUI* guimgr) noexcept;
	void LoadBindings(const std::string& filename); //automatic loading and assigning of each binding object to a given state from a txt file. binding callables must be registered seperately.
	bool RegisterBindingCallable(const GameStateType& state, const std::string& bindingname, const BindingTypes::BindingCallable& action); //assigns the callable to the already existing state binding
	template<typename T, typename = typename std::enable_if_t<std::is_same_v<typename std::decay_t<T>, GameBinding> || std::is_same_v<typename std::decay_t<T>, GUIBinding>>>
	bool RegisterBindingObject(const GameStateType& state, std::string& bindingname, const KeyProcessing::Keys& keys) {
		auto bindingexists = FindBindingData<Binding>(state, bindingname);
		if (bindingexists.first) {
			LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Binding of name " + bindingname + " within state " + std::to_string(Utility::ConvertToUnderlyingType(state)) + " already exists");
			return false;
		}
		auto& storage = statebindingobjects.at(state);
		auto bindingobj = std::make_unique<T>(bindingname);

		/**stream >> bindingobj.get();*/
		storage.emplace_back(bindingname, std::move(bindingobj));
		return true;




		//keys
		//or stream
	}
	template<typename T, typename = ENABLE_IF_CONTAINED<T>>
	bool RemoveBindingData(const GameStateType& state, const std::string& bindingname) {
		auto foundbinding = FindBindingData<T>(state, bindingname);
		if (!foundbinding.first) {
			LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Requested data of binding of name " + bindingname + " does not exist within state " + std::to_string(Utility::ConvertToUnderlyingType(state)));
			return false;
		}
		auto& container = GetBindingContainer<T>(state);
		container.erase(foundbinding);
		return true;
	}
	inline void SwitchToState(const GameStateType& state)const noexcept { activestate = state; } //the bindings of the active state are only processed	
	void HandleEvent(const sf::Event& evnt, sf::RenderWindow* winptr); //handles all incoming events dispatched from window.
	void Update(sf::RenderWindow* winptr); //handles all live input events - keyboard and mouse.
	
};




#endif