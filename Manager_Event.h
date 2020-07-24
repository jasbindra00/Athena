#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <functional>
#include "Utility.h"
#include <variant>
#include "GUIEvents.h"
#include "StreamAttributes.h"
#include <type_traits>

class Window;
enum class EventType {
	KEYPRESSED = sf::Event::EventType::KeyPressed,
	KEYRELEASED = sf::Event::EventType::KeyReleased,
	MOUSEPRESSED = sf::Event::EventType::MouseButtonPressed,
	MOUSERELEASED = sf::Event::EventType::MouseButtonReleased,
};
struct EventInfo {
	std::variant<int, GUIEvent> codeorguievent;
	EventInfo(const int& code)
		:codeorguievent(code) {
	}
};
struct EventDetails {
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
struct Binding {
	using BindingCondition = std::pair<EventType, EventInfo>;
	using BindingConditions = std::vector<BindingCondition>;
	Binding(std::string& bindingname, Attributes& attributes)
		:bindingname(bindingname) {
	}
	void AddCondition(const EventType& type, const int& keycode) {
		conditions.emplace_back(type, EventInfo(keycode));
	}
	int conditionsmet = 0; //once size := conditions, all conditions met, execute callable
	std::string bindingname;
	BindingConditions conditions; //event conditions necessary for the binding callable to be executed.
	EventDetails details;
	friend Attributes& operator>>(Attributes& stream, Binding& b) {

	}
	operator std::string() const {
		return std::string{};
	}
};


using BindingPtr = std::unique_ptr<Binding>;
using BindingCallable = std::function<void(EventDetails*)>;
template<typename T>
using BindingData = std::vector<std::pair<std::string, T>>;
using BindingCallables = BindingData <BindingCallable>;
using BindingObjects = BindingData<BindingPtr>;

template<typename T, typename = typename std::enable_if_t<std::is_same_v<std::decay_t<T>, BindingCallables> || std::is_same_v<std::decay_t<T>, BindingObjects>>>
using StateBindingData = std::unordered_map<GameStateType, T>;
using GUIStateBindingObjects = StateBindingData<BindingObjects>;

class Manager_Event{
protected:
	StateBindingData<BindingCallables> statebindingcallables;
	StateBindingData<BindingObjects> statebindingobjects;
	GUIStateBindingObjects guibindingobjects;
	mutable GameStateType activestate; //only the bindings for the active state will be executed.

	template<typename T>
	auto FindBinding(StateBindingData<T>& container, const GameStateType& state, const std::string& bindingname)->std::pair<bool, typename std::decay_t<T>::iterator> {
		auto& statebindings = container.at(state);
		auto foundbinding = std::find_if(statebindings.begin(), statebindings.end(), [bindingname](const auto& p) {
			return p.first == bindingname;
			});
		return (foundbinding == statebindings.end()) ? std::make_pair(false, foundbinding) : std::make_pair(true, foundbinding);
	}
public:
	Manager_Event() noexcept;
	inline void SwitchToState(const GameStateType& state)const noexcept { activestate = state; } //the bindings of the active state are only processed
	void RegisterBindingObject(const GameStateType& state, BindingPtr& bindingobject); //
	void RegisterBindingCallable(const GameStateType& state, const std::string& bindingname, BindingCallable action); //assigns the callable to the already existing state binding

	void RemoveBindingData(const GameStateType& state, const std::string& bindingname);

	void HandleEvent(const GUIEvent& evnt);
	void HandleEvent(const sf::Event& evnt, sf::RenderWindow* winptr); //handles all incoming events dispatched from window.
	void Update(sf::RenderWindow* winptr); //handles all live input events - keyboard and mouse.
	void LoadBindings(const std::string& filename); //automatic loading and assigning of each binding object to a given state from a txt file. binding callables must be registered seperately.
};




#endif