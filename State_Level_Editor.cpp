#include "State_Level_Editor.h"
#include "Manager_GUI.h"
#include "GameStateData.h"
#include "Manager_Event.h"
#include "GUIButton.h"
#include "GUILabel.h"



State_LevelEditor::State_LevelEditor(Manager_State* statemgr, Manager_GUI* guimanager):State_Base(statemgr,guimanager){
	guimanager->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "Activate_Pop_Up", [this](EventDetails* details) {this->ActivatePopUp(details); });
	guimanager->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "Deactivate_Pop_Up", [this](EventDetails* details) {this->DeactivatePopUp(details); });
	guimanager->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "Confirm_Spritesheet", [this](EventDetails* details) {this->ConfirmButtonPopUp(details); });
	guimgr->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "Move_Selector_Left", [this](EventDetails* details) {this->MoveSelectorLeft(details); });
	guimgr->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "Move_Selector_Right", [this](EventDetails* details) {this->MoveSelectorRight(details); });
	guimgr->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "Deactivate_Atlas_Map", [this](EventDetails* details) {this->DeactivateAtlasMap(details); });
	guimgr->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "Activate_Atlas_Map", [this](EventDetails* details) {this->ActivateAtlasMap(details); });
	guimgr->GetContext()->eventmanager->RegisterBindingCallable(GameStateData::GameStateType::LEVELEDITOR, "Crop_Sprite", [this](EventDetails* details) {this->CropSprite(details); });
}
void State_LevelEditor::Draw(sf::RenderTarget& target){
	if (active_map != nullptr) {
		for (auto& registered_tile : active_map->registered_tiles) {
			auto unavailable_texture_position = static_cast<sf::Vector2f>(ArrayToAtlasCoord(registered_tile.first.master_tile_reference));
			unavailable_texture_position.x *= tile_selector.getSize().x;
			unavailable_texture_position.y *= tile_selector.getSize().y;
			unavailable_texture_position += GetInterface<RIGHT_PANEL>()->GetElement<GUILabel>("ATLAS_VIEW")->GetGlobalPosition();
			unavailable_tile_texture.setPosition(std::move(unavailable_texture_position));
			target.draw(unavailable_tile_texture);
		}
	}
	target.draw(tile_selector);
	
	//draw contiguous rectangles from 0 : 75 x && y

// 	float win_scale{ 1 };
// 	auto win_size = sf::Vector2f{ static_cast<float>(target.getSize().x * win_scale), static_cast<float>(target.getSize().y * win_scale) };
// 	int n_columns = 20;
// 	int n_rows = 20;
// 	auto rect_size = sf::Vector2f{win_size.x / n_columns, win_size.y / n_rows};
// 	sf::RectangleShape s;
// 	s.setSize(rect_size);
// 	s.setFillColor(sf::Color::Color(200, 200, 200, 255));
// 	s.setOutlineThickness(2);
// 	s.setOutlineColor(sf::Color::Color(0, 0, 0, 100));
// 	for (int y = 0; y < n_rows; ++y) {
// 		for (int x = 0; x < n_columns; ++x) {
// 			//find the screen x and y position.
// 			sf::Vector2f worldposition{ x * rect_size.x, y * rect_size.y };
// 			s.setPosition(worldposition);
// 			target.draw(s);
// 		}
// 	}

	
	
	//Determine the empty_tile size based on the current window scale.
	auto win = static_cast<sf::RenderWindow*>(&target);

	//we need to be able to limit the zoom such that the maximum zoom out will accomodate 256 x 256 tiles.
	//Find the zoom at which 256 tiles is fit in.
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		auto v = win->getView();
		v.zoom(1.00001);
		win->setView(std::move(v));
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		auto v = win->getView();
			v.zoom(0.99999);
			win->setView(std::move(v));
	}
	//Find the zoom scale of the window.
	auto win_view_size = win->getView().getSize();
	auto win_size = win->getSize();
	sf::Vector2f scale{ win_size.x / win_view_size.x, win_size.y/win_view_size.y };

	//Multiply the scale_x and scale_y by the empty_rect_size
	empty_tile.setSize(sf::Vector2f{ empty_tile.getSize().x * scale.x, empty_tile.getSize().y * scale.y });
	//Iterate through the available canvas, drawing as many rects as possible.
	
	float view_space_proportion{ 0.75 };
	sf::Vector2f empty_tile_size{ empty_tile.getSize() };
	for (float posy = 0; posy <= win_size.y * view_space_proportion; posy += empty_tile_size.y) {
		for (float posx = 0; posx <= win_size.x * view_space_proportion; posx += empty_tile_size.x) {
			empty_tile.setPosition(sf::Vector2f{ posx,posy });
			target.draw(empty_tile);
		}
	}



}
void State_LevelEditor::Update(const float& dT){

}
void State_LevelEditor::Activate(){

}
void State_LevelEditor::Deactivate(){
}
void State_LevelEditor::OnCreate(){
	//Create the interfacess.
	interfaces.at(static_cast<unsigned int>(STATEINTERFACES::BOT_PANEL)) = guimgr->CreateInterfaceFromFile(GameStateData::GameStateType::LEVELEDITOR, "Interface_StateLevelEditor_Bot_Panel.txt");
	interfaces.at(static_cast<unsigned int>(STATEINTERFACES::POP_UP_PANEL)) = guimgr->CreateInterfaceFromFile(GameStateData::GameStateType::LEVELEDITOR,  "Interface_StateLevelEditor_PopUp_Panel.txt");
	interfaces.at(static_cast<unsigned int>(STATEINTERFACES::TOP_PANEL)) = guimgr->CreateInterfaceFromFile(GameStateData::GameStateType::LEVELEDITOR,  "Interface_StateLevelEditor_Top_Panel.txt");
	interfaces.at(static_cast<unsigned int>(STATEINTERFACES::RIGHT_PANEL)) = guimgr->CreateInterfaceFromFile(GameStateData::GameStateType::LEVELEDITOR, "Interface_StateLevelEditor_Right_Panel.txt");
	//Configure the textfield predicates.
	guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "SPRITE_SHEET_NAME", "POP_UP_PANEL" })->SetPredicates(static_cast<unsigned int>(Utility::CharacterCheckData::STRING_PREDICATE::FILE_NAME));
	guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "TEXTURE_X_FIELD", "POP_UP_PANEL" })->SetPredicates(static_cast<unsigned int>(Utility::CharacterCheckData::STRING_PREDICATE::NUMBER));
	guimgr->GetElement<GUITextfield>(GameStateType::LEVELEDITOR, { "TEXTURE_Y_FIELD", "POP_UP_PANEL" })->SetPredicates(static_cast<unsigned int>(Utility::CharacterCheckData::STRING_PREDICATE::NUMBER));
	guimgr->SetActiveInterfacesEnable(GetInterface<POP_UP_PANEL>(), false);
	tile_selector.setPosition(GetInterface<RIGHT_PANEL>()->GetElement<GUILabel>("ATLAS_VIEW")->GetGlobalPosition());
	tile_selector.setFillColor(sf::Color::Transparent);
	tile_selector.setOutlineColor(sf::Color::Green);
	tile_selector.setOutlineThickness(2);
	unavailable_tile_texture.setFillColor(sf::Color::Color(0, 0, 0, 100));
	empty_tile.setFillColor(sf::Color::Color(200, 200, 200, 255));
	empty_tile.setOutlineColor(sf::Color::Color(0, 0, 0, 90));
	empty_tile.setOutlineThickness(2);
	empty_tile.setSize(sf::Vector2f{ 32,32 });
}
void State_LevelEditor::OnDestroy(){

}
void State_LevelEditor::UpdateCamera()
{

}
void State_LevelEditor::Continue()
{

}
UserMap* State_LevelEditor::CreateNewMap(const std::string& map_name, const sf::Vector2i& tiledimensions, std::shared_ptr<sf::Texture> tile_sheet){
	auto new_map = std::make_unique<UserMap>();
	new_map->tile_sheet = tile_sheet;
	sf::Vector2f atlas_view_size = GetInterface<RIGHT_PANEL>()->GetElement<GUILabel>("ATLAS_VIEW")->GetSize();
	auto atlas_actual_size = static_cast<sf::Vector2f>(tile_sheet->getSize());
	new_map->tile_dimensions = tiledimensions;
	new_map->tile_sheet = std::move(tile_sheet);
	auto ptr = new_map.get();
	user_maps[map_name] = std::move(new_map);
	active_map = ptr;
	return ptr;
}
void State_LevelEditor::ActivatePopUp(EventData::EventDetails* details){
	GUIInterface* panel = guimgr->GetInterface(GameStateData::GameStateType::LEVELEDITOR, "POP_UP_PANEL");
	panel->SetHidden(false);
	guimgr->SetActiveInterfacesEnable(panel, false);
}
void State_LevelEditor::DeactivatePopUp(EventData::EventDetails* details) {
	GetInterface<POP_UP_PANEL>()->SetHidden(true);
	guimgr->SetActiveInterfacesEnable(GetInterface<POP_UP_PANEL>(), true);
}
void State_LevelEditor::ConfirmButtonPopUp(EventData::EventDetails* details){
	//Check if the user map name has not already been taken.
	std::string map_name;
	if (user_maps.find(map_name) != user_maps.end()) return;
	//Grab the user input from the textfields and check if both dimensions lie below the MAX_TILE_DIMENSION
	auto texture_x_field = GetInterface<POP_UP_PANEL>()->GetElement<GUITextfield>("TEXTURE_X_FIELD");
	if (std::string texture_x_field_string = texture_x_field->GetTextfieldString(); !texture_x_field_string.empty() && texture_x_field_string != texture_x_field->GetDefaultTextfieldString()) {
		if (auto texture_x_dimension = std::stoi(texture_x_field_string); texture_x_dimension <= MAX_TILE_DIMENSION) {
			auto texture_y_field = GetInterface<POP_UP_PANEL>()->GetElement<GUITextfield>("TEXTURE_Y_FIELD");
			if (auto texture_y_field_string = texture_y_field->GetTextfieldString();  !texture_y_field_string.empty() && texture_y_field_string != texture_y_field->GetDefaultTextfieldString()) {
				if (auto texture_y_dimension = std::stoi(texture_y_field_string); texture_y_dimension <= MAX_TILE_DIMENSION) {
					std::string atlas_map_name = GetInterface<POP_UP_PANEL>()->GetElement<GUITextfield>("SPRITE_SHEET_NAME")->GetTextfieldString();
					if (atlas_map_name.empty()) return;
					if (std::shared_ptr<sf::Texture> atlas_sheet = LoadSheet(atlas_map_name); atlas_sheet != nullptr) {
						UserMap* newmap = CreateNewMap(map_name, sf::Vector2i{ std::move(texture_x_dimension), std::move(texture_y_dimension) },std::move(atlas_sheet));
						//Configure the tile selector sizes based on the user input.
						//Atlas map is scaled to fit the ATLAS_VIEW element. We must find the tile dimension relative to this element -> this is the selector size.
						sf::Vector2f atlas_view_size{ GetInterface<RIGHT_PANEL>()->GetElement<GUILabel>("ATLAS_VIEW")->GetSize() };
						auto atlas_actual_size = static_cast<sf::Vector2i>(active_map->tile_sheet->getSize());
						tile_selector.setSize(sf::Vector2f{ (atlas_view_size.x / atlas_actual_size.x) * active_map->tile_dimensions.x, (atlas_view_size.y / atlas_actual_size.y) * active_map->tile_dimensions.y });
						unavailable_tile_texture.setSize(tile_selector.getSize());
						DeactivatePopUp(details);
						DeactivateAtlasMap(nullptr);
						MoveSelectorRight(nullptr);
						//configure the selector size.
					}
				}
			}
		}
	}
}
//we need to toggle 
//AND || && SUPPORT TO EVENT MANAGER!!!!
void State_LevelEditor::MoveSelectorLeft(EventData::EventDetails* details){
	if (GetInterface<RIGHT_PANEL>()->GetElement<GUILabel>("ATLAS_VIEW")->GetActiveState() != GUIState::NEUTRAL) SetSelectorPosition(true);
}

