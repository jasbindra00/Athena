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
		virtual void ReadIn(const KeyProcessing::Keys& keys) = 0;
		friend Binding* operator>>(const KeyProcessing::Keys& keys, Binding* obj) {
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

		void ReadIn(const Keys& keys) override {
			// 			std::string registeredkeys;
			// 			for (auto& key : keys) {
			// 				unsigned int evnttype;
			// 				unsigned int code;
			// 				try {
			// 					evnttype = std::stoi(key.first);
			// 					code = std::stoi(key.second);
			// 				}
			// 				catch (const std::exception& exception) {
			// 					LOG::Log(LOCATION::STANDARDBINDING, LOGTYPE::ERROR, __FUNCTION__, "Unable to convert arguments in Standard Binding condition of name " + bindingname);
			// 					continue;
			// 				}
			// 				EventInfo evntinfo = std::move(code);
			// 				AddCondition(static_cast<EventType>(evnttype), std::move(evntinfo));
			// 				registeredkeys += KeyProcessing::ConstructKey(key.first, key.second) + " ";
			// 			}
			//LOG::Log(LOCATION::STANDARDBINDING, LOGTYPE::STANDARDLOG, __FUNCTION__, "Successfully initialised GameBinding of name " + bindingname + " with conditions : " + registeredkeys);
		}
	};

	//init via ctor
	struct GUIBinding : public Binding {
		GUIBinding(const std::string& bindingname) :Binding(bindingname, BINDINGTYPE::GUI) {
		}
		void ReadIn(const Keys& keys) override {
			// 			//we need two keys, as well as at least one hierarchy key.
			// 			EventData::EventType evnttype = EventData::EventTypeConverter(keys.at(0).second);
			// 			if (evnttype == EventData::EventType::NULLTYPE) throw CustomException("Invalid EventType for GUIBinding of name " + bindingname);
			// 			GUIData::GUIStateData::GUIState guistate = GUIData::GUIStateData::converter(keys.at(1).second);
			// 			if (guistate == GUIData::GUIStateData::GUIState::NULLSTATE) throw CustomException("Invalid GUIState Type for GUIBinding of name " + bindingname);
			// 			std::string tmphierarchy;
			// 			//construct the hierarchy string using the remaining keys.
			// 			for (auto it = keys.begin() + 2; it != keys.end(); ++it) {
			// 				if (it->first == "HIERARCHY") tmphierarchy += it->second + " ";
			// 			}
			// 			GUIEventInfo evntinfo;
			// 			evntinfo.elementstate = std::move(guistate);
			// 			evntinfo.interfacehierarchy = std::move(tmphierarchy);
			// 			AddCondition(std::move(evnttype), std::move(evntinfo));
			// 		}

		}
	};
}
#endif