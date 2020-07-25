#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H
#define GENERIC_TEMPLATE template<typename T>
#include <vector>
#include <SFML\Window\Event.hpp>
#include <string>
#include <type_traits>
#include <variant>
#include "GUIEventData.h"
#include "StreamAttributes.h"
#include "KeyProcessing.h"


namespace GameEventData {
	enum class WindowEventType {
		KEYPRESSED = sf::Event::EventType::KeyPressed,
		KEYRELEASED = sf::Event::EventType::KeyReleased,
		MOUSEPRESSED = sf::Event::EventType::MouseButtonPressed,
		MOUSERELEASED = sf::Event::EventType::MouseButtonReleased,
		MOUSESCROLLED = sf::Event::EventType::MouseWheelScrolled
	};
	struct StandardEventInfo {
		int code;
	};
	struct EventDetails {
		EventDetails() {
		}
		void Reset() {
			mouseposition = { 0,0 };
			keycode = -1;
			mousecode = -1;
			mousewheeldelta = 0;
		}
		sf::Vector2i mouseposition{};
		int mousewheeldelta{ 0 };
		int keycode{ -1 };
		int mousecode{ -1 };
	};

	template<typename T1, typename = typename std::enable_if_t<std::is_same_v<typename std::decay_t<T1>, WindowEventType> || std::is_same_v<typename std::decay_t<T1>, GUIEventData::GUIEventType>>>
	struct Binding {
		using EventInformationType = std::conditional_t<std::is_same_v<typename std::decay_t<T1>, WindowEventType>, StandardEventInfo, GUIEventData::GUIEventInfo>;
		using BindingCondition = std::pair<T1, EventInformationType>; //if input is eventtype, use eventinfo. if input is guieventtype, use guievent
		std::vector<BindingCondition> conditions;
		EventDetails details;
		std::string bindingname;
		operator std::string() const {
			return std::string{};
		}
		Binding(const std::string& name) :bindingname(name) {
		}
		void AddCondition(const T1& evnttype, const EventInformationType& evntinfo) {
			conditions.emplace_back(evnttype, evntinfo);
		}
		virtual void ReadIn(Attributes& attributes) = 0;
		int conditionsmet{ 0 };
		friend Binding<T1>* operator>>(Attributes& stream, Binding<T1>* obj) {
			obj->ReadIn(stream);
			return obj;
		}
	};
	struct StandardBinding :public Binding<WindowEventType> {
		StandardBinding(const std::string& bindingname) :Binding<WindowEventType>(bindingname) {
		}
		void ReadIn(Attributes& attributes) override {
			while (!attributes.eof()) {
				auto key = KeyProcessing::KeyValsToString(attributes.GetWord()); //remove {,} from key and distill it into a string whose values are seperated by spaces
				unsigned int evnttype;
				StandardEventInfo evntinfo;
				{ 
					std::stringstream vals(key);
					vals >> evnttype >> evntinfo.code; 
				}
				AddCondition(static_cast<WindowEventType>(evnttype), std::move(evntinfo));
			}
		}
	};
	struct GUIBinding : public Binding<GUIEventData::GUIEventType> {
		GUIBinding(const std::string& bindingname) :Binding<GUIEventData::GUIEventType>(bindingname) {

		}
		void ReadIn(Attributes& attributes) override {

		}
	};

}
#endif