#include "Manager_GUI.h"
#include "Log.h"
#include "Utility.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "GameStateType.h"
#include "GUITextfield.h"
#include "Manager_Texture.h"
#include "Manager_Font.h"
#include "FileRead.h"
Manager_GUI::Manager_GUI(SharedContext* cntxt):context(cntxt) {

	RegisterElementProducer<GUITextfield>(GUIType::TEXTFIELD);
	interfaces[StateType::GAME];

}


GUIStateStyles Manager_GUI::ReadStyle(const std::string& stylefile) {
	std::ifstream file(stylefile, std::ios::in);
	if (!file.is_open()) {
		return GUIStateStyles{};
	}
	GUIStateStyles styles;
	while (!file.eof()) {
		std::string line;
		std::getline(file, line);
		std::stringstream wordstream(line);
		std::string word;

		GUIState currentstate = GUIState::NEUTRAL;
		wordstream >> word;
		if (word == "GUIState") {
			unsigned int statetype;
			wordstream >> statetype;
			currentstate = static_cast<GUIState>(statetype);
		}
		std::getline(file, line);
		wordstream = std::stringstream{};
		wordstream << line;
		wordstream >> word;
		while (word != "/ENDSTATE" && !file.eof()) {

			if (word == "sbg" || word == "tbg") {
				wordstream.str(wordstream.str().substr(3, std::string::npos));
				wordstream >> styles[currentstate].background;
			}
			else if (word == "text") {
				wordstream.str(wordstream.str().substr(4, std::string::npos));
				wordstream >> styles[currentstate].text;
			}

			std::getline(file, line);
			wordstream = std::stringstream{};
			wordstream << line;
			wordstream >> word;
		}	
	}
	return styles;
}


GUIElementPtr Manager_GUI::CreateElement(GUIInterface* parent, std::stringstream& attributes)
{
	std::string elttype;
	attributes >> elttype;
	std::string eltname;
	attributes >> eltname;
	std::string stylefile;
	attributes >> stylefile;
	if (elttype == "INTERFACE") {
		return std::make_unique<GUIInterface>(parent, this, ReadStyle(stylefile), attributes);
	}
	else {
		auto guielementtype = StringToGUIType(elttype);
		if (guielementtype == GUIType::NULLTYPE) return nullptr;
		return elementfactory[guielementtype](parent, ReadStyle(stylefile), attributes);
	}
}

GUIInterfacePtr Manager_GUI::CreateInterfaceFromFile(const std::string& interfacefile) {
	std::ifstream file(interfacefile, std::ios_base::in);
	if (!file.is_open()) {
		LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Unable to open the interface template file of name " + interfacefile);
		return nullptr;//error
	}
	int linenumber{ 1 };
	std::vector<std::vector<GUIElementPtr>> hierachy;
	std::string line;
	std::string word;
	std::getline(file, line);
	std::stringstream wordstream(line);
	wordstream >> word;
	if (word != "INTERFACE") {
		return nullptr;
	}
	wordstream.seekg(0, std::ios_base::beg);
	file.seekg(0, std::ios_base::beg);
	int ninterfaces{ 0 };
	GUIInterface* leadinginterface{ nullptr };
	while (!file.eof()) {
		std::getline(file, line);
		wordstream = std::stringstream{ line };
		auto element = CreateElement(leadinginterface, wordstream);
		if (dynamic_cast<GUIInterface*>(element.get())) {
			hierachy.push_back(std::vector<GUIElementPtr>{});
			leadinginterface = static_cast<GUIInterface*>(element.get());
			hierachy[ninterfaces].emplace_back(std::move(element));
			++ninterfaces;
		}
		else hierachy[ninterfaces - 1].push_back(std::move(element));
	}
	GUIInterface* parentinterface{ nullptr };
		for (auto& structure : hierachy) {
			auto currentinterface = static_cast<GUIInterface*>(structure[0].get());
			if (structure.size() == 1) continue;
			for (int i = structure.size() - 1; i > 0; --i) {
				auto& element = structure[i];
				if (!currentinterface->AddElement(element->GetName(), element)) {
					element.reset();
					continue;
				}
			}
			if (parentinterface != nullptr) {
				if (!parentinterface->AddElement(structure[0]->GetName(), structure[0])) {
					structure[0].reset();
					continue;
				}
			}
			parentinterface = currentinterface;
		}
		return std::unique_ptr<GUIInterface>(static_cast<GUIInterface*>(hierachy[0][0].release()));
}
bool Manager_GUI::RegisterInterface(const StateType& state, const std::string& name, const std::string& interfacefile){
	auto interfaceexists = interfaces.at(state).find(name);
	if (interfaceexists != interfaces.at(state).end()) {
		LOG::Log(LOCATION::MANAGER_GUI, LOGTYPE::ERROR, __FUNCTION__, "Interface within the game state " + std::to_string(Utility::ConvertToUnderlyingType(state)) + " of name " + name + " already exists.");
		return false;
	}
	auto interfaceobj = CreateInterfaceFromFile(interfacefile);
	if (interfaceobj == nullptr) return false;
	interfaces[state][name] = std::move(interfaceobj);
	return true;
}

bool Manager_GUI::RemoveInterface(const StateType& state, const std::string& name){
	return true;
}

GUIInterface* Manager_GUI::GetInterface(const StateType& state, const std::string& interfacename){
	auto found = interfaces.at(state).find(interfacename);
	if (found == interfaces.at(state).end()) return nullptr;
	return found->second.get();
}

bool Manager_GUI::PollEvent(GUIEvent& evnt)
{
	return true;
}


bool Manager_GUI::RemoveElement(const StateType& state, const std::string& interfacename, const std::string& elementname)
{
	return true;
}

void Manager_GUI::Update(const float& dT){
	auto stategui = interfaces.find(activestate);
	if (stategui == interfaces.end()) return;
	for (auto& interface : stategui->second) {
		interface.second->Update(dT);
	}
}

void Manager_GUI::Draw(){
	auto stateinterfaces = interfaces.find(activestate);
	if (stateinterfaces == interfaces.end()) return;
	for (auto& interface : stateinterfaces->second) {
		interface.second->Render();
	}
}

void Manager_GUI::AddEvent(const GUIEvent& evnt)
{

}

