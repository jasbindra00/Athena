#include "GUITextfield.h"
#include "GUIInterface.h"
#include "Manager_GUI.h"
#include "Utility.h"
#include <iostream>

GUITextfield::GUITextfield(GUIInterface* parent):GUIElement(parent,GUIType::TEXTFIELD,GUILayerType::CONTENT), predicatebitset(4026531840), maxchars(INT_MAX) {
}

std::string GUITextfield::GetTextfieldString() {
	return std::get<std::string>(GetVisual().GetStyle(activestate).GetAttribute(STYLE_ATTRIBUTE::TEXT_STRING));
}

void GUITextfield::OnNeutral(){
	auto textfieldstr = GetTextfieldString();
	GUIElement::OnNeutral();
	if (textfieldstr.empty()) SetCurrentStateString(default_text_string);
	else SetCurrentStateString(std::move(textfieldstr));
}

void GUITextfield::OnHover(){
}
void GUITextfield::OnClick(const sf::Vector2f& mousepos) {
	auto currstr = GetTextfieldString();
	SetState(GUIState::FOCUSED);
	if (currstr == default_text_string) SetCurrentStateString("");
}
void GUITextfield::OnLeave(){
}
void GUITextfield::OnRelease(){
}

void GUITextfield::ReadIn(KeyProcessing::Keys& keys){
	using namespace Utility::CharacterCheckData;
	GUIElement::ReadIn(keys);
	auto textfieldpredicatekeys = keys.equal_range("TEXTFIELD_PREDICATE");
	for (auto it = textfieldpredicatekeys.first; it != textfieldpredicatekeys.second; ++it) {
		auto validpredicate = magic_enum::enum_cast<STRING_PREDICATE>(it->second);
		if (validpredicate.has_value()) AddPredicate(std::move(validpredicate.value()));
	}
	auto maxcharkey = KeyProcessing::GetKey("MAX_TEXTFIELD_CHARS", keys);
	if (!maxcharkey.first) return;
	try { SetMaxChars(std::stoi(maxcharkey.second->second)); }
	catch (const std::exception& exception) {}
}

void GUITextfield::SetCurrentStateString(const std::string& str) { GetVisual().GetStyle(activestate).ReadIn(STYLE_ATTRIBUTE::TEXT_STRING, str);}

void GUITextfield::OnElementCreate(Manager_Texture* texturemgr, Manager_Font* fontmgr, KeyProcessing::Keys& attributes, const GUIStateStyles& stylemap){
	GUIElement::OnElementCreate(texturemgr, fontmgr, attributes, stylemap);
	default_text_string = std::get<std::string>(GetVisual().GetStyle(GUIState::NEUTRAL).GetAttribute(STYLE_ATTRIBUTE::TEXT_STRING));
}

void GUITextfield::AppendChar(const char& c){	
	std::string str = GetTextfieldString();
	if (str.size() + 1 > maxchars) return;
	std::cout << str + c << std::endl;
	GetVisual().GetStyle(activestate).ReadIn(STYLE_ATTRIBUTE::TEXT_STRING, std::move(str += c));
}
void GUITextfield::PopChar() {	
	std::string str = GetTextfieldString();
	if(str.size() == 0) return;
	str.pop_back();
	GetVisual().GetStyle(activestate).ReadIn(STYLE_ATTRIBUTE::TEXT_STRING, std::move(str));
}