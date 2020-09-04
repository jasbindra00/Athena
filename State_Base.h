#ifndef STATE_BASE_H
#define STATE_BASE_H
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Graphics\Drawable.hpp>

//need access to statemanager.
//need access to the window.

class Manager_State;
class Manager_GUI;
class State_Base //abstract base for all specialised states
{
protected:
	sf::View state_view; //Information on where exactly this is located in the world.
	Manager_State* statemgr;
	Manager_GUI* guimgr;
	mutable bool transcendency;
	mutable bool transparency;
public:
	State_Base(Manager_State* statemanager, Manager_GUI* guimanager): statemgr(statemanager),guimgr(guimanager), transparency(true), transcendency(true){
	}
	virtual ~State_Base(){}
	virtual void Draw(sf::RenderTarget& target) = 0;
	virtual void Update(const float& dT) = 0;
	virtual void Activate() = 0;
	virtual void Deactivate() = 0;
	virtual void OnCreate() = 0;
	virtual void OnDestroy() = 0;
	virtual void UpdateCamera() = 0; //only really applicable to the game state.
	virtual void Continue() = 0;

	inline void SetTransparency(const bool& input) const { transparency = input; }
	inline void SetTranscendency(const bool& input) const{ transcendency = input; }
	inline bool GetTransparency() const { return transparency; }
	inline bool GetTranscendency() const { return transcendency; }
	const sf::View& GetStateView() const { return state_view; }
	const sf::FloatRect GetStateViewAABB() const {
		auto state_view_centre = static_cast<sf::Vector2f>(state_view.getCenter());
		auto state_view_size = static_cast<sf::Vector2f>(state_view.getSize());
		return sf::FloatRect{ state_view_centre.x - state_view_size.x / 2, state_view_centre.y - state_view_size.y / 2, state_view_size.x, state_view_size.y };
	}

};



#endif