#ifndef STATE_GAME_H
#define STATE_GAME_H
#include "State_Base.h"
#include "EventData.h"

using EventData::EventDetails;
class State_Game : public State_Base
{
protected:
	using State_Base::transcendency;
	using State_Base::transparency;
	using State_Base::statemgr;
public:
	State_Game(Manager_State* statemanager, Manager_GUI* guimgr);
	virtual void Update(const float& dT) override;
	virtual void Draw(sf::RenderTarget& target) override;
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Activate() override;
	virtual void Deactivate() override;
	virtual void Continue() override;
	virtual void UpdateCamera() override;
	virtual ~State_Game() override;
	void KeyPress(EventDetails* details);
};




#endif