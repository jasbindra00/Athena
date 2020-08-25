#ifndef GUIBUTTON_H
#define GUIBUTTON_H
#include "GUIElement.h"

class GUIButton : public GUIElement {
private:
public:
	GUIButton(GUIInterface* parent);
	void OnNeutral() override;
	void OnHover() override;
	void OnClick(const sf::Vector2f& mousepos) override;
	void OnLeave() override;
	void OnRelease() override;
protected:

};
#endif