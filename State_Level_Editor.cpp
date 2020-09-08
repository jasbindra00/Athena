#include "State_Level_Editor.h"
#include "EventData.h"
#include "Manager_GUI.h"
#include "GUILabel.h"
#include "GUITextfield.h"
#include "Manager_Event.h"
#include "TileMap.h"
#include "GUIInterface.h"
#include "Window.h"
#include <chrono>
State_LevelEditor::State_LevelEditor(Manager_State* smgr, Manager_GUI* gmgr)
	:State_Base(smgr, gmgr)
{
	state_view = gmgr->GetContext()->window->GetRenderWindow()->getDefaultView();

}

void State_LevelEditor::OnCreate()
{
	//***Build the interface for this state.***
	using namespace GameStateData;
	interfaces.at(BOT_PANEL) = guimgr->CreateInterfaceFromFile(GameStateType::LEVELEDITOR, "Interface_StateLevelEditor_Bot_Panel.txt");
	interfaces.at(POP_UP_PANEL) = guimgr->CreateInterfaceFromFile(GameStateType::LEVELEDITOR, "Interface_StateLevelEditor_PopUp_Panel.txt");
	interfaces.at(RIGHT_PANEL) = guimgr->CreateInterfaceFromFile(GameStateType::LEVELEDITOR, "Interface_StateLevelEditor_Right_Panel.txt");
	interfaces.at(TOP_PANEL) = guimgr->CreateInterfaceFromFile(GameStateType::LEVELEDITOR, "Interface_StateLevelEditor_Top_Panel.txt");
	//*** ***

	//**Register the binding callables for this state.***
	Manager_Event* event_manager = guimgr->GetContext()->eventmanager;
	event_manager->RegisterBindingCallable(GameStateType::LEVELEDITOR, "Load_Map", [this](EventData::EventDetails* details) {this->TogglePopUp(); });
	event_manager->RegisterBindingCallable(GameStateType::LEVELEDITOR, "Horizontal_Scroll", [this](EventData::EventDetails* details) {this->Scroll(std::move(details)); });
	event_manager->RegisterBindingCallable(GameStateType::LEVELEDITOR, "Vertical_Scroll", [this](EventData::EventDetails* details) {this->Scroll(std::move(details)); });
	event_manager->RegisterBindingCallable(GameStateType::LEVELEDITOR, "Zoom_Scroll", [this](EventData::EventDetails* details) {this->Scroll(std::move(details)); });
	event_manager->RegisterBindingCallable(GameStateType::LEVELEDITOR, "Selector_Scroll", [this](EventData::EventDetails* details) {this->Scroll(std::move(details)); });
	event_manager->RegisterBindingCallable(GameStateType::LEVELEDITOR, "Place_Tile", [this](EventData::EventDetails* details) {this->PlaceTile(); });
	//*** ***

	interfaces.at(RIGHT_PANEL)->GetElement<GUIElement>("TILE_SHEET_VIEW")->SetLayerType(GUIData::GUILayerType::BACKGROUND);

	state_view.setViewport(sf::FloatRect{ 0,0,0.8,0.8 });	

	//Debugging purposes, default load in the tile_sheet

	CreateNewMap({ 5,5 }, { 32,32 }, "mytiles.png");

}

bool State_LevelEditor::ValidatePopUpTextfields()
{
	return true;
}

bool State_LevelEditor::CreateNewMap(sf::Vector2u&& map_dimensions, sf::Vector2u&& tile_pixel_dimensions, std::string&& tile_sheet_name)
{
	//Try loading in the texture.
	SharedTexture res = guimgr->GetContext()->texturemgr->RequestResource(tile_sheet_name);
	if (!res) return false;
	//Set the tile sheet viewer texture to this new texture.
	GUIInterface* right_panel = GetInterface(RIGHT_PANEL);
	right_panel->GetElement<GUILabel>("TILE_SHEET_VIEW")->GetActiveStyle().ReadIn(STYLE_ATTRIBUTE::BG_TEXTURE_NAME, std::move(tile_sheet_name));
	map_selector.OnCreate(map_dimensions, sf::Vector2f{ 32,32 }, sf::Vector2f{ 0,0 });
	//Create a new map.
	active_map = std::make_unique<MapData::MapImpl>(std::move(map_dimensions), std::move(tile_pixel_dimensions), std::move(res),guimgr->GetContext()->texturemgr);
	tile_selector.OnCreate(active_map->atlas_tile_dimensions, sf::Vector2f{ 32,32 }, right_panel->GetElement<GUILabel>("TILE_SHEET_VIEW")->GetGlobalPosition() - right_panel->GetGlobalPosition());
	return true;
}
void State_LevelEditor::SaveMap()
{

}

void State_LevelEditor::ConfirmButtonPopup()
{

}

void State_LevelEditor::MoveCamera(EventData::EventDetails* details)
{

}

void State_LevelEditor::Scroll(EventData::EventDetails* details)
{
	//Find the scroll direction.
	bool scroll_upwards = (details->mousewheeldelta < 0)? false : true;
	
	//Holding different keys down while scrolling will do different things.
	switch (details->keycode) {
	case sf::Keyboard::Key::LControl: { //ZOOM
		(scroll_upwards == true) ? state_view.zoom(1 - ZOOM_AMOUNT) : state_view.zoom(1 + ZOOM_AMOUNT);
		break;
	}
	case sf::Keyboard::Key::LAlt: {//HORIZONTAL SCROLL
		(scroll_upwards == true) ? state_view.move(sf::Vector2f{ -SCROLL_AMOUNT, 0 }) : state_view.move(sf::Vector2f{ SCROLL_AMOUNT,0 });
		
		break;
	}
	case sf::Keyboard::Key::LShift: { //VERTICAL SCROLL
		(scroll_upwards == true) ? state_view.move(sf::Vector2f{ 0, -SCROLL_AMOUNT }) : state_view.move(sf::Vector2f{ 0,SCROLL_AMOUNT });
		break;
	}
	case sf::Keyboard::Key::Tab: { //TILE_SELECTOR SCROLL
		
		break;
	}
							   
	}

}

