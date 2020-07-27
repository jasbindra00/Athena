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
#include "Log.h"
#include "GUIInfo.h"
#include "EnumConverter.h"

namespace GameEventData {
	enum class EventType {
		KEYPRESSED = sf::Event::EventType::KeyPressed,
		KEYRELEASED = sf::Event::EventType::KeyReleased,
		MOUSEPRESSED = sf::Event::EventType::MouseButtonPressed,
		MOUSERELEASED = sf::Event::EventType::MouseButtonReleased,
		MOUSESCROLLED = sf::Event::EventType::MouseWheelScrolled,
		GUI_HOVER,
		GUI_CLICK,
		GUI_RELEASE,
		GUI_FOCUS,
		NULLTYPE
	};
	EnumConverter<EventType> converter([](const std::string& str)->EventType {
		if (str == "KEYPRESSED") return EventType::KEYPRESSED;
		else if (str == "KEYRELEASED") return EventType::KEYRELEASED;
		else if (str == "MOUSEPRESSED") return EventType::MOUSEPRESSED;
		else if (str == "MOUSERELEASED") return EventType::MOUSERELEASED;
		else if (str == "MOUSESCROLLED") return EventType::MOUSESCROLLED;
		else if (str == "GUI_HOVER") return EventType::GUI_HOVER;
		else if (str == "GUI_CLICK") return EventType::GUI_CLICK;
		else if (str == "GUI_RELEASE") return EventType::GUI_RELEASE;
		else if (str == "GUI_FOCUS") return EventType::GUI_FOCUS;
		return EventType::NULLTYPE;
		});

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





	struct Binding {
		using EventInfo = std::variant<int, GUIEventInfo>;
		using BindingCondition = std::pair<EventType, EventInfo>; //if input is eventtype, use eventinfo. if input is guieventtype, use guievent
		std::vector<BindingCondition> conditions;
		EventDetails details;
		std::string bindingname;
		operator std::string() const {
			return std::string{};
		}
		Binding(const std::string& name) :bindingname(name) {
		}
		void AddCondition(const EventType& evnttype, const EventInformationType& evntinfo) {
			conditions.emplace_back(evnttype, evntinfo);
		}
		virtual void ReadIn(Attributes& attributes) = 0;
		int conditionsmet{ 0 };
		friend Binding* operator>>(Attributes& stream, Binding* obj) {
			obj->ReadIn(stream);
			return obj;
		}
	};

// 	template<typename T1, typename = typename std::enable_if_t<std::is_same_v<typename std::decay_t<T1>, WindowEventType> || std::is_same_v<typename std::decay_t<T1>, GUIEventData::GUIEventType>>>
// 	struct Binding {//abstract base class for 
// 		using EventInformationType = std::conditional_t<std::is_same_v<typename std::decay_t<T1>, WindowEventType>, StandardEventInfo, GUIEventData::GUIEventInfo>;
// 		using BindingCondition = std::pair<T1, EventInformationType>; //if input is eventtype, use eventinfo. if input is guieventtype, use guievent
// 		std::vector<BindingCondition> conditions;
// 		EventDetails details;
// 		std::string bindingname;
// 		operator std::string() const {
// 			return std::string{};
// 		}
// 		Binding(const std::string& name) :bindingname(name) {
// 		}
// 		void AddCondition(const T1& evnttype, const EventInformationType& evntinfo) {
// 			conditions.emplace_back(evnttype, evntinfo);
// 		}
// 		virtual void ReadIn(Attributes& attributes) = 0;
// 		int conditionsmet{ 0 };
// 		friend Binding<T1>* operator>>(Attributes& stream, Binding<T1>* obj) {
// 			obj->ReadIn(stream);
// 			return obj;
// 		}
// 	};
	struct StandardBinding :public Binding<WindowEventType> {
		StandardBinding(const std::string& bindingname) :Binding<WindowEventType>(bindingname) {
		}
		void ReadIn(Attributes& stream) override {
			while (!stream.eof()) {
				auto keyattributes = KeyProcessing::ExtractAttributesToStream(stream.GetWord(), false);
				if (keyattributes.PeekWord().empty()) continue;
				unsigned int evnttype;
				StandardEventInfo evntinfo;
				keyattributes >> evnttype >> evntinfo.code;
				AddCondition(static_cast<WindowEventType>(evnttype), std::move(evntinfo));
			}
		}
	};
	struct GUIBinding : public Binding<GUIEventData::GUIEventType> {
		GUIBinding(const std::string& bindingname) :Binding<GUIEventData::GUIEventType>(bindingname) {
		}
		void ReadIn(Attributes& attributes) override {
			std::string hierarchy;
			GUIEventData::GUIEventType evnttype = GUIEventData::GUIEventType::NULLTYPE;
			GUIData::GUIStateData::GUIState guistate = GUIData::GUIStateData::GUIState::NULLSTATE;
			while (!attributes.eof()) {
				auto keyattributes = KeyProcessing::ExtractAttributesToStream(attributes.GetWord(), true);
				auto attributetype = KeyProcessing::ToUpperString(keyattributes.GetWord());
				auto attribute = keyattributes.GetWord();
				if (attributetype == "GUIEVENTTYPE") evnttype = GUIEventData::converter(KeyProcessing::ToUpperString(attribute));
				else if (attributetype == "GUISTATE") guistate = GUIData::GUIStateData::converter(KeyProcessing::ToUpperString(attribute));
				else if (attributetype == "HIERARCHY") {
					hierarchy += std::move(attribute);
					hierarchy += ' ';
				}
				else LOG::Log(LOCATION::GUIBINDING, LOGTYPE::ERROR, __FUNCTION__, "Unable to recognise the binding type '" + attributetype + "' for binding of name " + bindingname);
			}
			if (evnttype == GUIEventData::GUIEventType::NULLTYPE) { LOG::Log(LOCATION::GUIBINDING, LOGTYPE::ERROR, __FUNCTION__, "GUIEventType for binding of name " + bindingname + " was NULLTYPE");return; }
			else if (guistate == GUIData::GUIStateData::GUIState::NULLSTATE) { LOG::Log(LOCATION::GUIBINDING, LOGTYPE::ERROR, __FUNCTION__, "GUIState for binding of name " + bindingname + " was NULLSTATE");return;}
			//construct a guievnt.
			GUIEventData::GUIEventInfo evnt;
			evnt.eventtype = std::move(evnttype);
			evnt.interfacehierarchy = std::move(hierarchy);
			evnt.elementstate = std::move(guistate);
			AddCondition(evnttype, std::move(evnt));
		}
	};

}
#endif