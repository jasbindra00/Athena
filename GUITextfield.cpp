#include "GUITextfield.h"
#include "GUIInterface.h"
#include "Manager_GUI.h"
#include <iostream>
#include <regex>



GUITextfield::GUITextfield(GUIInterface* parent, const GUIStateStyles& styles, KeyProcessing::Keys& attributes):GUIElement(parent, GUIType::TEXTFIELD, styles, attributes), predicatebitset(4026531840), maxchars(INT_MAX) {
	controlelement = false;
	//override the character size here.
	statestyles[GUIState::FOCUSED].text.charactersize = GUIFormatting::maxcharactersize;
	statestyles[GUIState::NEUTRAL].text.charactersize = GUIFormatting::maxcharactersize;
	statestyles[GUIState::CLICKED].text.charactersize = GUIFormatting::maxcharactersize;
	statestyles[GUIState::FOCUSED].text.customtext.clear();
	using namespace PredicateData;
	SetPredicates(0);
	//read user predicate keys.
	{
		auto textfieldpredicatekeys = attributes.equal_range("TEXTFIELD_PREDICATE");
		for (auto it = textfieldpredicatekeys.first; it != textfieldpredicatekeys.second; ++it) {
			AddPredicate(PredicateData::predicateconverter(it->second));
		}
	}
	auto maxtextfieldcharskey = attributes.find("MAX_TEXTFIELD_CHARS");
	if (maxtextfieldcharskey != attributes.end()) {
		try { SetMaxChars(std::stoi(maxtextfieldcharskey->second)); }
		catch (const std::exception& exception) {
		}
	}
	
	//configure custom text keys.
}

void GUITextfield::ApplyFieldString(const std::string& str){
	visual.SetTextStr(str);
	requirestextcalibration = true;
	QueueBackgroundRedraw(true);
}

sf::Text& GUITextfield::GetText() {
	return visual.text;
}
void GUITextfield::OnNeutral(){
	SetState(GUIState::NEUTRAL);
	if (GetTextfieldStr().empty()) ApplyFieldString(statestyles[activestate].text.customtext);
}
void GUITextfield::Update(const float& dT){
	GUIElement::Update(dT);
}
void GUITextfield::OnHover(){
}
void GUITextfield::OnClick(const sf::Vector2f& mousepos) {
	SetState(GUIState::FOCUSED);
	if (GetTextfieldStr() == statestyles[GUIState::NEUTRAL].text.customtext) ApplyFieldString(std::string{""});
}
void GUITextfield::OnLeave(){
}
void GUITextfield::OnRelease(){
}
void GUITextfield::Draw(sf::RenderTexture& texture){
	GUIElement::Draw(texture);
}
void GUITextfield::AppendChar(const char& c){
	std::string str = GetTextfieldStr();
	if (str.size() + 1 > maxchars) return;
	ApplyFieldString(std::move(str += c));
}
void GUITextfield::PopChar() {	std::string str = GetTextfieldStr();
	if(str.size() == 0) return;
	str.pop_back();
	ApplyFieldString(std::move(str));
}