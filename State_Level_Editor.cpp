#include "State_Level_Editor.h"
#include "Manager_GUI.h"
#include "GameStateData.h"
#include "Manager_Event.h"
#include "GUIButton.h"
#include "GUILabel.h"
State_LevelEditor::State_LevelEditor(Manager_State* statemgr, Manager_GUI* guimanager):State_Base(statemgr,guimanager){
	guimanager->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "MyBinding", [this](EventDetails* details) {this->ActivatePopUp(details); });
	guimanager->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "MyBind", [this](EventDetails* details) {this->DeactivatePopUp(details); });
	guimanager->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "POP_UP_PANEL_CONFIRM_BUTTON", [this](EventDetails* details) {this->ConfirmButtonPopUp(details); });
	guimgr->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "Move_Selector_Left", [this](EventDetails* details) {this->MoveSelectorLeft(details); });
	guimgr->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "Move_Selector_Right", [this](EventDetails* details) {this->MoveSelectorRight(details); });
}
void State_LevelEditor::draw(sf::RenderTarget& target, sf::RenderStates state) const{
	for (auto& shape : customshapes) {
		if(shape.first)target.draw(*shape.second);
	}
}

void State_LevelEditor::Update(const float& dT){

}

void State_LevelEditor::Activate(){

}

void State_LevelEditor::Deactivate(){
}
void State_LevelEditor::OnCreate(){
	interfaces["BOT_PANEL"] = guimgr->CreateInterfaceFromFile(GameStateData::GameStateType::LEVELEDITOR, "Interface_StateLevelEditor_Bot_Panel.txt");
    interfaces["POP_UP_PANEL"] = guimgr->CreateInterfaceFromFile(GameStateData::GameStateType::LEVELEDITOR,  "Interface_StateLevelEditor_PopUp_Panel.txt");
	interfaces["RIGHT_PANEL"] = guimgr->CreateInterfaceFromFile(GameStateData::GameStateType::LEVELEDITOR,  "Interface_StateLevelEditor_Top_Panel.txt");
	guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "SPRITESHEET_NAME", "POP_UP_PANEL" })->SetPredicates(static_cast<unsigned int>(Utility::CharacterCheckData::STRING_PREDICATE::FILE_NAME));
	guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "TEXTURE_X_TEXTFIELD", "POP_UP_PANEL" })->SetPredicates(static_cast<unsigned int>(Utility::CharacterCheckData::STRING_PREDICATE::NUMBER));
	guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "TEXTURE_Y_TEXTFIELD", "POP_UP_PANEL" })->SetPredicates(static_cast<unsigned int>(Utility::CharacterCheckData::STRING_PREDICATE::NUMBER));
	ActivatePopUp(nullptr);
	auto atlasmap = guimgr->CreateInterfaceFromFile(GameStateData::GameStateType::LEVELEDITOR, "Interface_StateLevelEditor_Right_Panel.txt")->GetElement<GUILabel>("ATLAS_MAP");
	//set up the atlas map selector.
	sf::RectangleShape selector;
	selector.setSize(atlasmap->GetSize());
	selector.setFillColor(sf::Color::Red);
	selector.setPosition(atlasmap->GetGlobalPosition());
	customshapes.at(0) = std::make_pair(false,std::make_unique<sf::RectangleShape>(selector));//make it invisible until the atlas map is loaded by the user.
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
	auto& _POP_UP = interfaces.at("POP_UP_PANEL");
	_POP_UP->SetHidden(true);
	guimgr->SetActiveInterfacesEnable(_POP_UP, true);
}
void State_LevelEditor::ConfirmButtonPopUp(EventData::EventDetails* details){
	auto texturexfield = guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "TEXTURE_X_TEXTFIELD", "POP_UP_PANEL" });
	if (auto texturexstr = texturexfield->GetTextfieldString(); !texturexstr.empty() && texturexstr != texturexfield->GetDefaultTextfieldString()) {
		if (auto texturexdimension = std::stof(texturexfield->GetTextfieldString()); texturexdimension <= 32) {
			auto textureyfield = guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "TEXTURE_Y_TEXTFIELD", "POP_UP_PANEL" });
			if (auto textureystr = textureyfield->GetTextfieldString();  !textureyfield->GetTextfieldString().empty() && textureystr != textureyfield->GetDefaultTextfieldString()) {
				if (auto textureydimension = std::stof(textureyfield->GetTextfieldString()); textureydimension <= 32) {
					std::string spritesheetname = guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "SPRITESHEET_NAME", "POP_UP_PANEL" })->GetTextfieldString();
					if (LoadSheet(spritesheetname)) {
						DeactivatePopUp(details);
						//configure the selector size.
						customshapes.at(0).first = true; //set the selector to visible.
						auto atlas_map_dimensions = static_cast<sf::Vector2f>(atlasmap->getSize());
						auto selector = static_cast<sf::RectangleShape*>(customshapes.at(0).second.get());
						auto atlas_element_dimensions = selector->getSize();
						sf::Vector2f scale{ atlas_element_dimensions.x / atlas_map_dimensions.x, atlas_element_dimensions.y / atlas_map_dimensions.y };
						sf::Vector2f sprite_dimensions{ texturexdimension, textureydimension };
						selector->setSize(sf::Vector2f{ sprite_dimensions.x * scale.x, sprite_dimensions.y * scale.y });
					}
					
				}
			}
		}
	}
}

void State_LevelEditor::MoveSelectorLeft(EventData::EventDetails* details){
	auto interfacepos = interfaces.at("RIGHT_PANEL")->GetGlobalPosition();

	auto selector = GetShape<sf::RectangleShape, 0>();
	selector->move(sf::Vector2f{ -selector->getSize().x, 0 });
}

void State_LevelEditor::MoveSelectorRight(EventData::EventDetails* details){//CHANGE EVENT MANAGER TO SUPPORT &| BINDINGS
	auto selector = GetShape<sf::RectangleShape, 0>();
	selector->move(sf::Vector2f{ selector->getSize().x, 0 });
}

bool State_LevelEditor::LoadSheet(const std::string& sheetname){
	atlasmap = guimgr->context->GetResourceManager<sf::Texture>()->RequestResource(sheetname);
	if (atlasmap != nullptr) {
		GUILabel* elt = guimgr->GetElement<GUILabel>(GameStateType::LEVELEDITOR,  { "ATLAS_MAP",  "RIGHT_PANEL" });
		elt->GetActiveStyle().ReadIn(STYLE_ATTRIBUTE::BG_TEXTURE_NAME, sheetname);
		spritesheet_loaded = true;
		return true;
	}
	return false;
}
