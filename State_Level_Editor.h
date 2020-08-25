#ifndef STATE_LEVEL_EDITOR_H
#define STATE_LEVEL_EDITOR_H
#include "State_Base.h"
#include "GUIInterface.h"
#include "EventData.h"
class Manager_State;
class State_LevelEditor : public State_Base {
private:
	using ElementHierarchyStorage = std::vector < std::pair < std::string, std::string>>;
	ElementHierarchyStorage elementhierarchy;
	std::shared_ptr<sf::Texture> atlasmap;
	bool spritesheet_loaded{ false };
	using Visible = bool;
	std::array<std::pair<Visible,std::unique_ptr<sf::Shape>>, 1> customshapes;
	template<typename SHAPE_TYPE, std::size_t INDEX>
	SHAPE_TYPE* GetShape() {
		return dynamic_cast<SHAPE_TYPE*>(customshapes.at(INDEX).second.get());
	}
	std::unordered_map<std::string, GUIInterface*> interfaces;
public:
	State_LevelEditor(Manager_State* statemgr, Manager_GUI* guimgr);
	void draw(sf::RenderTarget& target, sf::RenderStates state) const override;
	void Update(const float& dT) override;
	void Activate() override;
	void Deactivate() override;
	void OnCreate() override;
	void OnDestroy() override;
	void UpdateCamera() override;
	void Continue() override;


	//USER INPUT
	void ActivatePopUp(EventData::EventDetails* details);
	void DeactivatePopUp(EventData::EventDetails* details);
	void ConfirmButtonPopUp(EventData::EventDetails* details);
	void MoveSelectorLeft(EventData::EventDetails* details);
	void MoveSelectorRight(EventData::EventDetails* details);
	bool LoadSheet(const std::string& sheetname);
	
	

};


#endif