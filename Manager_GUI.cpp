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
using GUIData::GUIStateData::GUIState;
using namespace EventData;
using namespace GUIData;

Manager_GUI::Manager_GUI(SharedContext* cntxt) :context(cntxt) {
	RegisterElementProducer<GUIInterface>(GUIType::WINDOW);
	RegisterElementProducer<GUITextfield>(GUIType::TEXTFIELD);
	RegisterElementProducer<GUILabel>(GUIType::LABEL);
	RegisterElementProducer<GUIScrollbar>(GUIType::SCROLLBAR);
	RegisterElementProducer<GUICheckbox>(GUIType::CHECKBOX);
	stateinterfaces[GameStateType::GAME] = Interfaces{};
	stateinterfaces[GameStateType::LEVELEDITOR] = Interfaces{};
}

GUIStateStyles Manager_GUI::CreateStyleFromFile(const std::string& stylefile){
	FileReader file(stylefile);
	if (!file.IsOpen()) {
		LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the GUIStyle file of name " + stylefile + ". RETURNING DEFAULT STYLES..");
		return GUIStateStyles{};
	}
	GUIStateStyles styles;
	while (file.NextLine().GetFileStream()) {
		Keys linekeys = KeyProcessing::ExtractValidKeys(file.ReturnLine());
		if (linekeys.empty()) continue;
		GUIState currentstate;
		auto statekey = KeyProcessing::GetKey("GUISTATE", linekeys);
		if (!statekey.first) continue;
		currentstate = GUIData::GUIStateData::converter(statekey.second->second);
		if (currentstate == GUIState::NULLSTATE) continue;
// 		auto styleproperty = KeyProcessing::GetKey("STYLE_PROPERTY", linekeys);
// 		if (!styleproperty.first) continue;
		auto& style = styles.at(static_cast<int>(currentstate));
		

		if (style.ReadIn(linekeys) == STYLE_ATTRIBUTE::NULLTYPE) {
			std::cout << "NULL" << std::endl;
			//invalid line.
			//tell user.
		}
	
	}
	return styles;
}

