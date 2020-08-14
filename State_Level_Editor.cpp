#include "State_Level_Editor.h"
#include "Manager_GUI.h"
#include "GameStateData.h"
#include "Manager_Event.h"
State_LevelEditor::State_LevelEditor(Manager_State* statemgr, Manager_GUI* guimanager):State_Base(statemgr,guimanager){
	guimgr->CreateStateInterface(GameStateData::GameStateType::LEVELEDITOR, "Bot_Panel","Interface_StateLevelEditor_Bot_Panel.txt");
	guimgr->CreateStateInterface(GameStateData::GameStateType::LEVELEDITOR, "PopUp_Panel", "Interface_StateLevelEditor_PopUp_Panel.txt");
	guimgr->CreateStateInterface(GameStateData::GameStateType::LEVELEDITOR, "Right_Panel", "Interface_StateLevelEditor_Right_Panel.txt");
	guimgr->CreateStateInterface(GameStateData::GameStateType::LEVELEDITOR, "Top_Panel", "Interface_StateLevelEditor_Top_Panel.txt");
	guimanager->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "MyBinding", [this](EventDetails* details) {this->ActivatePopUp(details); });
	guimanager->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "MyBind", [this](EventDetails* details) {this->DeactivatePopUp(details); });
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

void State_LevelEditor::ActivatePopUp(EventData::EventDetails* details){
	guimgr->GetInterface(GameStateData::GameStateType::LEVELEDITOR, "PopUp_Panel")->SetHidden(false);
}

void State_LevelEditor::DeactivatePopUp(EventData::EventDetails* details)
{
	guimgr->GetInterface(GameStateData::GameStateType::LEVELEDITOR, "PopUp_Panel")->SetHidden(true);
}
