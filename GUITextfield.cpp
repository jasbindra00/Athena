#include "GUITextfield.h"
#include "GUIInterface.h"
#include "Manager_GUI.h"
#include "Utility.h"


GUITextfield::GUITextfield(GUIInterface* parent):GUIElement(parent,GUIType::TEXTFIELD,GUILayerType::CONTENT), predicatebitset(4026531840), maxchars(INT_MAX) {
	SetPredicates(0);
	

}

std::string GUITextfield::GetTextfieldString() {
	return GetVisual().GetTEXTAttribute<GUIFormattingData::TEXTAttribute::STRING>();
	
}

void GUITextfield::OnNeutral(){
	SetState(GUIState::NEUTRAL);
	if (GetTextfieldString().empty()) SetCurrentStateString(defaulttextstrings.at(static_cast<int>(activestate)));
}

void GUITextfield::OnHover(){
}
void GUITextfield::OnClick(const sf::Vector2f& mousepos) {
	SetState(GUIState::FOCUSED);
	if (GetTextfieldString() == defaulttextstrings.at(static_cast<int>(activestate))) SetCurrentStateString("");
}
void GUITextfield::OnLeave(){
}
void GUITextfield::OnRelease(){
}

void GUITextfield::ReadIn(KeyProcessing::Keys& keys){
	GUIElement::ReadIn(keys);
	auto textfieldpredicatekeys = keys.equal_range("TEXTFIELD_PREDICATE");
	for (auto it = textfieldpredicatekeys.first; it != textfieldpredicatekeys.second; ++it) {
		Utility::CharacterCheck::STRING_PREDICATE predicatetype = Utility::CharacterCheck::StringPredicateConv(it->second);
		if (predicatetype != Utility::CharacterCheck::STRING_PREDICATE::NULLTYPE) AddPredicate(std::move(predicatetype));
	}
	auto maxcharkey = KeyProcessing::GetKey("MAX_TEXTFIELD_CHARS", keys);
	if (!maxcharkey.first) return;
	try { SetMaxChars(std::stoi(maxcharkey.second->second)); }
	catch (const std::exception& exception) {}
}

void GUITextfield::SetCurrentStateString(const std::string& str) { GetVisual().ReadIn<GUIFormattingData::Text>(activestate, KeyProcessing::Keys{ { "STRING", str } }); }

void GUITextfield::AppendChar(const char& c){
	std::string str = GetTextfieldString();
	if (str.size() + 1 > maxchars) return;
	GetVisual().ReadIn<GUIFormattingData::Text>(activestate, KeyProcessing::Keys{ {"STRING", std::move(str + c)} });
}
void GUITextfield::PopChar() {	
	std::string str = GetTextfieldString();
	if(str.size() == 0) return;
	str.pop_back();
	GetVisual().ReadIn<GUIFormattingData::Text>(activestate, KeyProcessing::Keys{ {"STRING", std::move(str)} });
}