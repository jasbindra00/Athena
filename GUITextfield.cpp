#include "GUITextfield.h"
#include "GUIInterface.h"
#include "Manager_GUI.h"


GUITextfield::GUITextfield(GUIInterface* parent, const GUIStateStyles& styles, std::stringstream& attributes):GUIElement(parent, GUIType::TEXTFIELD, styles, attributes) {
	controlelement = false;
}
sf::Text& GUITextfield::GetString() {
	return visual.text;
}
void GUITextfield::OnNeutral(){
	SetState(GUIState::NEUTRAL);
}


void GUITextfield::Update(const float& dT){
	GUIElement::Update(dT);
}

void GUITextfield::OnHover(){
}

void GUITextfield::OnClick(const sf::Vector2f& mousepos) {
	SetState(GUIState::FOCUSED);
	GUIEvent evnt;
	evnt.elementname = name;
	evnt.elementtype = type;
	evnt.eventtype = GUIEventType::TEXTFIELDCLICK;
	parent->GetGUIManager()->AddEvent(std::move(evnt));
}



void GUITextfield::Draw(sf::RenderTexture& texture){
	texture.draw(visual.sbg);

}

void GUITextfield::AppendChar(const char& c){
	auto& str = GetString();
	str.setString(str.getString() + c);
}

void GUITextfield::PopChar(){
	auto& str = GetString();
	str.setString(str.getString().substring(0, str.getString().getSize() - 1));
}

void GUITextfield::ClearString(){
	GetString().setString("");
}




