#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Manager_Event.h"
#include "Window.h"
#include "FileReader.h"
#include "Utility.h"
#include "EventData.h"
#include "KeyProcessing.h"
#include "Manager_GUI.h"

using EventData::EventType;
using GameStateData::GameStateType;
using EventData::Binding;
using EventData::GUIBinding;
using EventData::GameBinding;
using EventData::BINDINGTYPE;
Manager_Event::Manager_Event(Manager_GUI* guimanager) noexcept :guimgr(guimanager)  {

	statebindingobjects[GameStateType::INTRO];
	statebindingobjects[GameStateType::MAINMENU];
	statebindingobjects[GameStateType::GAME];
	statebindingobjects[GameStateType::LEVELEDITOR];

	bindingcallables[GameStateType::INTRO];
	bindingcallables[GameStateType::MAINMENU];
	bindingcallables[GameStateType::GAME];
	bindingcallables[GameStateType::INTRO];
	bindingcallables[GameStateType::MAINMENU];
	bindingcallables[GameStateType::GAME];
	bindingcallables[GameStateType::LEVELEDITOR];
	LoadBindings("Bindings2.txt");
}
using BindingTypes::BindingCallable;
bool Manager_Event::RegisterBindingCallable(const GameStateType& state, const std::string& bindingname, const BindingTypes::BindingCallable& action) {
	auto callableexists = FindBindingData<BindingCallable>(state, bindingname);
	if (callableexists.first) {
		LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Callable for binding of name " + bindingname + " already exists.");
		return false;
	}
	bindingcallables[state].emplace_back(bindingname, action);
	return true;
}
using EventData::GUIEventInfo;
void Manager_Event::HandleEvent(const std::pair<EventType, GUIEventInfo>& evnt) {
	auto& statebindings = statebindingobjects[activestate];
	for (auto& binding : statebindings) {
		auto& bindingobject = binding.second;
		if (bindingobject->type != BINDINGTYPE::GUI) continue;
		for (auto& condition : bindingobject->conditions) {
			if (condition.first != evnt.first) continue;
			auto& requiredevntinfo = std::get<1>(condition.second);
			//if (requiredevntinfo.elementstate != evnt.second.elementstate) continue;
// 			std::cout << requiredevntinfo.interfacehierarchy << std::endl;
// 			std::cout << evnt.second.interfacehierarchy << std::endl;
			if (requiredevntinfo.interfacehierarchy != evnt.second.interfacehierarchy) continue;
			//if (evnt.elementstate != guicondition.elementstate) continue;
			//its a full match.
			++bindingobject->conditionsmet;
			bindingobject->details.guiinfo = evnt.second;
		}
	}
}
using EventData::EventType;
void Manager_Event::HandleEvent(const sf::Event& evnt, sf::RenderWindow* winptr) { //only considering the bindings of the active state.
	guimgr->HandleEvent(evnt, winptr); //checks if any of the events cause change within any active interfaces.
	if (evnt.type == sf::Event::KeyPressed) {
		int x = 4;
	}
	auto eventtype = static_cast<EventType>(evnt.type);
	auto& statebindings = statebindingobjects[activestate];
	for (auto& binding : statebindings){
		if (binding.second->type == BINDINGTYPE::GUI) continue; //we don't handle gui events here.
		auto& bindingobject = binding.second;
		for (auto& bindingcondition : bindingobject->conditions){
			auto& code = std::get<0>(bindingcondition.second); //get the int member in union
			if (bindingcondition.first == eventtype) { //
				if (bindingcondition.first == EventType::KEYPRESSED || bindingcondition.first == EventType::KEYRELEASED) {
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
			}
				else if (bindingcondition.first == EventType::MOUSEPRESSED || bindingcondition.first == EventType::MOUSERELEASED) {
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
		if (bindingobject->type != BINDINGTYPE::GUI) { //gui is not applicable here 
			for (const auto& bindingcondition : bindingobject->conditions) {
				auto& evnttype = bindingcondition.first;
				auto& code = std::get<0>(bindingcondition.second);
				switch (evnttype) {
				case EventType::KEYPRESSED: {
					if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(code))) {
						auto& latestkeypressed = bindingobject->details.keycode;
						if (latestkeypressed != code) {
							latestkeypressed = code;
							++bindingobject->conditionsmet;
						}
					}
					break;
				}
				case EventType::MOUSEPRESSED: {
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
		}
		if (bindingobject->conditionsmet == bindingobject->conditions.size()) { //checking if this binding has had all of its conditions met
			auto foundcallable = FindBindingData<BindingCallable>(activestate, bindingobject->bindingname);
			std::cout << "MATCH" << std::endl;
			if (foundcallable.first) {
				auto& callable = foundcallable.second->second;
				callable(&bindingobject->details);
				
			}
		}
		bindingobject->conditionsmet = 0; //resetting after every iteration.
		bindingobject->details.Reset();
	}
}
void Manager_Event::LoadBindings(const std::string& filename) {
	using KeyProcessing::Keys;
	using KeyProcessing::KeyPair;
	FileReader file;
	if (!file.LoadFile(filename)) {
		LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the binding file of name " + filename);
		return;
	}
	while (file.NextLine().GetFileStream()) {
		Keys keys = KeyProcessing::ExtractValidKeys(file.ReturnLine());
		{
			bool err = false;
			std::vector<KeyPair> missingkeys = KeyProcessing::FillMissingKeys(std::vector<KeyPair>{ {"GAMESTATE", "ERROR"}, { "BINDINGTYPE", "ERROR" }, { "BINDINGNAME", "ERROR" }}, keys);
			for (auto& missingkey : missingkeys) {
				LOG::Log(LOCATION::MAP, LOGTYPE::ERROR,__FUNCTION__, "Unable to identify essential binding key " + KeyProcessing::ConstructKeyStr(missingkey.first, missingkey.second) + "in bindingfile of name on line " + file.GetLineNumberString() + ". DID NOT READ BINDING..");
				err = true;
			}
			if (err) continue;
		}
		GameStateData::GameStateType gamestate = GameStateData::converter(keys.find("GAMESTATE")->second);
		std::string bindingtype = keys.find("BINDINGTYPE")->second;
		std::string bindingname = keys.find("BINDINGNAME")->second;
		//check if the essential keys have valid args.
		try{
			if (gamestate == GameStateData::GameStateType::NULLSTATE)throw CustomException("Unable to identify binding Game State");
			if (bindingtype != "GUIBINDING" && bindingtype != "GAMEBINDING") {
				throw CustomException("Unable to identify binding type");
			}
			if (FindBindingData<Binding>(gamestate, bindingname).first) throw CustomException("Binding with name " + bindingname + " already exists in state " + std::to_string(Utility::ConvertToUnderlyingType(gamestate)));
		}
		catch (const CustomException& exception) {
			LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, std::string{ exception.what() } + " on line " + file.GetLineNumberString() + ". DID NOT READ BINDING..");
			continue;
		}
		keys.erase("BINDINGTYPE");
		keys.erase("BINDINGNAME");
		keys.erase("GAMESTATE");
		try {
			if (bindingtype == "GAMEBINDING") RegisterBindingObject<GameBinding>(gamestate, bindingname, std::move(keys));
			else if (bindingtype == "GUIBINDING") RegisterBindingObject<GUIBinding>(gamestate, bindingname, std::move(keys));
		}
		catch (const CustomException& exception) {
			LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, std::string{ exception.what() });
		}
	}
	file.CloseFile();
}
void Manager_Event::ProcessGUIEvents() {
	std::pair<EventType,GUIEventInfo> evnt;
	while (guimgr->PollGUIEvent(evnt)) {
		HandleEvent(std::move(evnt));
	}
}