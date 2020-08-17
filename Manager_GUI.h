#ifndef MANAGER_GUI_H
#define MANAGER_GUI_H
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include "EventQueue.h"
#include "GUIFormatting.h"
#include "GUIInterface.h"
#include "SharedContext.h"
#include "EventData.h"
#include "GameStateData.h"
#include "GUITextfield.h"

using EventData::GUIEventInfo;
using GameStateData::GameStateType;
enum class GUIElementType;
class GUIElement;
class GUIInterface;
class Manager_Font;
class Manager_Texture;
class Attributes;

using GUIInterfacePtr = std::unique_ptr<GUIInterface>;
using Interfaces = std::vector<std::pair<std::string,GUIInterfacePtr>>;

using GameStateInterfaces = std::unordered_map<GameStateType, Interfaces>; //each game state has a number of GUI interfaces.

using GUIElementPtr = std::unique_ptr<GUIElement>;
using GUIElementProducer = std::function<GUIElementPtr(GUIInterface*, GUIStateStyles, KeyProcessing::Keys&)>;
using GUIElementFactory = std::unordered_map<GUIType, GUIElementProducer>;

class Manager_GUI{
	friend class State_LevelEditor;
private:
	GameStateInterfaces stateinterfaces;
	EventQueue<std::pair<EventData::EventType,GUIEventInfo>> guieventqueue;
	GUIElementFactory elementfactory;
	sf::Vector2f globalmouseposition;
	SharedContext* context;
	mutable GameStateType activestate;
	mutable GUITextfield* activetextfield{ nullptr };
	template<typename T>
	void RegisterElementProducer(const GUIType& type) { //factory pattern
		elementfactory[type] = [type](GUIInterface* parent, const GUIStateStyles& style, KeyProcessing::Keys& keys) {return std::make_unique<T>(parent, style, keys); };
	}
	GUIStateStyles CreateStyleFromFile(const std::string& stylefile);
	GUIElementPtr CreateElement(GUIInterface* parent, Keys& keys);
	GUIInterfacePtr CreateInterfaceFromFile(const std::string& interfacefile);
	std::pair<bool,Interfaces::iterator> FindInterface(const GameStateType& state, const std::string& interfacename) noexcept;
protected:
	Interfaces& GetActiveInterfaces() { return stateinterfaces.at(activestate); }
public:
	Manager_GUI(SharedContext* context);
	
	template<typename T>
	T* GetElement(const GameStateType& state, const std::string hierarchy) {
		Attributes hierarchystream(hierarchy);
		GUIElement* elt = GetInterface(hierarchystream.GetWord());
		while (hierarchystream.NextWord()) {
			if (!elt) return nullptr;
			if (dynamic_cast<GUIInterface*>(elt)) {
				elt = static_cast<GUIInterface*>(elt)->GetElement(hierarchystream.GetWord());
			}
			else break; //we have reached an element.
		}
		//at this point, we are either at the end of the stream with no words left (elt not found)
		if (hierarchystream.eof()) return nullptr;
		if (hierarchystream.GetWord() == elt->GetName()) return dynamic_cast<T*>(elt);
	}

	GUIInterface* CreateStateInterface(const GameStateType& state, const std::string& name, const std::string& interfacefile);
	bool RemoveStateInterface(const GameStateType& state, const std::string& name);

	inline void SetActiveState(const GameStateType& state) const { activestate = state; activetextfield = nullptr; }
	
	void Update(const float& dT);
	void Draw();

	void SetActiveInterfacesEnable(const GUIInterface* const exceptthis, const bool& enabled);
	bool PollGUIEvent(std::pair<EventData::EventType,GUIEventInfo>& evnt);
	void AddGUIEvent(const std::pair<EventData::EventType, GUIEventInfo>& evnt);
	void HandleEvent(const sf::Event& evnt, sf::RenderWindow* winptr);
	SharedContext* GetContext() const { return context; }
	GUIInterface* GetInterface(const GameStateType& state, const std::string& interfacename);
	
	sf::Vector2f GetGlobalMousePosition() const { return globalmouseposition; }
	void SetActiveTextfield(GUITextfield* ptr) {
		activetextfield = ptr;
	}
	

};



#endif