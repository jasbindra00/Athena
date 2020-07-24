#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Manager_Event.h"
#include "GameStateType.h"
#include "Window.h"
Manager_Event::Manager_Event() noexcept {
	statebindingcallables[StateType::INTRO];
	statebindingcallables[StateType::MAINMENU];
	statebindingcallables[StateType::GAME];
	//statebindingcallables[StateType::CREDITS];
	//statebindingcallables[StateType::RESUMEGAMESTATE];
	//statebindingcallables[StateType::GAMELOSTSTATE];
	LoadBindings("Bindings.txt");
	
}
void Manager_Event::RemoveBindingData(const StateType& state, const std::string& bindingname) {
	auto foundcallable = FindBinding(statebindingcallables, state, bindingname);
	if (foundcallable != statebindingcallables[state].end()) statebindingcallables[state].erase(foundcallable);
	auto foundobject = FindBinding(statebindingobjects, state, bindingname);
	if (foundobject != statebindingobjects[state].end()) statebindingobjects[state].erase(foundobject);
}
void Manager_Event::RegisterBindingObject(const StateType& state, std::unique_ptr<Binding> bindingobject) {
	statebindingobjects[state].insert({ bindingobject->bindingname,std::move(bindingobject) });
}
void Manager_Event::RegisterBindingCallable(const StateType& state, const std::string& bindingname, BindingCallable action) {
	auto binding = FindBinding(statebindingobjects,state, bindingname);
	if (binding == statebindingobjects[state].end()){
		LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to find binding of name " + bindingname + ". RegisterBindingCallable() failed.");
		return;
	}
	statebindingcallables[state].insert(std::make_pair(bindingname, action));
}
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
void Manager_Event::LoadBindings(const std::string& filename) {


	
	std::string fcnname{ "LoadBindings()" };
	std::fstream bindingfile(filename, std::ios_base::in);
	if (!bindingfile.is_open()) {
		Utility::log.Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, "LoadBindings()", "Could not open binding file of name " + filename);
		return;
	}
	int linenumber{ 1 };
	std::string line;
	std::stringstream wordstream; //separates grabbed line into words(delimiter = space)
	auto StringToStateType = [](const std::string& state)->StateType { //helper lambda : converts statestring into state enum
		if (state == "GAME") return StateType::GAME;
		if (state == "INTRO") return StateType::INTRO;
		if (state == "MAINMENU") return StateType::MAINMENU;
		if (state == "OPTIONS") return StateType::OPTIONS;
		if (state == "GAMELOST") return StateType::GAMELOST;
		return StateType::NULLSTATE;
	};
	auto CheckKeySyntax = [](const std::string& key, EventType& eventtype, int& eventinfo)->bool { //checks key, if valid, extracts string values into input reference values.
		//standard syntax for a key is {,} //helper lambda : checks syntax of binding key and extracts info from the string if correct.
		std::string syntax{ "{,}" }; //standard binding key syntax
		if (key == syntax) return false;
		std::string reduction = key; 
		std::string eventtypestr; //extracted info strings
		std::string eventinfostr;
		bool passedcomma = false;
		reduction.erase(std::remove_if(reduction.begin(), reduction.end(), [&eventtypestr, &eventinfostr, &passedcomma](const auto& c)
			{
				if (c == ',') passedcomma = true; //if we are onto the second number within the key.
				if (c >= '0' && c <= '9') { //if currentchar is a number, determine which number, the first or second one to push into
					if (!passedcomma) eventtypestr.push_back(c);
					if (passedcomma) eventinfostr.push_back(c);
					return true;
				}
				return false;
			}), reduction.end());
		if (reduction != syntax) return false; //invalid key syntax.
		eventtype = static_cast<EventType>(std::stoi(eventtypestr)); 
		eventinfo = std::stoi(eventinfostr);
	};
	while (!bindingfile.eof()) {
		line.clear();
		wordstream = std::stringstream{};

		std::getline(bindingfile, line);
		wordstream << line;
		//statetype
		StateType bindingstate;
		std::string detail;
		wordstream >> detail;
		bindingstate = StringToStateType(detail);
		if (bindingstate == StateType::NULLSTATE) {
			Utility::log.Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, fcnname, "NULLSTATE - could not recognise state on line " + std::to_string(linenumber));
			++linenumber;
			continue;
		}
		//bindingname
		detail.clear();
		wordstream >> detail;
		auto bindingexists = FindBinding(statebindingobjects, bindingstate, detail);
		if (bindingexists != statebindingobjects[bindingstate].end()) {
			Utility::log.Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, fcnname, "Binding of name " + detail + " in State " + std::to_string(Utility::ConvertToUnderlyingType(bindingstate)) + " already exists. Overwriting binding...");
		}
		auto bindingobject = std::make_unique<Binding>(detail);
		//n binding conditions
		while (!wordstream.eof()) {
			detail.clear();
			EventType eventtype;
			int eventinfocode{ 0 };
			wordstream >> detail;
			if (!CheckKeySyntax(detail, eventtype, eventinfocode)) {
				Utility::log.Log(LOCATION::MANAGER_EVENT, LOGTYPE::ERROR, fcnname, "Binding key on line " + std::to_string(linenumber) + " has an invalid syntax");
				++linenumber;
				continue;
			}
			bindingobject->AddCondition(eventtype, eventinfocode); //valid key. add the binding.
		}
		LOG::Log(LOCATION::MANAGER_EVENT, LOGTYPE::STANDARDLOG, __FUNCTION__, "|BINDING CREATED| |STATE :" + std::to_string(Utility::ConvertToUnderlyingType(bindingstate)) + "|" + std::string(*bindingobject));
		std::cout << *bindingobject << std::endl;
		statebindingobjects[bindingstate].insert({ bindingobject->bindingname, std::move(bindingobject) });
		++linenumber;
	}


}