void State_LevelEditor::TogglePopUp()
{

}

void State_LevelEditor::PlaceTile()
{
	/*
	-Check if the current selector tile on the atlas view has been registered or not.
		-If not, register the tile.
		-The tile ID of this tile is its array co-ordinates in the atlas map.

		-At this point, the tile is registered by nature, or by call.
		-Place the tile onto the atlas map:
			-Pull the texture of the active tile, using the selector atlas co-ordinates.
			-Need a way to differentiate between a tile_map texturised or not.
				-An empty tile will have a tile ID of default init.
					-Now, we can simply populate these empty tiles by using the registered tile map.
			-Don't place a tile if the tileID is not default.
	*/
	
	//Convert the tile selector position into array coordinates and check if it has been registered.
	//Obtain the local position of the selector relative to the atlas map, and try registering the tile.

	active_map->RegisterTile(MapData::WorldToMapCoords(tile_selector.GetWorldPosition() - GetInterface(RIGHT_PANEL)->GetElement<GUILabel>("TILE_SHEET_VIEW")->GetGlobalPosition(), active_map->tile_pixel_dimensions));
	
	//Now we must place the tile onto the actual game map.

	active_map->PlaceTile(map_selector.GetLocalPosition(), tile_selector.GetLocalPosition());
}

void State_LevelEditor::MoveSelector(EventData::EventDetails* details)
{
		auto ConvertKeyToDirection = [details]()->sf::Vector2u {
			auto& keycode = details->keycode;
			if (keycode == sf::Keyboard::Key::Up || keycode == sf::Keyboard::Key::W) return { 0,-1 };
			else if (keycode == sf::Keyboard::Key::Left || keycode == sf::Keyboard::Key::A) return { -1,0 };
			else if (keycode == sf::Keyboard::Key::Down || keycode == sf::Keyboard::Key::S) return { 0,1 };
			else if (keycode == sf::Keyboard::Key::Right || keycode == sf::Keyboard::Key::D) return { 1,0 };
		};
		auto dir = ConvertKeyToDirection();
		





	switch (details->keycode) {
	case sf::Keyboard::Key::W: { //MOVE MAP SELECTOR
		map_selector.Move({ 0,-1 });
		break;
	}
	case sf::Keyboard::Key::A: { //MOVE MAP SELECTOR
		map_selector.Move({ -1,0 });
		break;
	}
	case sf::Keyboard::Key::S: { //MOVE MAP SELECTOR
		map_selector.Move({ 0,1 });
		break;
	}
	case sf::Keyboard::Key::D: {//MOVE MAP SELECTOR
		map_selector.Move({ 1,0 });
		break;
	}
	case sf::Keyboard::Key::Up: {
		tile_selector.Move({ 0,-1 });
		break;
	}
	case sf::Keyboard::Key::Right: {
		tile_selector.Move({ 1,0 });
		break;
	}
	case sf::Keyboard::Key::Down: {
		tile_selector.Move({ 0,1 });
		break;
	}
	case sf::Keyboard::Key::Left: {
		tile_selector.Move({ -1,0 });
		break;
	}
	}
}

void State_LevelEditor::Draw(sf::RenderTarget& target)
{
	
	if (active_map == nullptr) return;
	//Draw the user map.
	auto& default_tile_sprite = active_map->master_tiles.at(UINT_MAX).tile_sprite;
	//Culling -> we only really want to draw the tiles that our view intersects, and nothing more.
	auto view_aabb = GetStateViewAABB();
	if (view_aabb.left < 0) view_aabb.left = 0;
	if (view_aabb.top < 0) view_aabb.top = 0;
	sf::Vector2u view_space_top_left = MapData::LimitMapCoords(MapData::WorldToMapCoords(sf::Vector2f{ view_aabb.left, view_aabb.top }), active_map->map_tile_dimensions);
	sf::Vector2u view_space_bottom_right = MapData::LimitMapCoords(MapData::WorldToMapCoords(sf::Vector2f{ view_aabb.left + std::move(view_aabb.width),view_aabb.left + std::move(view_aabb.height) }), active_map->map_tile_dimensions);
	for (unsigned int y = view_space_top_left.y; y < view_space_bottom_right.y; ++y) {
		for (unsigned int x = view_space_top_left.x; x < view_space_bottom_right.x; ++x) {
			sf::Vector2f map_position = MapData::MapToWorldCoords({ x,y });
			//Draw the default_tile
			default_tile_sprite.setPosition(map_position);
			target.draw(default_tile_sprite);
			//Obtain the user tile at this current position
			auto& user_tile = active_map->worker_tiles.at(MapData::MapToArrayCoords({ x,y }, active_map->map_tile_dimensions));
			if (user_tile.master_id == UINT_MAX) continue;
			active_map->master_tiles.at(user_tile.master_id).tile_sprite.setPosition(MapData::MapToWorldCoords({ x,y }));
			target.draw(active_map->master_tiles.at(user_tile.master_id).tile_sprite);
		}
	}
	map_selector.Render(target);
	GetInterface(RIGHT_PANEL)->GetElement<GUILabel>("TILE_SHEET_VIEW")->GetVisual().RenderWithDrawables({ tile_selector.GetSelectorDrawable() });
	
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

void State_LevelEditor::UpdateCamera()
{

}

void State_LevelEditor::OnDestroy()
{

}

void State_LevelEditor::Continue()
{

}
