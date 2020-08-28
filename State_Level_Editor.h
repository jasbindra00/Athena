#ifndef STATE_LEVEL_EDITOR_H
#define STATE_LEVEL_EDITOR_H
#include "State_Base.h"
#include "GUIInterface.h"
#include "EventData.h"
#include "GUILayerData.h"


using TileID = unsigned int;
struct Tile {
	std::shared_ptr<sf::Texture> atlas_map;
	sf::Sprite tile_sprite;
	sf::IntRect texture_rect;
	Tile(sf::Texture* texture, const sf::IntRect& texture_rect) {
		tile_sprite.setTexture(*texture);
		tile_sprite.setTextureRect(texture_rect);
		//we need access to the texture to initialise.
		//but this may 
	}

};
struct TileProperties {
	TileProperties(const TileID& id):master_tile_reference(id) {

	}
	TileID master_tile_reference;
	sf::Vector2f friction;
	bool is_teleport{ false };
	bool is_deadly{ false };
	int layer_number{ 0 };
};

using DefaultProperties = TileProperties;
struct UserMap {
	std::shared_ptr<sf::Texture> tile_sheet;
	sf::Vector2i tile_dimensions;
	sf::Vector2u map_dimensions;
	std::vector<std::pair<DefaultProperties, Tile>> registered_tiles;
	UserMap(const sf::Vector2u& m_d, const sf::Vector2i& t_d, std::shared_ptr<sf::Texture> t_s):map_dimensions(m_d), tile_dimensions(t_d), tile_sheet(std::move(t_s)) {

	}

	//tile id will be the array position on the map.
};

using UserMapPtr = std::unique_ptr<UserMap>;
class Manager_State;

class State_LevelEditor : public State_Base {
	const static unsigned int MAX_TILE_DIMENSION{ 32 };
	const static unsigned int MAX_MAP_SIZE_X{ 256 };
	const static unsigned int MAX_MAP_SIZE_Y{ 256 };
	sf::Vector2i GetSelectorAtlasCoords();
	sf::Vector2i GetUnitAtlasSize() const{
		auto sheet_size = static_cast<sf::Vector2i>(active_map->tile_sheet->getSize());
		return sf::Vector2i { static_cast<int>(sheet_size.x / active_map->tile_dimensions.x), static_cast<int>(sheet_size.y / active_map->tile_dimensions.y) };
	}
	unsigned int AtlasCoordToArray(const sf::Vector2i& coord)const {
		return coord.y * GetUnitAtlasSize().x + coord.x;
	}
	sf::Vector2i ArrayToAtlasCoord(const unsigned int& arr_coord) const{
		
		sf::Vector2i unit_atlas_size{ GetUnitAtlasSize() };
		return sf::Vector2i{ static_cast<int>(arr_coord % unit_atlas_size.x), static_cast<int>(arr_coord / unit_atlas_size.y) };
	}
private:
	enum STATEINTERFACES {
		TOP_PANEL, POP_UP_PANEL, RIGHT_PANEL, BOT_PANEL
	};
	//Map canvas data
	sf::RenderTexture map_screen;
	sf::View map_view;
	sf::Sprite map_sprite;
	float x_off = 0; 
	float y_off = 0;
	unsigned int inc = 10;

	GUILayerData::GUILayerPtr map_canvas;





	sf::RectangleShape tile_selector;
	sf::RectangleShape empty_tile;



	UserMap* active_map;
	//spritesheet loaded by the size of user_maps
	std::unordered_map<std::string, std::unique_ptr<UserMap>> user_maps;
	std::array<GUIInterface*, 4> interfaces;


	template<STATEINTERFACES INTERFACE>
	GUIInterface* GetInterface() {
		return interfaces.at(static_cast<int>(INTERFACE));
	}

	void SetSelectorPosition(const bool& left);
	
public:
	State_LevelEditor(Manager_State* statemgr, Manager_GUI* guimgr);
	void Draw(sf::RenderTarget& target) override;
	void Update(const float& dT) override;
	void Activate() override;
	void Deactivate() override;
	void OnCreate() override;
	void OnDestroy() override;
	void UpdateCamera() override;
	void Continue() override;

	UserMap* CreateNewMap(const std::string& map_name,const sf::Vector2u& map_tile_dimensions, const sf::Vector2i& tiledimensions, std::shared_ptr<sf::Texture> tile_sheet);

	//USER INPUT
	void ActivatePopUp(EventData::EventDetails* details);
	void DeactivatePopUp(EventData::EventDetails* details);
	void ConfirmButtonPopUp(EventData::EventDetails* details);
	void MoveSelectorLeft(EventData::EventDetails* details);
	void MoveSelectorRight(EventData::EventDetails* details);
	std::shared_ptr<sf::Texture> LoadSheet(const std::string& sheetname);
	void ActivateAtlasMap(EventData::EventDetails* details);
	void DeactivateAtlasMap(EventData::EventDetails* details);
	void CropSprite(EventData::EventDetails* details);
	void UnCropSprite(EventData::EventDetails* details);

};


#endif