GUIElementPtr Manager_GUI::CreateElement(const GUIType& TYPE, GUIInterface* parent, Keys& keys){
	if (elementfactory.find(TYPE) == elementfactory.end()) return nullptr;
	KeyProcessing::FillMissingKeys({ { "STYLE_FILE", "", }, { "ELEMENT_HIDDEN", "FALSE" }, { "POSITION_X","ERROR" }, { "POSITION_Y","ERROR" }, { "ORIGIN_X", "ERROR" }, { "ORIGIN_Y", "ERROR" }, { "SIZE_X", "ERROR" }, { "SIZE_Y","ERROR" },
	{ "CUSTOM_TEXT", "ERROR" }, { "ENABLED","TRUE" },
		{ "WINX", std::to_string(this->context->window->GetRenderWindow()->getSize().x) },
		{ "WINY",std::to_string(this->context->window->GetRenderWindow()->getSize().y) } }, keys);
	using KeyProcessing::KeyPair;
	std::string stylefile = keys.find("STYLE_FILE")->second;
	auto element = elementfactory[TYPE](parent); //DANGEROUS, ELT MAY NOT BE REGISTERED
	element->OnElementCreate(context->texturemgr, context->fontmgr, keys, CreateStyleFromFile(std::move(stylefile)));
	return std::move(element);
}
GUIInterfacePtr Manager_GUI::CreateInterfaceFromFile(const std::string& interfacefile) {
	using KeyProcessing::KeyPair;
	using KeyProcessing::Keys;
	FileReader file;
	if (!file.LoadFile(interfacefile)) {
		LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the interface template file of name " + interfacefile);
		return nullptr;
	}
	std::string appenderrorstr{ " in interface file of name " + interfacefile + ". " };
	//check if the first entry is a new interface. 
	file.NextLine();
	{
		KeyProcessing::Keys linekeys = KeyProcessing::ExtractValidKeys(file.ReturnLine());
		if (KeyProcessing::FillMissingKey({ "ELEMENT_TYPE","WINDOW" }, linekeys)) {
			LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify leading GUIWindow " + appenderrorstr + "The first entry should always be {ELEMENT_TYPE,WINDOW} {CONFIGURATION_TYPE,NEW}. EXITING INTERFACE READ..");
			return nullptr;
		}
		if (auto it = KeyProcessing::GetKey("CONFIGURATION", linekeys); it.first) {
			if (it.second->second != "NEW") {
				LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "The leading GUIWindow entry must contain a {CONFIGURATION,NEW} key" + appenderrorstr + "EXITING INTERFACE READ..");
				return nullptr;
			}

		}
	}
	using IsNestedInterface = bool;
	file.PutBackLine();
	GUIInterface* leadinginterface{ nullptr };
	GUIInterface* masterinterface{ nullptr };
	int ninterfaces{ 0 };
	std::vector<std::pair<IsNestedInterface, std::vector<GUIElementPtr>>> interfacehierarchy;
	while (file.NextLine().GetFileStream()) {
		Keys linekeys = KeyProcessing::ExtractValidKeys(file.ReturnLine());
		//fill the standard keys for base guielement.
		auto missingessentials = KeyProcessing::FillMissingKeys(std::vector<KeyPair>{ {"ELEMENT_TYPE", "FATALERROR"}, { "ELEMENT_NAME", "FATALERROR" }}, linekeys);
		if (!missingessentials.empty()) {
			std::string missing;
			std::for_each(missingessentials.begin(), missingessentials.end(), [&missing](const KeyPair& key) {
				missing += "{" + key.first + ",ARG} "; });
			LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify essential " + missing + " keys" + appenderrorstr + " on line " + file.GetLineNumberString());
			continue;
		}
		GUIType element_type = GUIData::GUITypeData::converter(linekeys.find("ELEMENT_TYPE")->second);
		GUIElementPtr element;
		GUIInterface* elementparent = leadinginterface;
		//check if there is a configuration key. otherwise, default is nested.
		if (!KeyProcessing::FillMissingKey(KeyPair{ "CONFIGURATION", "X" }, linekeys)) {
			if (linekeys.find("CONFIGURATION")->second == "NEW") elementparent = masterinterface;
		}
		try { element = CreateElement(element_type, elementparent, linekeys); }
		catch (const CustomException& exception) {
			if (std::string{ exception.what() } == "ELEMENTTYPE") LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Unable to read the GUIElement on line " + file.GetLineNumberString() + appenderrorstr + "DID NOT READ ELEMENT..");
			continue;
		}
		if (element_type == GUIType::WINDOW) {
			bool isnested = (elementparent == leadinginterface);
			if (ninterfaces == 0) {
				masterinterface = static_cast<GUIInterface*>(element.get());
				isnested = false;
			}
			//create a new structure line
			
			interfacehierarchy.push_back({ std::move(isnested), std::vector<GUIElementPtr>{} });
			//this is now the leading interface. subsequent nested interfaces will be relative to this one.
			leadinginterface = static_cast<GUIInterface*>(element.get());
			//add the interface as the first element within its structure
			interfacehierarchy[ninterfaces].second.emplace_back(std::move(element));
			++ninterfaces;
		}
		//if its an element, add the element to the lastmost active interface structure
		else {
			int hierarchypos = ninterfaces - 1;
			if (elementparent == masterinterface) hierarchypos = 0;
			interfacehierarchy.at(hierarchypos).second.emplace_back(std::move(element));
		}
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
			int hierarchypos = i - 1;
			if (structure.first == false) hierarchypos = 0; //if the current hierarchy is a new interface.
			auto parent = static_cast<GUIInterface*>(interfacehierarchy.at(hierarchypos).second.at(0).get());
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
				if (!activetextfield->GetTextfieldString().empty()) activetextfield->PopChar();
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
		interface.second->Draw(*context->window->GetRenderWindow(), true);
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
