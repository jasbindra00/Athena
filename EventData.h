#ifndef GAMEEVENTS_H
#define GAMEEVENTS_H
#include <vector>
#include <SFML\Window\Event.hpp>
#include <string>
#include <type_traits>
#include <variant>
#include "StreamAttributes.h"
#include "KeyProcessing.h"
#include "Utility.h"
#include "GUIData.h"
#include "EnumConverter.h"
#include "CustomException.h"



using GUIData::GUIStateData::GUIState;
using GUIData::GUITypeData::GUIType;
using KeyProcessing::Keys;




namespace EventData {
	enum class EventType {
		KEYPRESSED = sf::Event::EventType::KeyPressed,
		KEYRELEASED = sf::Event::EventType::KeyReleased,
		MOUSEPRESSED = sf::Event::EventType::MouseButtonPressed,
		MOUSERELEASED = sf::Event::EventType::MouseButtonReleased,
		MOUSESCROLLED = sf::Event::EventType::MouseWheelScrolled,
		MOUSEMOVED = sf::Event::EventType::MouseMoved,
		TEXTENTERED = sf::Event::EventType::TextEntered,
		GUI_CLICK = 13,
		GUI_RELEASE = 14,
		NULLTYPE = 17
	};
	static EnumConverter<EventType> EventTypeConverter([](const std::string& str)->EventType {
		if (str == "KEYPRESSED") return EventType::KEYPRESSED;
		else if (str == "KEYRELEASED") return EventType::KEYRELEASED;
		else if (str == "MOUSEPRESSED") return EventType::MOUSEPRESSED;
		else if (str == "MOUSERELEASED") return EventType::MOUSERELEASED;
		else if (str == "MOUSESCROLLED") return EventType::MOUSESCROLLED;
		else if (str == "MOUSEMOVED") return EventType::MOUSEMOVED;
		else if (str == "GUI_CLICK") return EventType::GUI_CLICK;
		else if (str == "GUI_RELEASE") return EventType::GUI_RELEASE;

		return EventType::NULLTYPE;
		});
	struct GUIEventInfo {
		std::string interfacehierarchy;
		GUIData::GUIStateData::GUIState elementstate;
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
		virtual void ReadIn(KeyProcessing::Keys keys) = 0;
		friend Binding* operator>>(KeyProcessing::Keys& keys, Binding* obj) {
			obj->ReadIn(keys);
			return obj;
		}
		operator std::string() const {
			return std::string{};
		}
	};
	struct GameBinding :public Binding {
		GameBinding(const std::string& bindingname) :Binding(bindingname, BINDINGTYPE::GAME) {
		}

		void ReadIn(Keys keys) override {
			for (auto& key : keys) {
				unsigned int evnttype;
				unsigned int keycode;
				if (!KeyProcessing::IsOnlyNumeric(key.first) || !KeyProcessing::IsOnlyNumeric(key.second)){
					LOG::Log(LOCATION::STANDARDBINDING, LOGTYPE::ERROR, __FUNCTION__, "Invalid GAMEBINDING condition arguments for binding of name " + bindingname + ". Ensure that the GAMEBINDING conditions are integral types. DID NOT READ CONDITION..");
					continue;
				}
				evnttype = std::stoi(key.first);
				keycode = std::stoi(key.second);
				AddCondition(static_cast<EventType>(evnttype), std::move(keycode));
			}
		}
	};

	//init via ctor
	struct GUIBinding : public Binding {
		GUIBinding(const std::string& bindingname) :Binding(bindingname, BINDINGTYPE::GUI) {
		}
		void ReadIn(Keys keys) override {
			EventType guieventtype;
			{
				auto it = keys.find("GUIEVENTTYPE");
				if (it != keys.end()) {
					guieventtype = EventTypeConverter(it->second);
					if (guieventtype == EventType::GUI_CLICK) {
					}
				}
				if (it == keys.end() || guieventtype == EventType::NULLTYPE) throw CustomException("GUIEVENTTYPE");
				keys.erase(it);
			}
			GUIData::GUIStateData::GUIState guistate;
			{
				auto it = keys.find("GUISTATE");
				if (it != keys.end()) {
					guistate = GUIData::GUIStateData::converter(it->second);
				}
				if (it == keys.end() || guistate == GUIData::GUIStateData::GUIState::NULLSTATE) throw CustomException("GUISTATE");
				keys.erase(it);
			}
			//configure the hierarchy string
			std::string hierarchystr = Utility::ConstructGUIHierarchyString(keys);
			if (hierarchystr.empty()) return;//LOG ERROR HERE****************
			GUIEventInfo evntinfo;
			evntinfo.elementstate = std::move(guistate);
			evntinfo.interfacehierarchy = std::move(hierarchystr);
			AddCondition(std::move(guieventtype), std::move(evntinfo));
		}

		
	};
}
#endif