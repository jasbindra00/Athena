#ifndef STATETYPE_H
#define STATETYPE_H
#include "EnumConverter.h"

namespace GameState {
	static enum class GameStateType {
		INTRO,MAINMENU, OPTIONS, GAME, GAMELOST, NULLSTATE
	};
	static EnumConverter<GameStateType> converter([](const std::string& str)->GameStateType {
		if (str == "INTRO") return GameStateType::INTRO;
		else if (str == "MAINMENU") return GameStateType::MAINMENU;
		else if (str == "OPTIONS") return GameStateType::OPTIONS;
		else if (str == "GAME") return GameStateType::GAME;
		else if (str == "GAMELOST") return GameStateType::GAMELOST;
		return GameStateType::NULLSTATE;
		});
}
using GameState::GameStateType;
#endif


/*

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
					if (bindingcondition.second.keycode == eventcode) {
						auto& latestkeypressed = bindingobject->details.keycode;
						if (latestkeypressed != bindingcondition.second.keycode) {//if the key hasn't been pressed already
							latestkeypressed = bindingcondition.second.keycode;
							bindingobject->conditionsmet++; //its a first time match. condition met.
							break;
						}
					}
				}
			}
				if (bindingcondition.first == EventType::MOUSEPRESSED || bindingcondition.first == EventType::MOUSERELEASED) {
					const auto& eventcode = evnt.mouseButton.button;
					if (bindingcondition.second.keycode == eventcode) {
						auto& latestmousepress = bindingobject->details.mousecode;
						if (latestmousepress != bindingcondition.second.keycode) {
							latestmousepress = bindingcondition.second.keycode;
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
			const auto& code = bindingcondition.second.keycode;
			switch (static_cast<EventType>(bindingcondition.first)) {
			case EventType::KEYPRESSED: {
				if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(code))) {
					auto& latestkeypressed = bindingobject->details.keycode;
					if (latestkeypressed != bindingcondition.second.keycode) {
						latestkeypressed = bindingcondition.second.keycode;
						bindingobject->conditionsmet++;
					}
				}
				break;
			}
			case EventType::MOUSEPRESSED: {
				if (sf::Mouse::isButtonPressed(static_cast<sf::Mouse::Button>(code))) {
					auto& latestmousepress = bindingobject->details.mousecode;
					if (latestmousepress != bindingcondition.second.keycode) {
						latestmousepress = bindingcondition.second.keycode;
						//bindingobject->details.mouseposition = sf::Mouse::getPosition(*winptr->GetWindow());
						bindingobject->conditionsmet++;
						break;
					}
				}
			}
			}
		}
		if (bindingobject->conditionsmet == bindingobject->conditions.size()) { //checking if this binding has had all of its conditions met
			auto bindingcallable = FindBinding(statebindingcallables, activestate, bindingobject->bindingname);
			if (bindingcallable != statebindingcallables[activestate].end()) {
				bindingcallable->second(&bindingobject->details); //execute its callable if all conditions have been met.
			}
		}
		bindingobject->conditionsmet = 0; //resetting after every iteration.
		bindingobject->details.Reset();
	}
}





*/