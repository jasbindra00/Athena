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
#include "GUIEventData.h"


using namespace GUIEventData;
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
using GUIElementProducer = std::function<GUIElementPtr(GUIInterface*, GUIStateStyles, std::stringstream& attributes)>;
using GUIElementFactory = std::unordered_map<GUIType, GUIElementProducer>;

class Manager_GUI{
private:
	GameStateInterfaces stateinterfaces;
	EventQueue<GUIEventInfo> guieventqueue;
	GUIElementFactory elementfactory;

	sf::Vector2f globalmouseposition;
	SharedContext* context;
	mutable GameStateType activestate;

	template<typename T>
	void RegisterElementProducer(const GUIType& type) { //factory pattern
		elementfactory[type] = [type](GUIInterface* parent, const GUIStateStyles& style, std::stringstream& attributes) {return std::make_unique<T>(parent, style, attributes); };
	}
	GUIStateStyles CreateStyleFromFile(const std::string& stylefile);
	GUIElementPtr CreateElement(GUIInterface* parent, Attributes& attributes);
	GUIInterfacePtr CreateInterfaceFromFile(const std::string& interfacefile);
	std::pair<bool,Interfaces::iterator> FindInterface(const GameStateType& state, const std::string& interfacename) noexcept;
public:
	Manager_GUI(SharedContext* context);
	
	bool CreateStateInterface(const GameStateType& state, const std::string& name, const std::string& interfacefile);
	bool RemoveStateInterface(const GameStateType& state, const std::string& name);

	inline void SetActiveState(const GameStateType& state) const { activestate = state; }
	
	
	void Update(const float& dT);
	void Draw();

	bool PollGUIEvent(GUIEventInfo& evnt);
	void AddGUIEvent(const GUIEventInfo& evnt);
	void HandleEvent(const sf::Event& evnt, sf::RenderWindow* winptr);
	SharedContext* GetContext() const { return context; }
	GUIInterface* GetInterface(const GameStateType& state, const std::string& interfacename);
	GUIType StringToGUIType(const std::string& str) const{
		if (str == "INTERFACE") return GUIType::WINDOW;
		else if (str == "LABEL") return GUIType::LABEL;
		else if (str == "TEXTFIELD") return GUIType::TEXTFIELD;
		else if (str == "SCROLLBAR") return GUIType::SCROLLBAR;
		return GUIType::NULLTYPE;
	}
};



#endif