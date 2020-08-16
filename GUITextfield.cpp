#include "GUITextfield.h"
#include "GUIInterface.h"
#include "Manager_GUI.h"
#include <iostream>

GUITextfield::GUITextfield(GUIInterface* parent, const GUIStateStyles& styles, KeyProcessing::Keys& attributes):GUIElement(parent, GUIType::TEXTFIELD, styles, attributes) {
	controlelement = false;
	//override the character size here.
	statestyles[GUIState::FOCUSED].text.charactersize = GUIFormatting::maxcharactersize;
	statestyles[GUIState::NEUTRAL].text.charactersize = GUIFormatting::maxcharactersize;
	statestyles[GUIState::CLICKED].text.charactersize = GUIFormatting::maxcharactersize;
	visual.text.setString(customtext);
}
sf::Text& GUITextfield::GetText() {
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

}
void GUITextfield::OnLeave(){
}
void GUITextfield::OnRelease(){
}
void GUITextfield::Draw(sf::RenderTexture& texture){
	GUIElement::Draw(texture);
	texture.draw(visual.sbg);
	texture.draw(visual.text);

}
void GUITextfield::AppendChar(const char& c){
	textfieldstr += c;
	GetText().setString(textfieldstr);
	requirestextcalibration = true;
	MarkRedraw(true);
}
void GUITextfield::PopChar() {
	if (textfieldstr[textfieldstr.size() - 1] == '\b') textfieldstr.pop_back();
	textfieldstr = textfieldstr.substr(0, textfieldstr.length() - 1);
	//GetText().setString((textfieldstr.empty()) ? customtext : textfieldstr);
	GetText().setString(textfieldstr);
	requirestextcalibration = true;
	MarkRedraw(true);
}





