#ifndef MANAGER_GUI_H
#define MANAGER_GUI_H
#include <unordered_map>
#include <vector>
#include <string>
#include <functional>
#include "EventQueue.h"
#include "GUIFormatting.h"
#include "GUIInterface.h"
#include "SharedContext.h"
#include "GUIEvents.h"

enum class StateType;
enum class GUIElementType;
class GUIElement;
class GUIInterface;
class Manager_Font;
class Manager_Texture;


using GUIInterfacePtr = std::unique_ptr<GUIInterface>;
using StateInterfaces = std::unordered_map<std::string,GUIInterfacePtr>;

using Interfaces = std::unordered_map<StateType, StateInterfaces>; //each game state has a number of GUI interfaces.

using GUIElementPtr = std::unique_ptr<GUIElement>;
using GUIElementProducer = std::function<GUIElementPtr(GUIInterface*, GUIStateStyles, std::stringstream& attributes)>;
using GUIElementFactory = std::unordered_map<GUIType, GUIElementProducer>;

class Manager_GUI{
protected:
	Interfaces interfaces;
	EventQueue<GUIEvent> guieventqueue;
	GUIElementFactory elementfactory;
	GUIStateStyles ReadStyle(const std::string& stylefile);
	
	SharedContext* context;

	mutable StateType activestate;
	GUIElementPtr CreateElement(GUIInterface* parent, std::stringstream& attributes);
	GUIInterfacePtr CreateInterfaceFromFile(const std::string& interfacetemplatefile);
public:
	Manager_GUI(SharedContext* context);
	
	bool RegisterInterface(const StateType& state, const std::string& name, const std::string& interfacefile);
	bool RemoveInterface(const StateType& state, const std::string& name);
	void SetActiveState(const StateType& state) const { activestate = state; }
	GUIInterface* GetInterface(const StateType& state, const std::string& interfacename);
	bool PollEvent(GUIEvent& evnt); 
	bool RemoveElement(const StateType& state, const std::string& interfacename, const std::string& elementname);
	void Update(const float& dT);
	void Draw();

	template<typename T>
	void RegisterElementProducer(const GUIType& type) {
		elementfactory[type] = [type](GUIInterface* parent, const GUIStateStyles& style, std::stringstream& attributes) {return std::make_unique<T>(parent,style, attributes); };
	}
	
	void AddEvent(const GUIEvent& evnt);
	SharedContext* GetContext() const { return context; }
	GUIType StringToGUIType(const std::string& str) const{
		if (str == "INTERFACE") return GUIType::WINDOW;
		else if (str == "LABEL") return GUIType::LABEL;
		else if (str == "TEXTFIELD") return GUIType::TEXTFIELD;
		return GUIType::NULLTYPE;
	}

};



#endif