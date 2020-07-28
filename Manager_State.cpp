#include "Manager_State.h"
#include "State_Base.h"
#include "State_Intro.h"
#include "State_Game.h"
#include "State_Menu.h"
#include "State_Options.h"
#include"State_Options.h"
#include "SharedContext.h"
#include "Window.h"

#include "GameStateData.h"


using GameStateData::GameStateType;
Manager_State::Manager_State(SharedContext* c):context(c) {

	RegisterStateProducer<State_Intro>(GameStateType::INTRO);
	RegisterStateProducer<State_Game>(GameStateType::GAME);
	SwitchState(GameStateType::INTRO);
}
bool Manager_State::StateExists(const GameStateType& state) const{
	auto pendingdestruction = std::find(destructionqueue.begin(), destructionqueue.end(), state);
	if (pendingdestruction != destructionqueue.end()) return false; //state does not exist if it is pending destruction
	auto foundstate = std::find_if(statestack.begin(), statestack.end(), [&state](const auto& p) {
		return state == p.first;
		});
	if (foundstate == statestack.end()) return false;
	return true;
}
void Manager_State::ProcessInsertions() {
	if (insertionqueue.empty()) return;
	for (int i = insertionqueue.size() - 1; i >= 0; --i) {
		SwitchState(insertionqueue[i]);
	}
	insertionqueue.clear();
}
void Manager_State::RemoveStateProducer(const GameStateType& state){
	auto found = statefactory.find(state);
	statefactory.erase(found);
}
void Manager_State::Update(const float& dT) {
	ProcessDestructions();
	if (statestack.empty()) return;
	auto currentstate = statestack.end() - 1;
	while (currentstate > statestack.begin()) { //finding the first non transcendent state
		if (currentstate->second->GetTranscendency() == false) break;
		--currentstate;
	}
	while (currentstate != statestack.end()) { //updating upwards from that state.
		currentstate->second->Update(dT);
		++currentstate; 
	}
	ProcessInsertions();
}
void Manager_State::ProcessDestructions() {
	if (destructionqueue.empty()) return;
	for (int i = destructionqueue.size() - 1; i >= 0; --i) {
		auto foundstate = std::find_if(statestack.begin(), statestack.end(), [i, this](const auto& p) {//finding the state in our objects
			return destructionqueue[i] == p.first;
			});
		if (foundstate == statestack.end()) return; //tried to destroy state which did not exist
		foundstate->second->OnDestroy();
		foundstate->second.reset();
		statestack.erase(foundstate);
	}
	destructionqueue.clear();
}
void Manager_State::SwitchState(const GameStateType& s) {
	auto foundstate = std::find_if(statestack.begin(), statestack.end(), [s](const auto& p) {
		return p.first == s;
		});
	if (foundstate != statestack.end()) {//state already exists. shift state to top of stack.
		auto tmp = std::move(foundstate->second);
		statestack.erase(foundstate);
		statestack.push_back(std::pair<GameStateType, StatePtr>{s, std::move(tmp)});
		statestack.back().second->Activate();
		return;
	}
	//if the state does not exist, create and push
	auto stateobj = statefactory[s]();
	stateobj->OnCreate();
	statestack.emplace_back(s, std::move(stateobj));
	context->eventmanager->SwitchToState(s);
	//NEED TO CALL SWITCH STATE IN STATEMGR.
}
void Manager_State::Draw() {
	auto currentstate = statestack.end() - 1;
	while (currentstate > statestack.begin()) {
		if (currentstate->second->GetTransparency() == false) break;
		currentstate--;
	}
	while (currentstate != statestack.end()) {
		context->window->GetRenderWindow()->draw(*currentstate->second);
		currentstate++;
	}
}

