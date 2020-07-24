#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Manager_Event.h"
#include "GameStateType.h"
#include "Window.h"
#include "FileReader.h"
#include "Utility.h"
#include "GUIEvents.h"
#include "KeyProcessing.h"

Manager_Event::Manager_Event() noexcept {
	statebindingobjects[GameStateType::INTRO];
	statebindingobjects[GameStateType::MAINMENU];
	statebindingobjects[GameStateType::GAME];

	statebindingcallables[GameStateType::INTRO];
	statebindingcallables[GameStateType::MAINMENU];
	statebindingcallables[GameStateType::GAME];
	guibindingobjects[GameStateType::INTRO];
	guibindingobjects[GameStateType::MAINMENU];
	guibindingobjects[GameStateType::GAME];
	//statebindingcallables[StateType::CREDITS];
	//statebindingcallables[StateType::RESUMEGAMESTATE];
	//statebindingcallables[StateType::GAMELOSTSTATE];
	LoadBindings("Bindings.txt");
}
void Manager_Event::RemoveBindingData(const GameStateType& state, const std::string& bindingname) {
	{ auto foundcallable = FindBinding(statebindingcallables, state, bindingname);
	if (foundcallable.first) statebindingcallables[state].erase(foundcallable.second);
	}
	auto foundobject = FindBinding(statebindingobjects,state, bindingname);
	if (foundobject.first) statebindingobjects[state].erase(foundobject.second);
}
void Manager_Event::RegisterBindingObject(const GameStateType& state, BindingPtr& bindingobject) {
	statebindingobjects[state].emplace_back(std::make_pair( bindingobject->bindingname,std::move(bindingobject) ));
}
void Manager_Event::RegisterBindingCallable(const GameStateType& state, const std::string& bindingname, BindingCallable action) {
	auto binding = FindBinding(statebindingobjects, state, bindingname);
	if (!binding.first){
		LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to find binding of name " + bindingname + ". RegisterBindingCallable() failed.");
		return;
	}
	statebindingcallables[state].emplace_back(std::make_pair(bindingname, action));
}
void Manager_Event::HandleEvent(const GUIEvent& evnt) {
}
//handle non gui bindings -> active variant is guaranteed to be ind 0 (int)
void Manager_Event::HandleEvent(const sf::Event& evnt, sf::RenderWindow* winptr) { //only considering the bindings of the active state.
	auto eventtype = static_cast<EventType>(evnt.type);
	auto& statebindings = statebindingobjects[activestate];
	for (auto& binding : statebindings){
		auto& bindingobject = binding.second;
		for (auto& bindingcondition : bindingobject->conditions)
		{//loop through every single condition of statebindings
			if (bindingcondition.first == eventtype) {
				if (bindingcondition.first == EventType::KEYPRESSED || bindingcondition.first == EventType::KEYRELEASED) {
					const auto& eventcode = evnt.key.code;
					if (std::get<0>(bindingcondition.second.codeorguievent) == eventcode) {
						auto& latestkeypressed = bindingobject->details.keycode;
						if (latestkeypressed != std::get<0>(bindingcondition.second.codeorguievent)) {//if the key hasn't been pressed already
							latestkeypressed = std::get<0>(bindingcondition.second.codeorguievent);
							bindingobject->conditionsmet++; //its a first time match. condition met.
							break;
						}
					}
				}
			}
				if (bindingcondition.first == EventType::MOUSEPRESSED || bindingcondition.first == EventType::MOUSERELEASED) {
					const auto& eventcode = evnt.mouseButton.button;
					if (std::get<0>(bindingcondition.second.codeorguievent) == eventcode) {
						auto& latestmousepress = bindingobject->details.mousecode;
						if (latestmousepress != std::get<0>(bindingcondition.second.codeorguievent)) {
							latestmousepress = std::get<0>(bindingcondition.second.codeorguievent);
							//bindingobject->details.mouseposition = sf::Mouse::getPosition(*winptr->GetWindow());
							bindingobject->conditionsmet++;
							break;
						}

					}
				}
		}
	}
}
void Manager_Event::Update(sf::RenderWindow* winptr) { //handling live input events
	auto& statebindings = statebindingobjects[activestate];
	for (auto& binding : statebindings) {
		auto& bindingobject = binding.second;
		for (const auto& bindingcondition : bindingobject->conditions) {
			const auto& code = std::get<0>(bindingcondition.second.codeorguievent);
			switch (static_cast<EventType>(bindingcondition.first)) {
			case EventType::KEYPRESSED: {
				if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(code))) {
					auto& latestkeypressed = bindingobject->details.keycode;
					if (latestkeypressed != std::get<0>(bindingcondition.second.codeorguievent)) {
						latestkeypressed = std::get<0>(bindingcondition.second.codeorguievent);
						bindingobject->conditionsmet++;
					}
				}
				break;
			}
			case EventType::MOUSEPRESSED: {
				if (sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(code))) {
					auto& latestmousepress = bindingobject->details.mousecode;
					if (latestmousepress != std::get<0>(bindingcondition.second.codeorguievent)) {
						latestmousepress = std::get<0>(bindingcondition.second.codeorguievent);
						//bindingobject->details.mouseposition = sf::Mouse::getPosition(*winptr->GetWindow());
						bindingobject->conditionsmet++;
						break;
					}
				}
			}
			}
		}
		if (bindingobject->conditionsmet == bindingobject->conditions.size()) { //checking if this binding has had all of its conditions met
			auto bindingcallable = FindBinding(statebindingcallables,activestate, bindingobject->bindingname);
			if (bindingcallable.first == true) {
				bindingcallable.second->second(&bindingobject->details); //execute its callable if all conditions have been met.
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
	if (!file.CheckStandardSyntax({ "|SYNTAX||STANDARD BINDING| GAMESTATE BINDING BINDINGNAME {EVENTTYPE, KEYCODE} ... n", "|SYNTAX||GUI BINDING| GAMESTATE GUIBINDING BINDINGNAME HIERARCHY={INTERFACE,INTERFACE,...,ELT} GUIEVENTTYPE TYPE " })) {
		LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Binding Syntax Guidelines are incorrect in binding file of name " + filename);
	}
	auto attributes = static_cast<Attributes*>(&file.GetLineStream());
	while (!file.EndOfFile()) {
		GameStateType gamestate;{
			file.NextLine();
			gamestate = GameState::converter(file.GetWord());
			if (gamestate == GameStateType::NULLSTATE) LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to recognise the game state on line " + file.GetLineNumberString() + " in binding file of name " + filename); continue;
		}
		auto bindingtype = attributes->GetWord();
		auto bindingname = attributes->GetWord();
		attributes->seekg(0, std::ios_base::beg);
		auto binding = std::make_unique < Binding>(bindingname, *attributes);
		if (binding == nullptr) continue; //binding read error
		auto bindingptr = binding.get();
		if (bindingtype == "BINDING") {
			auto bindingexists = FindBinding(statebindingobjects,gamestate, bindingname);
			if (bindingexists.first) {
				LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Binding in state " + std::to_string(Utility::ConvertToUnderlyingType(gamestate)) + " already exists");
				continue;
			}
			statebindingobjects[gamestate].emplace_back(std::make_pair(bindingname, std::move(binding)));
		}
		else if (bindingtype == "GUIBINDING") {
			auto bindingexists = FindBinding(guibindingobjects,gamestate, bindingname);
			if (bindingexists.first) {
				LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "GUIBinding in state " + std::to_string(Utility::ConvertToUnderlyingType(gamestate)) + " already exists");
				continue;
			}
			guibindingobjects[gamestate].emplace_back(std::make_pair(bindingname, std::move(binding)));
		}
		else LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to recognise the binding type on line " + file.GetLineNumberString()); continue;
		LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::STANDARDLOG, __FUNCTION__, "Binding successfully created. " + std::string(*bindingptr));
	}

	file.CloseFile();
}
