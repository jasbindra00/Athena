#include "State_Level_Editor.h"
#include "Manager_GUI.h"
#include "GameStateData.h"
#include "Manager_Event.h"
#include "GUIButton.h"
#include "GUILabel.h"

void State_LevelEditor::SetSelectorPosition(const bool& left){
	sf::Clock c;
	while (c.getElapsedTime().asSeconds() < 0.123) { //limit the selection speed

	}
	//get the selector position.
	auto selector = GetShape<sf::RectangleShape, 0>();
	sf::Vector2f selector_position = selector->getPosition();
	sf::Vector2f selector_size = selector->getSize();
	//convert the selector position into array co-ordinates.
	auto atlas_element = interfaces.at("RIGHT_PANEL")->GetElement<GUILabel>("ATLAS_MAP");
	sf::Vector2f atlas_position = atlas_element->GetGlobalPosition();
	sf::Vector2f atlas_size = atlas_element->GetSize();

	auto atlas_sprite_dimensions = sf::Vector2i{ int(atlas_size.x / selector_size.x), int(atlas_size.y / selector_size.y) };
	auto selector_arr_position = sf::Vector2i{ int((selector_position.x - atlas_position.x) / selector_size.x), int((selector_position.y - atlas_position.y) / selector_size.y) };
	if (left) {
		selector_arr_position.x -= 1;
		if (selector_arr_position.x < 0) {
			if (selector_arr_position.y == 0) return;
			else {
				selector_arr_position.x = atlas_sprite_dimensions.x - 1;
				selector_arr_position.y -= 1;
			}
		}
	}
	else {
		selector_arr_position.x += 1;
		if (selector_arr_position.x > atlas_sprite_dimensions.x - 1) {
			if (selector_arr_position.y == atlas_sprite_dimensions.y - 1) return;
			else {
				selector_arr_position.x = 0;
				selector_arr_position.y += 1;
			}
		}
	}
	sf::Vector2i atlas_map_sprite_coord{ static_cast<int>(selector_arr_position.x * atlas_tile_size.x), static_cast<int>(selector_arr_position.y * atlas_tile_size.y) };
	interfaces.at("RIGHT_PANEL")->GetActiveStyle().ReadIn(STYLE_ATTRIBUTE::BG_TEXTURE_RECT, sf::IntRect{ std::move(atlas_map_sprite_coord), static_cast<sf::Vector2i>(atlas_tile_size) });
	selector->setPosition(sf::Vector2f{ static_cast<float>(selector_arr_position.x * selector_size.x),static_cast<float>(selector_arr_position.y * selector_size.y) } + atlas_position);
}

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
	interfaces["TOP_PANEL"] = guimgr->CreateInterfaceFromFile(GameStateData::GameStateType::LEVELEDITOR,  "Interface_StateLevelEditor_Top_Panel.txt");
	guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "SPRITESHEET_NAME", "POP_UP_PANEL" })->SetPredicates(static_cast<unsigned int>(Utility::CharacterCheckData::STRING_PREDICATE::FILE_NAME));
	guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "TEXTURE_X_TEXTFIELD", "POP_UP_PANEL" })->SetPredicates(static_cast<unsigned int>(Utility::CharacterCheckData::STRING_PREDICATE::NUMBER));
	guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "TEXTURE_Y_TEXTFIELD", "POP_UP_PANEL" })->SetPredicates(static_cast<unsigned int>(Utility::CharacterCheckData::STRING_PREDICATE::NUMBER));
	ActivatePopUp(nullptr);
	interfaces["RIGHT_PANEL"] = guimgr->CreateInterfaceFromFile(GameStateData::GameStateType::LEVELEDITOR, "Interface_StateLevelEditor_Right_Panel.txt");
	auto atlasmap = interfaces.at("RIGHT_PANEL")->GetElement<GUILabel>("ATLAS_MAP");
	//set up the atlas map selector.
	sf::RectangleShape selector;
	selector.setSize(atlasmap->GetSize());
	selector.setFillColor(sf::Color::Red);
	selector.setPosition(atlasmap->GetGlobalPosition());

	auto atlas_map_dimensions = sf::Vector2f{ 256,256 };
	auto atlas_element_dimensions = atlasmap->GetSize();

	sf::Vector2f scale{ atlas_element_dimensions.x / atlas_map_dimensions.x, atlas_element_dimensions.y / atlas_map_dimensions.y };
	atlas_tile_size = { 32,32 };
	selector.setSize(sf::Vector2f{ atlas_tile_size.x * scale.x, atlas_tile_size.y * scale.y });

	customshapes.at(0) = std::make_pair(true,std::make_unique<sf::RectangleShape>(selector));//make it invisible until the atlas map is loaded by the user.

	//interfaces.at("RIGHT_PANEL")->GetElement<GUILabel>("ACTIVE_TILE")->GetActiveStyle().ReadIn(STYLE_ATTRIBUTE::BG_TEXTURE_RECT, sf::IntRect{ sf::Vector2i{0,0}, static_cast<sf::Vector2i>(sf::Vector2f{32,32}) });
	LoadSheet("mytiles.png");
	DeactivatePopUp(nullptr);
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
						atlas_tile_size = sf::Vector2f{ texturexdimension, texturexdimension };
						//configure the selector size.
// 						customshapes.at(0).first = true; //set the selector to visible.
// 						auto atlas_map_dimensions = static_cast<sf::Vector2f>(atlasmap->getSize());
// 						auto selector = static_cast<sf::RectangleShape*>(customshapes.at(0).second.get());
// 						auto atlas_element_dimensions = selector->getSize();
// 						sf::Vector2f scale{ atlas_element_dimensions.x / atlas_map_dimensions.x, atlas_element_dimensions.y / atlas_map_dimensions.y };
// 						sf::Vector2f sprite_dimensions{ texturexdimension, textureydimension };
// 						selector->setSize(sf::Vector2f{ sprite_dimensions.x * scale.x, sprite_dimensions.y * scale.y });
					}
					
				}
			}
		}
	}
}



//AND || && SUPPORT TO EVENT MANAGER!!!!
void State_LevelEditor::MoveSelectorLeft(EventData::EventDetails* details){
	SetSelectorPosition(true);
}

void State_LevelEditor::MoveSelectorRight(EventData::EventDetails* details){//CHANGE EVENT MANAGER TO SUPPORT &| BINDINGS
	SetSelectorPosition(false);
}

bool State_LevelEditor::LoadSheet(const std::string& sheetname){
	atlasmap = guimgr->context->GetResourceManager<sf::Texture>()->RequestResource(sheetname);
	if (atlasmap != nullptr) {
		auto right_panel = interfaces.at("RIGHT_PANEL");
		auto atlas_map_elt = right_panel->GetElement<GUILabel>("ATLAS_MAP");

		atlas_map_elt->GetActiveStyle().ReadIn(STYLE_ATTRIBUTE::BG_TEXTURE_NAME, sheetname);
		atlas_map_elt->GetActiveStyle().ReadIn(STYLE_ATTRIBUTE::BG_TEXTURE_RECT, sf::IntRect{ sf::Vector2i{0,0}, static_cast<sf::Vector2i>(atlasmap->getSize()) });

		right_panel->GetElement<GUILabel>("ACTIVE_TILE")->GetActiveStyle().ReadIn(STYLE_ATTRIBUTE::BG_TEXTURE_NAME, sheetname);
		
		spritesheet_loaded = true;
		return true;
	}
	return false;
}
