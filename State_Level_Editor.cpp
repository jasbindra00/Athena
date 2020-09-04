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
bool State_LevelEditor::ValidatePopUpTextfields()
{
	return true;

}



bool State_LevelEditor::CreateNewMap(sf::Vector2u&& map_dimensions, sf::Vector2f&& tile_pixel_dimensions, std::string&& tile_sheet_name)
{
	//Try loading in the texture.
	SharedTexture res = guimgr->GetContext()->texturemgr->RequestResource(tile_sheet_name);
	if (!res) return false;
	//Set the tile sheet viewer texture to this new texture.
	GetInterface(RIGHT_PANEL)->GetElement<GUILabel>("TILE_SHEET_VIEW")->GetActiveStyle().ReadIn(STYLE_ATTRIBUTE::BG_TEXTURE_NAME, std::move(tile_sheet_name));
	//Create a new map.
	active_map = std::make_unique<MapData::MapImpl>(std::move(map_dimensions), std::move(tile_pixel_dimensions), std::move(res));
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
	if (details->keycode == sf::Keyboard::LControl) { //ZOOM
		(scroll_upwards == true) ? state_view.zoom(0.99) : state_view.zoom(1.01);
		return;
	}
	auto scroll_offset = static_cast<float>(SCROLL_AMOUNT);
	if (scroll_upwards) scroll_offset *= -1;
	switch (details->keycode) {
	case sf::Keyboard::Key::LAlt: {state_view.move(sf::Vector2f{ std::move(scroll_offset), 0 }); break; } //HORIZONTAL SCROLL
	case sf::Keyboard::Key::LShift: {state_view.move(0, std::move(scroll_offset)); break; } //VERTICAL SCROLL
	}
}

void State_LevelEditor::TogglePopUp()
{

}

void State_LevelEditor::PlaceTile()
{

}

State_LevelEditor::State_LevelEditor(Manager_State* smgr, Manager_GUI* gmgr)
	:State_Base(smgr,gmgr)
{
	state_view = gmgr->GetContext()->window->GetRenderWindow()->getDefaultView();

}

void State_LevelEditor::Draw(sf::RenderTarget& target)
{
	if (active_map == nullptr) return;
	
	/*
	-We only want to draw the tiles which are visible within the view space.
	-So, convert the view space top left and bottom right co-ordinates to map-coordinates to see which tile to start and end at.
	-We 
	*/

	auto LimitCoords = [](sf::Vector2u v, const sf::Vector2u& map_tile_dimensions) {
		if (v.x < 0) v.x = 0;
		else if (v.x >= map_tile_dimensions.x) v.x = map_tile_dimensions.x;
		if (v.y < 0) v.y = 0;
		else if (v.y > map_tile_dimensions.y) v.y = map_tile_dimensions.y;
		return v;
	};
	sf::Vector2f half_size{ state_view.getSize().x / 2, state_view.getSize().y / 2 };
	sf::Vector2f tl_f = state_view.getCenter() - half_size;
	if (tl_f.x < 0)tl_f.x = 0;
	if (tl_f.y < 0) tl_f.y = 0;
	sf::Vector2f br_f = state_view.getCenter() + half_size;
	if (br_f.x < 0)br_f.x = 0;
	if (br_f.y < 0) br_f.y = 0;

	sf::Vector2u view_space_top_left = LimitCoords(MapData::WorldToMapCoords(tl_f), active_map->map_tile_dimensions);
	sf::Vector2u view_space_bottom_right = LimitCoords(MapData::WorldToMapCoords(br_f), active_map->map_tile_dimensions);
	for (unsigned int y = view_space_top_left.y; y < view_space_bottom_right.y; ++y) {
		for (unsigned int x = view_space_top_left.x; x < view_space_bottom_right.x; ++x) {
			empty_tile.setPosition(MapData::MapToWorldCoords({ x, y }));
			target.draw(empty_tile);
		}
	}
}

void State_LevelEditor::Update(const float& dT)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		state_view.move(sf::Vector2f{ 0,1 });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		state_view.move(sf::Vector2f{ -1,0 });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		state_view.move(sf::Vector2f{ 0,-1 });
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		state_view.move(sf::Vector2f{ 1,0 });
	}
	
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
	event_manager->RegisterBindingCallable(GameStateType::LEVELEDITOR, "Zoom", [this](EventData::EventDetails* details) {this->Scroll(std::move(details)); });
	event_manager->RegisterBindingCallable(GameStateType::LEVELEDITOR, "Place_Tile", [this](EventData::EventDetails* details) {this->PlaceTile(); });
	//*** ***

	interfaces.at(RIGHT_PANEL)->GetElement<GUIElement>("TILE_SHEET_VIEW")->SetLayerType(GUIData::GUILayerType::BACKGROUND);
	
	state_view.setViewport(sf::FloatRect{ 0,0,0.8,0.8 });

	empty_tile.setSize(sf::Vector2f{ 32,32 });
	empty_tile.setFillColor(sf::Color::Color(190, 190, 190, 255));
	empty_tile.setOutlineColor(sf::Color::Black);
	empty_tile.setOutlineThickness(3);

	//Debugging purposes, default load in the tile_sheet
	std::chrono::system_clock::time_point  start, end;
	start = std::chrono::system_clock::now();
	CreateNewMap({ 10000,10000 }, { 32,32 }, "mytiles.png");	
	end = std::chrono::system_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms\n";
}

void State_LevelEditor::OnDestroy()
{

}

void State_LevelEditor::Continue()
{

}
