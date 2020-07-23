#ifndef STATE_BASE_H
#define STATE_BASE_H
#include <SFML\Graphics\RenderTarget.hpp>
#include <SFML\Graphics\RenderWindow.hpp>
#include <SFML\Graphics\Drawable.hpp>

//need access to statemanager.
//need access to the window.

class Manager_State;
class State_Base : public sf::Drawable //abstract base for all specialised states
{
protected:
	sf::View stateview;
	Manager_State* statemgr;
	mutable bool transcendency;
	mutable bool transparency;
public:
	State_Base(Manager_State* statemanager): statemgr(statemanager), transparency(true), transcendency(true){
	}
	virtual ~State_Base(){}
	virtual void draw(sf::RenderTarget& target, sf::RenderStates state) const override = 0;
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


	
};



#endif