#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Utility.h"
#include <variant>
#include "GUIEvents.h"

struct GUIEvent;
extern enum class StateType;
enum class EventType{
	KEYPRESSED = sf::Event::EventType::KeyPressed,
	KEYRELEASED = sf::Event::EventType::KeyReleased,
	MOUSEPRESSED = sf::Event::EventType::MouseButtonPressed,
	MOUSERELEASED = sf::Event::EventType::MouseButtonReleased, 
};
struct EventInfo{
	std::variant<int, GUIEvent> codeorguievent;
	EventInfo(const int& code)
		:codeorguievent(code){
	}
};
struct EventDetails{
	EventDetails() {
	}
	void Reset() {
		mouseposition = {  };
		keycode = -1;
		mousecode = -1;
	}
	sf::Vector2i mouseposition{};
	int keycode{ -1 };
	int mousecode{ -1 };
};
using BindingCondition = std::pair<EventType, EventInfo>;
using BindingConditions = std::vector<BindingCondition>;
struct Binding{
	Binding(std::string& bindingname)
		:bindingname(bindingname) {
	}
	void AddCondition(const EventType& type, const int& keycode) {
		conditions.emplace_back(type, EventInfo(keycode));
	}
	int conditionsmet = 0; //once size := conditions, all conditions met, execute callable
	std::string bindingname;
	BindingConditions conditions; //event conditions necessary for the binding callable to be executed.
	EventDetails details;
};

using BindingPtr = std::unique_ptr<Binding>;
using BindingCallable = std::function<void(EventDetails*)>;

template<typename T>
using BindingData = std::unordered_map<std::string, T>;

using BindingCallables = BindingData < BindingCallable>;
using BindingObjects = BindingData<BindingPtr>;

template<typename T>
using StateBindingData = std::unordered_map<StateType, T>;

using StateBindingCallables = StateBindingData<BindingCallables>;
using StateBindingObjects = StateBindingData<BindingObjects>;



class Window;
class Manager_Event{
protected:
	mutable StateType activestate; //only the bindings for the active state will be executed.
	StateBindingCallables statebindingcallables; //seperately stored callables and objects to allow flexibility in changing binding callables.
	StateBindingObjects statebindingobjects;
	template<typename T, typename = typename std::enable_if_t<std::is_same_v<std::decay_t<T>, BindingCallables> || std::is_same_v<std::decay_t<T>, BindingObjects>>>
	auto FindBinding(StateBindingData<T>& container, const StateType& state, const std::string& bindingname)->typename std::decay_t<T>::iterator{
		return container[state].find(bindingname);
	}
public:
	Manager_Event() noexcept;
	inline void SwitchToState(const StateType& state)const noexcept { activestate = state; } //the bindings of the active state are only processed
	void RegisterBindingObject(const StateType& state, std::unique_ptr<Binding> bindingobject); //
	void RegisterBindingCallable(const StateType& state, const std::string& bindingname, BindingCallable action); //assigns the callable to the already existing state binding

	void RemoveBindingData(const StateType& state, const std::string& bindingname);

	void HandleEvent(const GUIEvent& evnt);
	void HandleEvent(const sf::Event& evnt, sf::RenderWindow* winptr); //handles all incoming events dispatched from window.
	void Update(sf::RenderWindow* winptr); //handles all live input events - keyboard and mouse.
	void LoadBindings(const std::string& filename); //automatic loading and assigning of each binding object to a given state from a txt file. binding callables must be registered seperately.
};




#endif