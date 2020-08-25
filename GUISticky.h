#ifndef GUISTICKY_H
#define GUISTICKY_H
#include "GUIElement.h"
class GUISticky : public GUIElement {
private:
public:
	virtual void OnNeutral() {
		GUIElement::OnNeutral();
	}
	virtual void OnRelease() {

	}
	virtual void OnClick(const sf::Vector2f& mousepos) {
		SetState(GUIState::FOCUSED);
	}
};



#endif