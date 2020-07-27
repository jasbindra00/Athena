#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Manager_Event.h"
#include "GameStateType.h"
#include "Window.h"
#include "FileReader.h"
#include "Utility.h"
#include "GUIEventData.h"
#include "KeyProcessing.h"
#include "Manager_GUI.h"

using GameEventData::WindowEventType;
Manager_Event::Manager_Event(Manager_GUI* guimanager) noexcept :guimgr(guimanager)  {
	statebindingobjects[GameStateType::INTRO];
	statebindingobjects[GameStateType::MAINMENU];
	statebindingobjects[GameStateType::GAME];

	statebindingcallables[GameStateType::INTRO];
	statebindingcallables[GameStateType::MAINMENU];
	statebindingcallables[GameStateType::GAME];
	guistatebindingobjects[GameStateType::INTRO];
	guistatebindingobjects[GameStateType::MAINMENU];
	guistatebindingobjects[GameStateType::GAME];
	//statebindingcallables[StateType::CREDITS];
	//statebindingcallables[StateType::RESUMEGAMESTATE];
	//statebindingcallables[StateType::GAMELOSTSTATE];
	LoadBindings("Bindings2.txt");
}

bool Manager_Event::RegisterBindingCallable(const GameStateType& state, const std::string& bindingname, const BindingTypes::BindingCallable& action) {
	auto callableexists = FindBindingData<BindingCallable>(state, bindingname);
	if (callableexists.first) {
		LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Callable for binding of name " + bindingname + " already exists.");
		return false;
	}
	statebindingcallables[state].emplace_back(bindingname, std::make_unique<BindingCallable>(action));
	return true;
}
void Manager_Event::HandleEvent(const GUIEventData::GUIEventInfo& evnt) {
	

}
//handle non gui bindings -> active variant is guaranteed to be ind 0 (int)
void Manager_Event::HandleEvent(const sf::Event& evnt, sf::RenderWindow* winptr) { //only considering the bindings of the active state.
	//guimgr->HandleEvent(evnt, winptr); //checks if any of the events cause change within any active interfaces.
	sf::Clock c;
	auto eventtype = static_cast<GameEventData::WindowEventType>(evnt.type);
	auto& statebindings = statebindingobjects[activestate];
	for (auto& binding : statebindings){
		auto& bindingobject = binding.second;
		for (auto& bindingcondition : bindingobject->conditions){
			auto& code = bindingcondition.second.code;
			if (bindingcondition.first == eventtype) {
				if (bindingcondition.first == GameEventData::WindowEventType::KEYPRESSED || bindingcondition.first == GameEventData::WindowEventType::KEYRELEASED) {
 					const auto& eventcode = evnt.key.code;
					if (code == eventcode) {
						auto& latestkeypressed = bindingobject->details.keycode;
						if (latestkeypressed != code) {//if the key hasn't been pressed already
							latestkeypressed = code;
							++bindingobject->conditionsmet; //its a first time match. condition met.
							break;
						}
					}
				}
			}//
				else if (bindingcondition.first == GameEventData::WindowEventType::MOUSEPRESSED || bindingcondition.first == GameEventData::WindowEventType::MOUSERELEASED) {
					const auto& eventcode = evnt.mouseButton.button;
					if (code == eventcode) {
						auto& latestmousepress = bindingobject->details.mousecode;
						if (latestmousepress != code) {
							latestmousepress = code;
							bindingobject->details.mouseposition = sf::Vector2i{ evnt.mouseButton.x,evnt.mouseButton.y };
							++bindingobject->conditionsmet;
							break;
						}

					}
				}
		}
	}
}
void Manager_Event::Update(sf::RenderWindow* winptr) { //handling live input events
	ProcessGUIEvents();
	auto& statebindings = statebindingobjects[activestate];
	for (auto& binding : statebindings) {
		auto& bindingobject = binding.second;
		for (const auto& bindingcondition : bindingobject->conditions) {
			auto& evnttype = bindingcondition.first;
			auto& code = bindingcondition.second.code;
			switch (evnttype) {
			case GameEventData::WindowEventType::KEYPRESSED: {
				if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(code))) {
					auto& latestkeypressed = bindingobject->details.keycode;
					if (latestkeypressed != code) {
						latestkeypressed = code;
						++bindingobject->conditionsmet;
					}
				}
				break;
			}
			case GameEventData::WindowEventType::MOUSEPRESSED: {
				if (sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(code))) {
					auto& latestmousepress = bindingobject->details.mousecode;
					if (latestmousepress != code) {
						latestmousepress = code;
						bindingobject->details.mouseposition = sf::Mouse::getPosition(*winptr);
						++bindingobject->conditionsmet;
						break;
					}
				}
			}
			}
		}
		if (bindingobject->conditionsmet == bindingobject->conditions.size()) { //checking if this binding has had all of its conditions met
			auto foundcallable = FindBindingData<BindingCallable>(activestate, bindingobject->bindingname);
			if (foundcallable.first) {
				auto& callable = *foundcallable.second->second;
				callable(&bindingobject->details);
			}
		}
		bindingobject->conditionsmet = 0; //resetting after every iteration.
		bindingobject->details.Reset();
	}

}
void Manager_Event::LoadBindings(const std::string& filename) {
	FileReader file;
	if (!file.LoadFile(filename)) {
		LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the binding file of name " + filename);
		return;
	}
	if (!file.CheckStandardSyntax({ "***BINDING SYNTAX*** GAMESTATE BINDING BINDINGNAME {EVENTTYPE, CODE} ....n", "***GUIBINDING SYNTAX*** GAMESTATE GUIBINDING BINDINGNAME {GUIEVENTTYPE,TYPE} {GUISTATE,STATE} {HIERARCHY,ELTNAME}....{HIERARCHY,TARGET}" })){
		LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Binding Syntax Guidelines are incorrect in binding file of name " + filename);
	}
	auto attributes = static_cast<Attributes*>(&file.GetLineStream());
	while (!file.EndOfFile()) {
		file.NextLine();
		GameStateType gamestate; 
		if (attributes->PeekWord().empty()) continue;
		gamestate = GameState::converter(file.GetWord());
		if (gamestate == GameStateType::NULLSTATE) { LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to recognise the game state on line " + file.GetLineNumberString() + " in binding file of name " + filename); continue; }
		auto bindingtype = attributes->GetWord();
		auto bindingname = attributes->GetWord();
		if (bindingtype == "BINDING") RegisterBindingObject<GameEventData::StandardBinding>(gamestate,bindingname, attributes);
		else if (bindingtype == "GUIBINDING") RegisterBindingObject<GameEventData::GUIBinding>(gamestate, bindingname,attributes);
		else { LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to recognise the binding type on line " + file.GetLineNumberString());}
		
	}
	file.CloseFile();
}
void Manager_Event::ProcessGUIEvents() {
	GUIEventData::GUIEventInfo evnt;
	while (guimgr->PollGUIEvent(evnt)) {
		HandleEvent(std::move(evnt));
	}
}
