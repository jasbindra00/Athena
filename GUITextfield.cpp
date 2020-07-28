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
	std::cout << "TEXTFIELD NEUTRAL" << std::endl;
	
}


void GUITextfield::Update(const float& dT){
	GUIElement::Update(dT);
}

void GUITextfield::OnHover(){
	SetState(GUIState::FOCUSED);
	std::cout << "TEXFIELD HOVER" << std::endl;
}

void GUITextfield::OnClick(const sf::Vector2f& mousepos) {
	SetState(GUIState::FOCUSED);
	GUIEventInfo evnt;
	evnt.interfacehierarchy = name;
	parent->GetGUIManager()->AddGUIEvent(std::move(evnt));
	std::cout << "TEXTFIELD CLICK" << std::endl;
}
void GUITextfield::OnLeave(){
	SetState(GUIState::NEUTRAL);
	std::cout << "LEFT TEXTFIELD" << std::endl;
}

void GUITextfield::OnRelease(){
	SetState(GUIState::NEUTRAL);
	std::cout << "RELEASED TEXTFIELD" << std::endl;
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




