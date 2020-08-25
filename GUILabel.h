#ifndef GUILABEL_H
#define GUILABEL_H
#include "GUIElement.h"
class GUILabel : public GUIElement {
private:
public:
	GUILabel(GUIInterface* parent);
	void OnNeutral() override;
	void OnHover() override;
	void OnClick(const sf::Vector2f& mousepos) override;
	void OnLeave() override;
	void OnRelease() override;
protected:

};
#endif