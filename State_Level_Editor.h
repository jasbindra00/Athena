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

class State_LevelEditor : public State_Base
{
private:
	MapData::MapImplPtr active_map;

	//***State interface data***
	enum INTERFACE_TYPE : unsigned char{
		TOP_PANEL, RIGHT_PANEL, BOT_PANEL, POP_UP_PANEL
	};
	enum : unsigned char {
		SCROLL_AMOUNT = 10
	};
	std::array<GUIInterface*, 4> interfaces;
	inline GUIInterface* GetInterface(const INTERFACE_TYPE& t) {
		return interfaces.at(static_cast<unsigned int>(t));
	}
	//*** ***
	sf::RectangleShape empty_tile;
	


	bool ValidatePopUpTextfields(); //Checks the user textfield input to ensure that they lie within valid ranges.
	bool CreateNewMap(sf::Vector2u&& map_dimensions, sf::Vector2f&& tile_pixel_dimensions, std::string&& tile_sheet_name); //Called after a successful tile sheet load in.
	void SaveMap();


	//GUIBinding Callables
	void ConfirmButtonPopup();
	void MoveCamera(EventData::EventDetails* details);
	void Scroll(EventData::EventDetails* details);
	void TogglePopUp();
	void PlaceTile();
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