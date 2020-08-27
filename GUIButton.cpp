#include "GUIButton.h"
#include "EventData.h"
#include "Manager_GUI.h"
GUIButton::GUIButton(GUIInterface* parent):GUIElement(parent,GUIType::BUTTON,GUILayerType::CONTENT){

}

void GUIButton::OnNeutral(){
	GUIElement::OnNeutral();
}

void GUIButton::OnHover(){

}

void GUIButton::OnClick(const sf::Vector2f& mousepos){
	GUIElement::OnClick(mousepos);
}

void GUIButton::OnLeave(){

}
void GUIButton::OnRelease(){
	SetState(GUIState::NEUTRAL);
	EventData::GUIEventInfo evntinfo;
	evntinfo.elementstate = GUIState::NEUTRAL;
	evntinfo.interfacehierarchy = GetHierarchyString();
	GetGUIManager()->AddGUIEvent(std::make_pair(EventData::EventType::GUI_RELEASE, std::move(evntinfo)));
}


