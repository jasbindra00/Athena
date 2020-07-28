#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H
#include <vector>
#include <SFML\Window\Event.hpp>
#include <string>
#include <type_traits>
#include <variant>
#include "StreamAttributes.h"
#include "KeyProcessing.h"
#include "Log.h"
#include "GUIData.h"
#include "EnumConverter.h"



using GUIData::GUIStateData::GUIState;
using GUIData::GUITypeData::GUIType;





namespace EventData {
	enum class EventType {
		KEYPRESSED = sf::Event::EventType::KeyPressed,
		KEYRELEASED = sf::Event::EventType::KeyReleased,
		MOUSEPRESSED = sf::Event::EventType::MouseButtonPressed,
		MOUSERELEASED = sf::Event::EventType::MouseButtonReleased,
		MOUSESCROLLED = sf::Event::EventType::MouseWheelScrolled,
		MOUSEMOVED = sf::Event::EventType::MouseMoved,
		GUI_HOVER,
		GUI_CLICK,
		GUI_RELEASE,
		GUI_FOCUS,
		GUI_LEAVE,
		NULLTYPE
	};
	static EnumConverter<EventType> EventTypeConverter([](const std::string& str)->EventType {
		if (str == "KEYPRESSED") return EventType::KEYPRESSED;
		else if (str == "KEYRELEASED") return EventType::KEYRELEASED;
		else if (str == "MOUSEPRESSED") return EventType::MOUSEPRESSED;
		else if (str == "MOUSERELEASED") return EventType::MOUSERELEASED;
		else if (str == "MOUSESCROLLED") return EventType::MOUSESCROLLED;
		else if (str == "MOUSEMOVED") return EventType::MOUSEMOVED;
		else if (str == "GUI_HOVER") return EventType::GUI_HOVER;
		else if (str == "GUI_CLICK") return EventType::GUI_CLICK;
		else if (str == "GUI_RELEASE") return EventType::GUI_RELEASE;
		else if (str == "GUI_FOCUS") return EventType::GUI_FOCUS;
		else if (str == "GUI_LEAVE") return EventType::GUI_LEAVE;
		return EventType::NULLTYPE;
		});
	struct GUIEventInfo {
		std::string interfacehierarchy;
		GUIData::GUIStateData::GUIState elementstate;
		EventType guievnttype;
	};
	struct EventDetails {
		EventDetails() {
		}
		void Reset() {
			mouseposition = { 0,0 }; keycode = -1; mousecode = -1; mousewheeldelta = 0;
		}
		sf::Vector2i mouseposition{};
		int mousewheeldelta{ 0 };
		int keycode{ -1 };
		int mousecode{ -1 };
		GUIEventInfo guiinfo;
	};

	static enum class BINDINGTYPE {
		GAME, GUI
	};
	using EventInfo = std::variant<int, GUIEventInfo>;
	struct Binding {

		std::vector<std::pair<EventType, EventInfo>> conditions;
		EventDetails details;
		std::string bindingname;
		BINDINGTYPE type;
		int conditionsmet{ 0 };

		Binding(const std::string& name, const BINDINGTYPE& t) :bindingname(name), type(t) {
		}
		void AddCondition(const EventType& evnttype, const EventInfo& evntinfo) {
			conditions.emplace_back(evnttype, evntinfo);
		}
		virtual void ReadIn(Attributes& attributes) = 0;
		friend Binding* operator>>(Attributes& stream, Binding* obj) {
			obj->ReadIn(stream);
			return obj;
		}
		operator std::string() const {
			return std::string{};
		}
	};
	struct GameBinding :public Binding {
		GameBinding(const std::string& bindingname) :Binding(bindingname, BINDINGTYPE::GAME) {
		}
		void ReadIn(Attributes& stream) override {
			while (!stream.eof()) {
				auto keyattributes = KeyProcessing::ExtractAttributesToStream(stream.GetWord(), false);
				if (keyattributes.PeekWord().empty()) continue;
				unsigned int evnttype;
				unsigned int code;
				EventInfo evntinfo;
				keyattributes >> evnttype >> code;
				evntinfo = std::move(code);
				AddCondition(static_cast<EventType>(evnttype), std::move(evntinfo));
			}
		}
	};
	struct GUIBinding : public Binding {
		GUIBinding(const std::string& bindingname) :Binding(bindingname, BINDINGTYPE::GUI) {
		}
		void ReadIn(Attributes& attributes) override {
			std::string hierarchy;
			auto evnttype = EventType::NULLTYPE;
			auto guistate = GUIData::GUIStateData::GUIState::NULLSTATE;
			while (!attributes.eof()) {
				auto keyattributes = KeyProcessing::ExtractAttributesToStream(attributes.GetWord(), true);
				auto attributetype = KeyProcessing::ToUpperString(keyattributes.GetWord());
				auto attribute = keyattributes.GetWord();
				if (attributetype == "GUIEVENTTYPE") evnttype = EventTypeConverter(KeyProcessing::ToUpperString(attribute));
				else if (attributetype == "GUISTATE") guistate = GUIData::GUIStateData::converter(KeyProcessing::ToUpperString(attribute));
				else if (attributetype == "HIERARCHY") {
					hierarchy += std::move(attribute);
					hierarchy += ' ';
				}
				else LOG::Log(LOCATION::GUIBINDING, LOGTYPE::ERROR, __FUNCTION__, "Unable to recognise the binding type '" + attributetype + "' for binding of name " + bindingname);
			}
			if (evnttype == EventType::NULLTYPE) { LOG::Log(LOCATION::GUIBINDING, LOGTYPE::ERROR, __FUNCTION__, "GUIEventType for binding of name " + bindingname + " was NULLTYPE");return; }
			else if (guistate == GUIState::NULLSTATE) { LOG::Log(LOCATION::GUIBINDING, LOGTYPE::ERROR, __FUNCTION__, "GUIState for binding of name " + bindingname + " was NULLSTATE");return;}
			//construct a guievnt.
			GUIEventInfo evnt;
			evnt.interfacehierarchy = std::move(hierarchy);
			evnt.elementstate = std::move(guistate);
			AddCondition(evnttype, std::move(evnt));
		}
	};

}
#endif