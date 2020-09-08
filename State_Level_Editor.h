#ifndef STATE_LEVEL_EDITOR_H
#define STATE_LEVEL_EDITOR_H
#include <array>
#include "State_Base.h"
#include <SFML/Graphics/RectangleShape.hpp>


//***FORWARD DECLARATIONS
namespace EventData {
	class EventDetails;
}
namespace MapData {
	struct MapImpl;
	using MapImplPtr = std::unique_ptr<MapImpl>;
}
class Manager_State;
class GUIInterface;
//*** ***


struct TileSelector {

	TileSelector() {

	}
	void SetLocalPosition(const sf::Vector2u& local_position) {
		local_coordinates = static_cast<sf::Vector2i>(local_position);
		ApplyPosition();
	}
	void Move(const sf::Vector2i& offset) {
		local_coordinates += offset;
		ApplyPosition();
	}
	void OnCreate(const sf::Vector2u& max_dim, const sf::Vector2f& selector_size, const sf::Vector2f& starting_world_coords) {
		starting_world_coordinates = starting_world_coords;
		max_dimensions = max_dim;
		selector.setFillColor(sf::Color::Transparent);
		selector.setOutlineColor(sf::Color::Green);
		selector.setOutlineThickness(3);
		selector.setSize(selector_size);
		ApplyPosition();
	}
	void SetWorldPosition(const sf::Vector2f& vec) {

	}
	const auto GetLocalPosition() const { return static_cast<sf::Vector2u>(local_coordinates); }
	const sf::Vector2f& GetWorldPosition() const { return selector.getPosition(); }
	sf::Drawable* GetSelectorDrawable() { return &selector; }
	const bool& PendingRedraw() const { return pending_redraw; }
	void Render(sf::RenderTarget& target) const {
		target.draw(selector);
		pending_redraw = false;
	}
private:
	mutable bool pending_redraw{ true };
	sf::Vector2i local_coordinates;
	sf::Vector2f starting_world_coordinates;
	sf::RectangleShape selector;
	sf::Vector2u max_dimensions;
	void ApplyPosition() {
		
	}
};

class State_LevelEditor : public State_Base
{
private:
	MapData::MapImplPtr active_map;

	//***State interface data***
	enum INTERFACE_TYPE : unsigned char{
		TOP_PANEL, RIGHT_PANEL, BOT_PANEL, POP_UP_PANEL
	};
	enum : unsigned char {
		SCROLL_AMOUNT = 32,
	};
	static constexpr float ZOOM_AMOUNT = 0.1;
	std::array<GUIInterface*, 4> interfaces;
	inline GUIInterface* GetInterface(const INTERFACE_TYPE& t) {
		return interfaces.at(static_cast<unsigned int>(t));
	}
	//*** ***

	TileSelector map_selector;
	TileSelector tile_selector;
	
	
	bool ValidatePopUpTextfields(); //Checks the user textfield input to ensure that they lie within valid ranges.
	bool CreateNewMap(sf::Vector2u&& map_dimensions, sf::Vector2u&& tile_pixel_dimensions, std::string&& tile_sheet_name); //Called after a successful tile sheet load in.
	void SaveMap();


	//GUIBinding Callables
	void ConfirmButtonPopup();
	void MoveCamera(EventData::EventDetails* details);
	void Scroll(EventData::EventDetails* details);
	void TogglePopUp();
	void PlaceTile();
	void MoveSelector(EventData::EventDetails* details);

public:
	State_LevelEditor(Manager_State* smgr, Manager_GUI* gmgr);
	
	void Draw(sf::RenderTarget& target) override;
	void Update(const float& dT) override;
	void Activate() override;
	void Deactivate() override;
	void UpdateCamera() override;
	void OnCreate() override;
	void OnDestroy() override;
	void Continue() override;

};



#endif