sf::Vector2i State_LevelEditor::GetSelectorAtlasCoords() {
	auto atlas_view = GetInterface<RIGHT_PANEL>()->GetElement<GUILabel>("ATLAS_VIEW");
	return sf::Vector2i{ static_cast<int>((tile_selector.getPosition().x - atlas_view->GetGlobalPosition().x) / tile_selector.getSize().x), static_cast<int>((tile_selector.getPosition().y - atlas_view->GetGlobalPosition().y) / tile_selector.getSize().y) };
}
void State_LevelEditor::SetSelectorPosition(const bool& left) {
	//ATLAS_VIEW can only be manipulated when it has been clicked by the user.
	auto ATLAS_VIEW = GetInterface<RIGHT_PANEL>()->GetElement<GUILabel>("ATLAS_VIEW");
	if (ATLAS_VIEW->GetActiveState() == GUIState::NEUTRAL) return;
	//Limit the speed as the cycle speed is too fast.
	{
		sf::Clock c;
		while (c.getElapsedTime().asSeconds() < 0.17) {}
	}
	//Convert the tile_selector position into tile_sheet array co-ordinates.
	auto atlas_max_tile_dimensions = sf::Vector2i{ static_cast<int>(ATLAS_VIEW->GetSize().x / tile_selector.getSize().x), static_cast<int>(ATLAS_VIEW->GetSize().y / tile_selector.getSize().y) };
	auto tile_selector_atlas_pos = GetSelectorAtlasCoords();
	//Move the selector left or right, and wrap it such that it traverses the atlas map in a contiguous fashion.
	if (left) {
		tile_selector_atlas_pos.x -= 1;
		if (tile_selector_atlas_pos.x < 0) {
			if (tile_selector_atlas_pos.y == 0) return; //At top left of atlas map.
			else {
				tile_selector_atlas_pos.x = atlas_max_tile_dimensions.x - 1;
				tile_selector_atlas_pos.y -= 1;
			}
		}
	}
	else {
		tile_selector_atlas_pos.x += 1;
		if (tile_selector_atlas_pos.x > atlas_max_tile_dimensions.x - 1) {
			if (tile_selector_atlas_pos.y == atlas_max_tile_dimensions.y - 1) return; //At bottom right of atlas map.
			else {
				tile_selector_atlas_pos.x = 0;
				tile_selector_atlas_pos.y += 1;
			}
		}
	}


	//Change the texture rect of the ACTIVE_TILE element to reflect the tile which lies underneath the selector.
	sf::Vector2i atlas_sheet_coords{ static_cast<int>(tile_selector_atlas_pos.x * active_map->tile_dimensions.x), static_cast<int>(tile_selector_atlas_pos.y * active_map->tile_dimensions.y) };
	GetInterface<RIGHT_PANEL>()->GetElement<GUILabel>("ACTIVE_TILE")->GetActiveStyle().ReadIn(STYLE_ATTRIBUTE::BG_TEXTURE_RECT, sf::IntRect{ std::move(atlas_sheet_coords), active_map->tile_dimensions });
	tile_selector.setPosition(sf::Vector2f{ static_cast<float>(tile_selector_atlas_pos.x * tile_selector.getSize().x), static_cast<float>(tile_selector_atlas_pos.y * tile_selector.getSize().y) } + ATLAS_VIEW->GetGlobalPosition());

	//Eye candy : change the color of the selector based on whether it has already been cropped or not.
	unsigned int selector_array_position = AtlasCoordToArray(tile_selector_atlas_pos);
	//Check if the array_position has been registered as a TileID already.
	auto tile_registered = std::find_if(active_map->registered_tiles.begin(), active_map->registered_tiles.end(), [&selector_array_position](const std::pair<DefaultProperties, Tile>& p) {
		return p.first.master_tile_reference == selector_array_position;
		});
	if (tile_registered == active_map->registered_tiles.end()) tile_selector.setOutlineColor(sf::Color::Green);
	else tile_selector.setOutlineColor(sf::Color::Red);
}
void State_LevelEditor::MoveSelectorRight(EventData::EventDetails* details){//CHANGE EVENT MANAGER TO SUPPORT &| BINDINGS
	if (GetInterface<RIGHT_PANEL>()->GetElement<GUILabel>("ATLAS_VIEW")->GetActiveState() != GUIState::NEUTRAL) SetSelectorPosition(false);
}
std::shared_ptr<sf::Texture> State_LevelEditor::LoadSheet(const std::string& sheetname){
	std::shared_ptr<sf::Texture> atlas_map_ptr = guimgr->context->GetResourceManager<sf::Texture>()->RequestResource(sheetname);
	if (atlas_map_ptr == nullptr) return nullptr;
	auto atlas_view = GetInterface<RIGHT_PANEL>()->GetElement<GUILabel>("ATLAS_VIEW");
	for (int i = 0; i < 3; ++i) {
		atlas_view->GetStyle(static_cast<GUIState>(i)).ReadIn(STYLE_ATTRIBUTE::BG_TEXTURE_NAME, sheetname);
		atlas_view->GetStyle(static_cast<GUIState>(i)).ReadIn(STYLE_ATTRIBUTE::BG_TEXTURE_RECT, sf::IntRect{ sf::Vector2i{0,0}, static_cast<sf::Vector2i>(atlas_map_ptr->getSize()) });
	}
	GetInterface<RIGHT_PANEL>()->GetElement<GUILabel>("ACTIVE_TILE")->GetActiveStyle().ReadIn(STYLE_ATTRIBUTE::BG_TEXTURE_NAME, sheetname);
	return atlas_map_ptr;
}
void State_LevelEditor::ActivateAtlasMap(EventData::EventDetails* details){
	SetSelectorPosition(true);
	SetSelectorPosition(false);
}
void State_LevelEditor::DeactivateAtlasMap(EventData::EventDetails* details){
	tile_selector.setOutlineColor(sf::Color::Transparent);
}
void State_LevelEditor::CropSprite(EventData::EventDetails* details){
	//If the color of the selector is red, then the tile has been cropped already.
	if (tile_selector.getOutlineColor() == sf::Color::Red) return;
	 //Transform the selector co-ordinates into atlas map array co-ordinates.
	auto tile_sheet_size = static_cast<sf::Vector2i>(active_map->tile_sheet->getSize());
	sf::Vector2i atlas_unit_dimensions{ static_cast<int>(tile_sheet_size.x / active_map->tile_dimensions.x), tile_sheet_size.y / active_map->tile_dimensions.y };
	sf::Vector2i selector_atlas_coords = GetSelectorAtlasCoords();
	auto selector_array_coords = AtlasCoordToArray(selector_atlas_coords);
	
	Tile x(active_map->tile_sheet.get(), sf::IntRect{ sf::Vector2i{selector_atlas_coords.x * active_map->tile_dimensions.x, selector_atlas_coords.y * active_map->tile_dimensions.y}, active_map->tile_dimensions });
	active_map->registered_tiles.emplace_back(std::make_pair(DefaultProperties(std::move(selector_array_coords)), std::move(x)));
	//Tile has been successfully cropped.
	tile_selector.setOutlineColor(sf::Color::Red);
}
void State_LevelEditor::UnCropSprite(EventData::EventDetails* details){

}
