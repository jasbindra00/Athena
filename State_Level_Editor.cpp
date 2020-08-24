#include "State_Level_Editor.h"
#include "Manager_GUI.h"
#include "GameStateData.h"
#include "Manager_Event.h"
#include "GUILabel.h"
State_LevelEditor::State_LevelEditor(Manager_State* statemgr, Manager_GUI* guimanager):State_Base(statemgr,guimanager){
	guimanager->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "MyBinding", [this](EventDetails* details) {this->ActivatePopUp(details); });
	guimanager->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "MyBind", [this](EventDetails* details) {this->DeactivatePopUp(details); });
	guimanager->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "POP_UP_PANEL_CONFIRM_BUTTON", [this](EventDetails* details) {this->ConfirmButtonPopUp(details); });
}
void State_LevelEditor::draw(sf::RenderTarget& target, sf::RenderStates state) const{
	
}

void State_LevelEditor::Update(const float& dT){

}

void State_LevelEditor::Activate(){

}

void State_LevelEditor::Deactivate(){
}
void State_LevelEditor::OnCreate(){
	guimgr->CreateStateInterface(GameStateData::GameStateType::LEVELEDITOR, "Bot_Panel", "Interface_StateLevelEditor_Bot_Panel.txt");
	auto popup = guimgr->CreateStateInterface(GameStateData::GameStateType::LEVELEDITOR, "POP_UP_PANEL", "Interface_StateLevelEditor_PopUp_Panel.txt");
	guimgr->CreateStateInterface(GameStateData::GameStateType::LEVELEDITOR, "RIGHT_PANEL", "Interface_StateLevelEditor_Right_Panel.txt");
	guimgr->CreateStateInterface(GameStateData::GameStateType::LEVELEDITOR, "Top_Panel", "Interface_StateLevelEditor_Top_Panel.txt");
	guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "SPRITESHEET_NAME", "POP_UP_PANEL" })->SetPredicates(static_cast<unsigned int>(Utility::CharacterCheckData::STRING_PREDICATE::FILE_NAME));
	guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "TEXTURE_X_TEXTFIELD", "POP_UP_PANEL" })->SetPredicates(static_cast<unsigned int>(Utility::CharacterCheckData::STRING_PREDICATE::NUMBER));
	guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "TEXTURE_Y_TEXTFIELD", "POP_UP_PANEL" })->SetPredicates(static_cast<unsigned int>(Utility::CharacterCheckData::STRING_PREDICATE::NUMBER));
	ActivatePopUp(nullptr);
	
	
}
void State_LevelEditor::OnDestroy(){

}

void State_LevelEditor::UpdateCamera()
{

}
void State_LevelEditor::Continue()
{

}
void State_LevelEditor::ActivatePopUp(EventData::EventDetails* details){
	GUIInterface* panel = guimgr->GetInterface(GameStateData::GameStateType::LEVELEDITOR, "POP_UP_PANEL");
	panel->SetHidden(false);
	guimgr->SetActiveInterfacesEnable(panel, false);
}
void State_LevelEditor::DeactivatePopUp(EventData::EventDetails* details) {
	GUIInterface* panel = guimgr->GetInterface(GameStateData::GameStateType::LEVELEDITOR, "POP_UP_PANEL");
	panel->SetHidden(true);
	guimgr->SetActiveInterfacesEnable(panel, true);
}
void State_LevelEditor::ConfirmButtonPopUp(EventData::EventDetails* details){

	auto texturexfield = guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "TEXTURE_X_TEXTFIELD", "POP_UP_PANEL" });
	auto textureyfield = guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "TEXTURE_Y_TEXTFIELD", "POP_UP_PANEL" });
	guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "SPRITESHEET_NAME", "POP_UP_PANEL" })->GetTextfieldString();

	



	if (auto texturexstr = texturexfield->GetTextfieldString(); !texturexstr.empty() && texturexstr != texturexfield->GetDefaultTextfieldString()){
		if (std::stoi(texturexfield->GetTextfieldString()) <= 32) {
			auto textureyfield = guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "TEXTURE_Y_TEXTFIELD", "POP_UP_PANEL" });
			if (auto textureystr = textureyfield->GetTextfieldString();  !textureyfield->GetTextfieldString().empty() && textureystr != textureyfield->GetDefaultTextfieldString()) {
				if (std::stoi(textureyfield->GetTextfieldString()) <= 32) {
					std::string spritesheetname = guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "SPRITESHEET_NAME", "POP_UP_PANEL" })->GetTextfieldString();
					if (LoadSheet(spritesheetname)) {
						DeactivatePopUp(details);
						return;
					}
					//sheet has been loaded.
				}
			}
		}
	}
}

bool State_LevelEditor::LoadSheet(const std::string& sheetname){
	atlasmap = guimgr->context->GetResourceManager<sf::Texture>()->RequestResource(sheetname);
	if (atlasmap != nullptr) {
		GUILabel* elt = guimgr->GetElement<GUILabel>(GameStateType::LEVELEDITOR,  { "ATLASMAP",  "RIGHT_PANEL" });
		elt->SetHidden(true);
		return true;
	}
	return false;
}
