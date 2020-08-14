#include "State_Level_Editor.h"
#include "Manager_GUI.h"

State_LevelEditor::State_LevelEditor(Manager_State* statemgr, Manager_GUI* guimanager):State_Base(statemgr,guimanager){
	guimgr->CreateStateInterface(GameStateData::GameStateType::LEVELEDITOR, "Bot_Panel","Interface_StateLevelEditor_Bot_Panel.txt");
	guimgr->CreateStateInterface(GameStateData::GameStateType::LEVELEDITOR, "PopUp_Panel", "Interface_StateLevelEditor_PopUp_Panel.txt");
	guimgr->CreateStateInterface(GameStateData::GameStateType::LEVELEDITOR, "Right_Panel", "Interface_StateLevelEditor_Right_Panel.txt");
	guimgr->CreateStateInterface(GameStateData::GameStateType::LEVELEDITOR, "Top_Panel", "Interface_StateLevelEditor_Top_Panel.txt");
}

void State_LevelEditor::draw(sf::RenderTarget& target, sf::RenderStates state) const{

}

void State_LevelEditor::Update(const float& dT)
{

}

void State_LevelEditor::Activate()
{

}

void State_LevelEditor::Deactivate()
{

}

void State_LevelEditor::OnCreate()
{

}

void State_LevelEditor::OnDestroy()
{

}

void State_LevelEditor::UpdateCamera()
{

}

void State_LevelEditor::Continue()
{

}
