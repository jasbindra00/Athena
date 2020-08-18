#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "Manager_GUI.h"
#include "Manager_Texture.h"
#include "Manager_Font.h"
#include "GUITextfield.h"
#include "GUIScrollbar.h"
#include "GUICheckbox.h"
#include "GUILabel.h"
#include "Utility.h"
#include "FileReader.h"
#include "GameStateData.h"
#include "SharedContext.h"
#include "Window.h"
#include "GUIData.h"
#include "EventData.h"
#include <array>
using GameStateData::GameStateType;
using GUIData::GUITypeData::GUIType;
using namespace EventData;
using namespace GUIData;

Manager_GUI::Manager_GUI(SharedContext* cntxt) :context(cntxt) {
	RegisterElementProducer<GUITextfield>(GUIType::TEXTFIELD);
	RegisterElementProducer<GUILabel>(GUIType::LABEL);
	RegisterElementProducer<GUIScrollbar>(GUIType::SCROLLBAR);
	RegisterElementProducer<GUICheckbox>(GUIType::CHECKBOX);
	stateinterfaces[GameStateType::GAME] = Interfaces{};
	stateinterfaces[GameStateType::LEVELEDITOR] = Interfaces{  };
}

GUIStateStyles Manager_GUI::CreateStyleFromFile(const std::string& stylefile){
	FileReader file(stylefile);
	if (!file.IsOpen()) {
		LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the GUIStyle file of name " + stylefile + ". RETURNING DEFAULT STYLES..");
		return GUIStateStyles{};
	}
	GUIStateStyles styles;
	styles[(GUIState::NEUTRAL)] = GUIStyle{};
	styles[(GUIState::FOCUSED)] = GUIStyle{};
	styles[(GUIState::CLICKED)] = GUIStyle{};

	while (file.NextLine().GetFileStream()) {
		Keys linekeys = KeyProcessing::ExtractValidKeys(file.ReturnLine());
		GUIState currentstate;
		try {
			auto foundstatekey = linekeys.find("GUISTATE");
			if (foundstatekey == linekeys.end()) throw CustomException("Unable to identify the {GUISTATE,STATE} key ");
			currentstate = GUIData::GUIStateData::converter(foundstatekey->second);
			if (currentstate == GUIState::NULLSTATE) throw CustomException("Unable to identify the GUISTATE argument ");
		}
		catch (const CustomException& exception) {
			LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, std::string{ exception.what() } + " on line " + file.GetLineNumberString() + " in style file of name " + stylefile + ".DID NOT READ ATTRIBUTE...");
			continue;
		}
		try { linekeys >> styles.at(currentstate); }
		catch (const CustomException& exception) {

		}
	}
	return styles;
}
GUIElementPtr Manager_GUI::CreateElement(GUIInterface* parent, Keys& keys) {
	using KeyProcessing::KeyPair;
	std::string elttype = keys.find("ELEMENTTYPE")->second;
	std::string stylefile = keys.find("STYLEFILE")->second;
	if (elttype == "NEWINTERFACE" || elttype == "NESTEDINTERFACE") {
		return std::make_unique<GUIInterface>(parent, this, CreateStyleFromFile(stylefile), keys);
	}
	else {
		auto guielementtype = GUIData::GUITypeData::converter(elttype);
		if (guielementtype == GUIType::NULLTYPE) throw CustomException("ELEMENTTYPE");
		return elementfactory[guielementtype](parent, CreateStyleFromFile(stylefile), keys);
	}
}
GUIInterfacePtr Manager_GUI::CreateInterfaceFromFile(const std::string& interfacefile) {
	using KeyProcessing::KeyPair;
	using KeyProcessing::Keys;
	std::string appenderrorstr{ " in interface file of name " + interfacefile+ ". " };
	FileReader file;
	if (!file.LoadFile(interfacefile)) {
		LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the interface template file of name " + interfacefile);
		return nullptr;
	}
	std::vector<std::pair<std::string,std::vector<GUIElementPtr>>> interfacehierarchy;
	auto linestream = static_cast<Attributes*>(&file.GetLineStream());
	//check if the first entry is a new interface. 
	file.NextLine();
		auto type = linestream->PeekWord();
		if (type != "{ELEMENTTYPE,NEWINTERFACE}" || type == "{ELEMENTTYPE,NESTEDINTERFACE}") { //cannot begin with nested interface.
			LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify leading interface" + appenderrorstr + "Leading interfaces must start with INTERFACE. EXITING INTERFACE READ..");
			return nullptr;
		}
	file.PutBackLine();
	GUIInterface* leadinginterface{ nullptr };
	GUIInterface* masterinterface{ nullptr };
	int ninterfaces{ 0 };

	while (file.NextLine().GetFileStream()) {
		Keys linekeys = KeyProcessing::ExtractValidKeys(file.ReturnLine());
		
		//fill the standard keys for base guielement.
		KeyProcessing::FillMissingKeys(std::vector<KeyPair>{ {"ELEMENTTYPE", "FATALERROR"}, { "STYLEFILE", "FATALERROR" }, { "ELEMENTNAME", "FATALERROR" }, { "ELEMENT_HIDDEN", "FALSE" },
			{ "POSITIONX","ERROR" }, { "POSITIONY","ERROR" }, { "POSITIONX%", "ERROR" }, { "POSITIONY%", "ERROR" }, { "SIZEX", "ERROR" }, { "SIZEY","ERROR" },
			{ "SIZEX%", "ERROR" }, { "SIZEY%", "ERROR" }, { "ORIGINX%","ERROR" }, { "ORIGINY%","ERROR" }, { "CUSTOMTEXT", "ERROR" }, { "ENABLED","TRUE" }, { "WINX", std::to_string(this->context->window->GetRenderWindow()->getSize().x) }, { "WINY",std::to_string(this->context->window->GetRenderWindow()->getSize().y) }}, linekeys);
		if (linekeys.find("ELEMENTNAME")->second == "POP_UP_PANEL") {
			int g = 3;
		}
		std::string elttype = linekeys.find("ELEMENTTYPE")->second;
		//fill keys for derived guielements.
		GUIElementPtr element;
		try { element = (elttype == "NESTEDINTERFACE" || elttype != "NEWINTERFACE") ? CreateElement(leadinginterface, linekeys) : CreateElement(masterinterface, linekeys); }
		catch (const CustomException& exception) {
			if (std::string{ exception.what() } == "ELEMENTTYPE") {
				LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Unable to read the GUIElement on line " + file.GetLineNumberString() + appenderrorstr + "DID NOT READ ELEMENT..");
			}
			continue;
		}
		if (dynamic_cast<GUIInterface*>(element.get())) {//if the element is an interface
			if (ninterfaces == 0) masterinterface = static_cast<GUIInterface*>(element.get()); //****************CHANGE THIS
			std::string elttype = linekeys.find("ELEMENTTYPE")->second;
			//create a new structure line
			interfacehierarchy.push_back(std::make_pair(elttype,std::vector<GUIElementPtr>{}));
			//this is now the leading interface. subsequent nested interfaces will be relative to this one.
			leadinginterface = static_cast<GUIInterface*>(element.get());
			//add the interface as the first element within its structure
			interfacehierarchy[ninterfaces].second.emplace_back(std::move(element));
			++ninterfaces;
		}
		//if its an element, add the element to the lastmost active interface structure
		else interfacehierarchy[ninterfaces - 1].second.emplace_back(std::move(element));
	}
	//link up all the individual interfaces to their elements
	for (auto& structure : interfacehierarchy) {
		auto currentinterface = static_cast<GUIInterface*>(structure.second[0].get());
		for (int i = structure.second.size() - 1; i > 0; --i) { //now, loop through all of the elements (coming after the first interface entry within the structure)
			auto& element = structure.second[i];
			if (!currentinterface->AddElement(element->GetName(), element)) { 
				LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Unable to add the GUIElement of name " + element->GetName() + " to interface of name " + currentinterface->GetName() + " in line " + file.GetLineNumberString() + " in interface file of name " + interfacefile);
				element.reset();
				continue;
			}
		}
	}
	//link up each interface to its parent 
	for (int i = interfacehierarchy.size() - 1; i > 0; --i) {
		auto& structure = interfacehierarchy.at(i);
		auto& interfaceptr = structure.second.at(0);
		//If the current element is a new interface, then the parent is the master (first) interface.
		//Else if the current element is a nested interface, then the parent is the interface that which came immediately before it.
		GUIInterface* parent = static_cast<GUIInterface*>((structure.first == "NEWINTERFACE") ? interfacehierarchy.at(0).second.at(0).get() : interfacehierarchy.at(i - 1).second.at(0).get());
		if (!parent->AddElement(interfaceptr->GetName(), interfaceptr)) {
			LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Ambiguous element name within interface file of name " + interfacefile + "DID NOT ADD ELEMENT..");
		}
	}
	return std::unique_ptr<GUIInterface>(static_cast<GUIInterface*>(interfacehierarchy[0].second[0].release())); //return the master interface.
}
std::pair<bool,Interfaces::iterator> Manager_GUI::FindInterface(const GameStateType& state, const std::string& interfacename) noexcept{
	auto& interfaces = stateinterfaces.at(state);
	auto foundinterface = std::find_if(interfaces.begin(), interfaces.end(), [interfacename](const auto& p) {
		return p.first == interfacename;
		});
	return (foundinterface == interfaces.end()) ? std::make_pair(false, foundinterface) : std::make_pair(true, foundinterface);
}
GUIInterface* Manager_GUI::CreateStateInterface(const GameStateType& state, const std::string& name, const std::string& interfacefile){
	auto interfaceexists = FindInterface(state, name);
	if (interfaceexists.first == true) {
		LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Interface of name " + name + " within the game state " + std::to_string(Utility::ConvertToUnderlyingType(state)) + " already exists.");
		return nullptr;
	}
	auto interfaceobj = CreateInterfaceFromFile(interfacefile);
	if (interfaceobj == nullptr) return nullptr;
	GUIInterface* ptr = interfaceobj.get();
	stateinterfaces[state].emplace_back(std::make_pair(name, std::move(interfaceobj)));
	return ptr;
}
bool Manager_GUI::RemoveStateInterface(const GameStateType& state, const std::string& name){
	auto foundinterface = FindInterface(state, name);
	if (foundinterface.first == true) {
		LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "State " + std::to_string(Utility::ConvertToUnderlyingType(state)) + " does not have an interface of name " + name);
		return false;
	}
	stateinterfaces.at(state).erase(foundinterface.second);
	return true;
}
GUIInterface* Manager_GUI::GetInterface(const GameStateType& state, const std::string& interfacename){
	auto foundinterface = FindInterface(state, interfacename);
	if (foundinterface.first == false) return nullptr;
	return foundinterface.second->second.get();
}
bool Manager_GUI::PollGUIEvent(std::pair<EventData::EventType, GUIEventInfo>& evnt){
	if (guieventqueue.PollEvent(evnt)) return true;
	return false;
}
void Manager_GUI::HandleEvent(const sf::Event& evnt, sf::RenderWindow* winptr) {
	auto evnttype = static_cast<EventType>(evnt.type);
	auto& activeinterfaces = stateinterfaces.at(activestate);
	switch (evnttype) {
	case EventType::MOUSEPRESSED: {
		auto clickcoords = sf::Vector2f{ static_cast<float>(evnt.mouseButton.x), static_cast<float>(evnt.mouseButton.y) };
		SetActiveTextfield(nullptr);
		for (auto& interfaceobj : activeinterfaces) {
			if (interfaceobj.second->IsHidden() || !interfaceobj.second->IsEnabled()) continue;
			interfaceobj.second->DefocusTextfields();
			if (interfaceobj.second->Contains(clickcoords)) {
				if (interfaceobj.second->GetActiveState() == GUIState::NEUTRAL) {
					interfaceobj.second->OnClick(clickcoords);
				}
			}
			else if (interfaceobj.second->GetActiveState() == GUIState::FOCUSED) {
				interfaceobj.second->OnLeave();
			}
		}
		break;
	}
	case EventType::MOUSERELEASED: {
		for (auto& interfaceobj : activeinterfaces) {
			if (interfaceobj.second->IsHidden()) continue;
			if (interfaceobj.second->GetActiveState() == GUIState::CLICKED) {
				interfaceobj.second->OnRelease();
			}
		}
		break;
	}
	case EventType::TEXTENTERED: {
		if (activetextfield != nullptr) {
			char c = evnt.text.unicode;
			//grab custom predicate from textfield here
			if(activetextfield->Predicate(c)) activetextfield->AppendChar(std::move(c));
		}
		break;
	}
	case EventType::KEYPRESSED: {
		if (activetextfield != nullptr) {
			if (evnt.key.code == sf::Keyboard::Key::Backspace) {
				if(!activetextfield->GetText().getString().isEmpty()) activetextfield->PopChar();
			}
			if (evnt.key.code == sf::Keyboard::Key::Enter) {
				activetextfield->OnEnter();
			}
		}
		break;
	}
	}
}
void Manager_GUI::Update(const float& dT){
	globalmouseposition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*context->window->GetRenderWindow()));
	auto &stategui = stateinterfaces.at(activestate);
	for (auto& interfaceptr : stategui) {
		if (interfaceptr.second->IsHidden()) continue;
		interfaceptr.second->Update(dT);
	}
}
void Manager_GUI::Draw() {
	auto& stategui = stateinterfaces.at(activestate);
	for (auto& interface : stategui) {
		if (interface.second->IsHidden()) continue;
		interface.second->Render();
	}
}

void Manager_GUI::SetActiveInterfacesEnable(const GUIInterface* exceptthis, const bool& enabled){
	if (exceptthis == nullptr) return;
	auto& activeinterfaces = GetActiveInterfaces();
	for (auto& interface : activeinterfaces) {
		if(interface.second.get() == exceptthis) continue;
		interface.second->SetEnabled(enabled);
	}
}
void Manager_GUI::AddGUIEvent(const std::pair<EventData::EventType,GUIEventInfo>& evnt){
	guieventqueue.InsertEvent(evnt);
}
