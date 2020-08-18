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
	void ActivatePopUp(EventData::EventDetails* details);
	void DeactivatePopUp(EventData::EventDetails* details);
	void ConfirmButtonPopUp(EventData::EventDetails* details);
	bool LoadSheet(const std::string& sheetname);
protected:
	

};


#endif