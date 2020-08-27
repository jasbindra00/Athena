#include "GUILabel.h"
#include "EventData.h"
GUILabel::GUILabel(GUIInterface* parent):GUIElement(parent, GUIType::LABEL, GUILayerType::CONTENT){
}

void GUILabel::OnNeutral() {
	GUIElement::OnNeutral();
}

void GUILabel::OnHover() {
}
void GUILabel::OnClick(const sf::Vector2f& mousepos) {
	GUIElement::OnFocus();
}
void GUILabel::OnLeave() {
}
void GUILabel::OnRelease(){

}

