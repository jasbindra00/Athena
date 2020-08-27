#ifndef STATE_OPTIONS_H
#define STATE_OPTIONS_H
#include "State_Base.h"
class State_Options : public State_Base {
protected:
public:
	State_Options();
	virtual void Draw(sf::RenderTarget& target) override;
	virtual void Update(const float& dT) override;
	virtual void OnCreate() override;
	virtual void OnDestroy() override;
	virtual void Continue() override;
	virtual void Activate() override;
	virtual void Deactivate() override;
	virtual void UpdateCamera() override { }
	virtual ~State_Options() override;
};


#endif