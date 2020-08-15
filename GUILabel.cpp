#include "GUILabel.h"
#include "EventData.h"
#include "Manager_GUI.h"
GUILabel::GUILabel(GUIInterface* parent, const GUIStateStyles& style, const KeyProcessing::Keys& keys):GUIElement(parent,GUIType::LABEL,style,keys){

}

void GUILabel::OnNeutral(){

}

void GUILabel::OnHover(){
	GUIElement::OnHover();
}

void GUILabel::OnClick(const sf::Vector2f& mousepos){
	GUIElement::OnClick(mousepos);
}

void GUILabel::OnLeave(){

}
void GUILabel::OnRelease(){
	SetState(GUIState::NEUTRAL);
	EventData::GUIEventInfo evntinfo;
	evntinfo.elementstate = GUIState::NEUTRAL;
	evntinfo.interfacehierarchy = GetHierarchyString();
	GetGUIManager()->AddGUIEvent(std::make_pair(EventData::EventType::GUI_RELEASE, std::move(evntinfo)));
}


