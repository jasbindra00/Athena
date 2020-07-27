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
#include <memory>
#include "GameEventData.h"

class Window;
class Manager_GUI;



using GameEventData::StandardBinding;
using GameEventData::GUIBinding;
using GameEventData::EventDetails;

namespace BindingTypes {
	using StandardBindingPtr = std::unique_ptr<StandardBinding>;
	using GUIBindingPtr = std::unique_ptr<GUIBinding>;
	using BindingCallable = std::function<void(EventDetails*)>;
	using BindingCallablePtr = std::unique_ptr<BindingCallable>;

	template<typename T>
	using BindingData = std::vector<std::pair<std::string, T>>;

}
using BindingTypes::BindingCallable;
using BindingTypes::BindingData;

template<typename T>
using IS_STANDARD_BINDING = std::is_same<std::decay_t<T>, StandardBinding>;
template<typename T>
using IS_GUI_BINDING = std::is_same<std::decay_t<T>, GUIBinding>;
template<typename T>
using IS_BINDING_CALLABLE = std::is_same<std::decay_t<T>, BindingCallable>;
template<typename T>
using ENABLE_IF_BINDING = std::enable_if_t<IS_STANDARD_BINDING<T>::value || IS_GUI_BINDING<T>::value>;
template<typename T>
using ENABLE_IF_VALID_TYPE = std::enable_if_t<IS_STANDARD_BINDING<T>::value || IS_GUI_BINDING<T>::value|| IS_BINDING_CALLABLE<T>::value>;

template<typename T>
using StateBindingData = std::unordered_map<GameStateType, BindingData<T>>;



class Manager_Event{
private:
	template<typename T, typename = typename ENABLE_IF_VALID_TYPE<T>>
	BindingTypes::BindingData<std::unique_ptr<T>>& GetBindingStorage(const GameStateType& state) {
		if constexpr (IS_GUI_BINDING<T>::value) return guistatebindingobjects.at(state);
		else if constexpr (IS_STANDARD_BINDING<T>::value) return statebindingobjects.at(state);
		else if constexpr (IS_BINDING_CALLABLE<T>::value) return bindingcallables.at(state);
	}
protected:
	StateBindingData<BindingTypes::BindingCallablePtr> bindingcallables;
	StateBindingData<BindingTypes::GUIBindingPtr> guistatebindingobjects;
	StateBindingData<BindingTypes::StandardBindingPtr> statebindingobjects;

	mutable GameStateType activestate; //only the bindings for the active game state will be executed.
	Manager_GUI* guimgr;
	template<typename T, typename = typename ENABLE_IF_VALID_TYPE<T>>
	auto FindBindingData(const GameStateType& state, const std::string& bindingname)->std::pair<bool, typename BindingTypes::BindingData<std::unique_ptr<T>>::iterator> {
		auto& b = GetBindingStorage<T>(state);
		auto data = std::find_if(b.begin(), b.end(), [bindingname](const auto& p) {
			return p.first == bindingname;
			});
		return (data == b.end()) ? std::make_pair(false, data) : std::make_pair(true, data);
	}
	void ProcessGUIEvents();
public:
	Manager_Event(Manager_GUI* guimgr) noexcept;
	bool RegisterBindingCallable(const GameStateType& state, const std::string& bindingname, const BindingTypes::BindingCallable& action); //assigns the callable to the already existing state binding
	inline void SwitchToState(const GameStateType& state)const noexcept { activestate = state; } //the bindings of the active state are only processed	
	template<typename T, typename  = typename ENABLE_IF_BINDING<T>>
	bool RegisterBindingObject(const GameStateType& state, std::string& bindingname, Attributes* stream) {
		auto bindingexists = FindBindingData<T>(state, bindingname);
		if (bindingexists.first) {
			LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Binding of name " + bindingname + " within state " + std::to_string(Utility::ConvertToUnderlyingType(state)) +" already exists");
			return false;
		}
		auto& storage = GetBindingStorage<T>(state);
		auto bindingobj = std::make_unique<T>(bindingname);
		*stream >> bindingobj.get();
		storage.emplace_back(bindingname, std::move(bindingobj));
		return true;
	}
	template<typename T, typename = ENABLE_IF_VALID_TYPE<T>>
	bool RemoveBindingData( const GameStateType& state, const std::string& bindingname) {
		auto foundbinding = FindBindingData<T>(state, bindingname);
		if (!foundbinding.first) {
			LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Requested data of binding of name " + bindingname + " does not exist within state " + std::to_string(Utility::ConvertToUnderlyingType(state)));
			return false;
		}
		auto& container = GetBindingStorage<T>(state);
		container.erase(foundbinding);
		return true;
	}
	void HandleEvent(const GUIEventData::GUIEventInfo& evnt);
	void HandleEvent(const sf::Event& evnt, sf::RenderWindow* winptr); //handles all incoming events dispatched from window.
	void Update(sf::RenderWindow* winptr); //handles all live input events - keyboard and mouse.
	void LoadBindings(const std::string& filename); //automatic loading and assigning of each binding object to a given state from a txt file. binding callables must be registered seperately.
};




#endif