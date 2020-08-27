#ifndef STATE_MENU_H
#define STATE_MENU_H
#include "State_Base.h"

class State_Menu : public State_Base
{
protected:
public:
	State_Menu(Manager_State* statemgr);
	virtual void Draw(sf::RenderTarget& target) override;
	virtual void Update(const float& dT) override;
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Continue() override;
	virtual void Activate() override;
	virtual void Deactivate() override;
	virtual void UpdateCamera() override { }
	virtual ~State_Menu() override;

};


#endif