#include "GUITextfield.h"
#include "GUIInterface.h"
#include "Manager_GUI.h"
#include <iostream>

GUITextfield::GUITextfield(GUIInterface* parent, const GUIStateStyles& styles, KeyProcessing::Keys& attributes):GUIElement(parent, GUIType::TEXTFIELD, styles, attributes) {
	controlelement = false;
}
sf::Text& GUITextfield::GetText() {
	return visual.text;
}

std::string GUITextfield::GetStdString(){
	return static_cast<std::string>(GetText().getString());
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
	GUIElement::OnClick(mousepos);
	SetState(GUIState::FOCUSED);
// 	GUIEventInfo evnt;
// 	evnt.interfacehierarchy = name;
// 	parent->GetGUIManager()->AddGUIEvent(std::move(evnt));
	std::cout << "TEXTFIELD CLICK" << std::endl;
}
void GUITextfield::OnLeave(){

}

void GUITextfield::OnRelease(){

}
void GUITextfield::Draw(sf::RenderTexture& texture){
	texture.draw(visual.sbg);

}

void GUITextfield::AppendChar(const std::uint32_t& unicode){
	auto str = GetText().getString();
	str.insert(str.getSize(), unicode);
	GetText().setString(std::move(str));
	
	
}

void GUITextfield::PopChar() {
	auto str = static_cast<std::string>(GetText().getString());
	str.pop_back();
	GetText().setString(std::move(str));
}
void GUITextfield::ClearString(){
	
}




