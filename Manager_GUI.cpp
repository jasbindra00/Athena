#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "Manager_GUI.h"
#include "Manager_Texture.h"
#include "Manager_Font.h"
#include "GUITextfield.h"
#include "GUILabel.h"
#include "GUIScrollbar.h"
#include "GUICheckbox.h"
#include "GUIButton.h"
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
	RegisterElementProducer<GUIButton>(GUIType::BUTTON);
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
GUIInterface* Manager_GUI::CreateInterfaceFromFile(const GameStateType& state, const std::string& interface_file) { //MAKE RECURSIVE
	using KeyProcessing::KeyPair;
	using KeyProcessing::Keys;
	FileReader file;
	std::string errorstr{ " in interface hierarchy file of name " + interface_file +". " };
	if (!file.LoadFile(interface_file)) {
		LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the interface template file of name " + interface_file);
		return nullptr;
	}
	KeyProcessing::Keys linekeys;
	using EssentialInfo = std::pair<GUIType, std::string>;

	auto ValidateEssentialKeys = [&linekeys, &errorstr](const std::string& linenumber)->EssentialInfo {
		EssentialInfo vals{ GUIType::NULLTYPE, "" };
		auto keys = KeyProcessing::GetKeys({ "ELEMENT_TYPE", "ELEMENT_NAME" }, linekeys);
		try {
			if(!(keys.at(0).first || keys.at(1).first)) throw CustomException("Unable to find essential GUI {ELEMENT_TYPE,TYPE} &| {ELEMENT_NAME,NAME} key(s)  ");
			auto element_type_valid = magic_enum::enum_cast<GUIType>(keys.at(0).second->second);
			if (!element_type_valid.has_value()) throw CustomException("Unable to identify the GUIElement type ");
			vals.first = std::move(element_type_valid.value());
			vals.second = std::move(keys.at(1).second->second);
		}
		catch (const CustomException& exception) {
			LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, std::string{ exception.what() } + " on line " + linenumber + errorstr);
		}
		return vals;
	};
	
	//first line should be a master interface
	using IsNestedInterface = bool;
	GUIInterface* leading_interface{ nullptr };
	GUIInterface* master_interface{ nullptr };
	int n_interfaces{ 0 };

	std::vector<std::pair<IsNestedInterface, std::vector<GUIElementPtr>>> interface_hierarchy;
	while (file.NextLine().GetFileStream()) {
		linekeys = KeyProcessing::ExtractValidKeys(file.ReturnLine());
		EssentialInfo essentialinfo = ValidateEssentialKeys(file.GetLineNumberString());
		if (essentialinfo.first == GUIType::NULLTYPE) return nullptr;
		if (n_interfaces == 0 && FindInterface(state, essentialinfo.second).first) {
			LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Master interface of name " + essentialinfo.second + " already exists in game state " + std::string{ magic_enum::enum_name(state) } + errorstr + "EXITING FILE READ..");
			return nullptr;
		}
		//first entry must always be a GUI interface.
		if (n_interfaces == 0 && essentialinfo.first != GUIType::WINDOW) essentialinfo.first = GUIType::WINDOW;
		//the hierarchy configuration is always relative to the previously specified interface, unless explicitly stated.
		GUIInterface* element_parent = leading_interface;
		if (auto configuration_key = KeyProcessing::GetKey("CONFIGURATION", linekeys); configuration_key.first) {
			//in which case it will be relative to the first interface specified within the file.
			if (configuration_key.second->second == "NEW") element_parent = master_interface;
		}
		GUIElementPtr resultant_elt;
		resultant_elt = CreateElement(essentialinfo.first, element_parent, linekeys);
		//a newly mentioned interface will have its own elements. all subsequent entries will be relative to this interface (unless explicitly stated otherwise by the {CONFIGURATION,TYPE} key)
		if (essentialinfo.first == GUIType::WINDOW) {
			bool isnested = (element_parent == leading_interface);
			if (n_interfaces == 0) {
				master_interface = static_cast<GUIInterface*>(resultant_elt.get());
				isnested = false;
			}
			interface_hierarchy.push_back({ std::move(isnested), std::vector<GUIElementPtr>{} });
			//this is now the leading interface. subsequent nested interfaces will be relative to this one.
			leading_interface = static_cast<GUIInterface*>(resultant_elt.get());
			//add the interface as the first element within its structure
			interface_hierarchy[n_interfaces].second.emplace_back(std::move(resultant_elt));
			++n_interfaces;
		}
		//if its an element, add the element to the corresponding interface structure
		else {
			int hierarchypos = n_interfaces - 1; //structure of the recent most interface
			if (element_parent == master_interface) hierarchypos = 0; //structure of the master interface
			interface_hierarchy.at(hierarchypos).second.emplace_back(std::move(resultant_elt));
		}
	}
	//link up all the individual interfaces to their elements
	for (auto& structure : interface_hierarchy) {
		auto currentinterface = static_cast<GUIInterface*>(structure.second[0].get());
		for (int i = structure.second.size() - 1; i > 0; --i) { //now, loop through all of the elements (coming after the first interface entry within the structure)
			auto& element = structure.second[i];
			if (!currentinterface->AddElement(element->GetName(), element)) {
				LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Unable to add the GUIElement of name " + element->GetName() + " to interface of name " + currentinterface->GetName() + " in line " + file.GetLineNumberString() + " in interface file of name " + interface_file);
				element.reset();
				continue;
			}
		}
	}
	//link up each interface to its parent 
	for (int i = interface_hierarchy.size() - 1; i > 0; --i) {
		auto& structure = interface_hierarchy.at(i);
		auto& interfaceptr = structure.second.at(0);
		int hierarchypos = i - 1;
		if (structure.first == false) hierarchypos = 0; //if the current hierarchy is a new interface.
		auto parent = static_cast<GUIInterface*>(interface_hierarchy.at(hierarchypos).second.at(0).get());
		if (!parent->AddElement(interfaceptr->GetName(), interfaceptr)) {
			LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Ambiguous element name within interface file of name " + interface_file + "DID NOT ADD ELEMENT..");
		}
	}
	auto MASTER_UNIQUE = std::unique_ptr<GUIInterface>(static_cast<GUIInterface*>(interface_hierarchy[0].second[0].release()));
	stateinterfaces.at(state).emplace_back(std::make_pair(MASTER_UNIQUE->GetName(), std::move(MASTER_UNIQUE)));
	return master_interface;
	
	}


std::pair<bool,Interfaces::iterator> Manager_GUI::FindInterface(const GameStateType& state, const std::string& interfacename) noexcept{
	auto& interfaces = stateinterfaces.at(state);
	auto foundinterface = std::find_if(interfaces.begin(), interfaces.end(), [interfacename](const auto& p) {
		return p.first == interfacename;
		});
	return (foundinterface == interfaces.end()) ? std::make_pair(false, foundinterface) : std::make_pair(true, foundinterface);
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
			interfaceobj.second->DefocusStickyElements();
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
