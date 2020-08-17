#include "GUITextfield.h"
#include "GUIInterface.h"
#include "Manager_GUI.h"
#include <iostream>



GUITextfield::GUITextfield(GUIInterface* parent, const GUIStateStyles& styles, KeyProcessing::Keys& attributes):GUIElement(parent, GUIType::TEXTFIELD, styles, attributes), predicatebitset(4026531840) {
	controlelement = false;
	//override the character size here.
	statestyles[GUIState::FOCUSED].text.charactersize = GUIFormatting::maxcharactersize;
	statestyles[GUIState::NEUTRAL].text.charactersize = GUIFormatting::maxcharactersize;
	statestyles[GUIState::CLICKED].text.charactersize = GUIFormatting::maxcharactersize;
	using namespace PredicateData;
	SetPredicates(0);
	//read user predicate keys.
	auto textfieldpredicatekeys = attributes.equal_range("TEXTFIELD_PREDICATE");
	for (auto it = textfieldpredicatekeys.first; it != textfieldpredicatekeys.second; ++it) {
		AddPredicate(PredicateData::predicateconverter(it->second));
	}
}
sf::Text& GUITextfield::GetText() {
	return visual.text;
}
void GUITextfield::OnNeutral(){
	SetState(GUIState::NEUTRAL);
	if (textfieldstr.empty() || textfieldstr.back() == '\b') GetText().setString(statestyles[activestate].text.customtext);
	requirestextcalibration = true;
}
void GUITextfield::Update(const float& dT){
	GUIElement::Update(dT);
}
void GUITextfield::OnHover(){
}
void GUITextfield::OnClick(const sf::Vector2f& mousepos) {
	SetState(GUIState::FOCUSED);
	requirestextcalibration = true;
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
	if (!textfieldstr.empty() && textfieldstr.back() == '\b') textfieldstr.pop_back();
	textfieldstr = textfieldstr.substr(0, textfieldstr.length() - 1);
	if (!textfieldstr.empty() && textfieldstr.back() == '\b') textfieldstr.pop_back();
	GetText().setString(textfieldstr);
	requirestextcalibration = true;
	MarkRedraw(true); //mark redraw too powerful for element.
}