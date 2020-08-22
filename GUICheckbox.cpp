#include "GUICheckbox.h"

GUICheckbox::GUICheckbox(GUIInterface* parent,Manager_GUI* mgr, const GUIStateStyles& styles, KeyProcessing::Keys& keys):GUIElement(parent,mgr, GUIType::CHECKBOX,GUILayerType::CONTENT, styles, keys){
	//specific keys for elements
	using GUIData::GUIStateData::GUIState;
	KeyProcessing::FillMissingKey(KeyProcessing::KeyPair{ "CHECKBOX_TYPE","ERROR" }, keys);
	std::string checkboxstr = keys.find("CHECKBOX_TYPE")->second;
	//default the checkboxtype if invalid
	if (checkboxstr == "CIRCLE") checkboxsquare = false;
	else checkboxstr = "SQUARE";
	std::string checkedtexture{ "GUICheckbox_Checked_" + checkboxstr + ".png" };
	std::string uncheckedtexture{ "GUICheckbox_Unchecked_" + checkboxstr + ".png" };
	GetVisual().ReadIn<GUIFormattingData::BG>(GUIState::FOCUSED, KeyProcessing::Keys{ {"TEXTURE_NAME" , std::move(checkedtexture)} });
	GetVisual().ReadIn<GUIFormattingData::Text>(GUIState::NEUTRAL, KeyProcessing::Keys{ {"TEXTURE_NAME" , std::move(uncheckedtexture)} });
	for (int i = 0; i < 3; ++i) GetVisual().ReadIn<GUIFormattingData::Text>(static_cast<GUIState>(i), KeyProcessing::Keys{ {"STRING", ""} });
}
void GUICheckbox::ToggleChecked() {
	if (checked == true) {
		checked = false;
		OnNeutral();
	}
	else {
		checked = true;
		OnFocus();
	}
	CheckboxCallback();
}
void GUICheckbox::OnNeutral(){
	SetState(GUIState::NEUTRAL);
}
void GUICheckbox::OnHover(){
}
void GUICheckbox::OnClick(const sf::Vector2f& mousepos){
	ToggleChecked();
}
void GUICheckbox::OnLeave(){
}

void GUICheckbox::OnFocus(){
	SetState(GUIState::FOCUSED);
}

void GUICheckbox::OnRelease(){

}
void GUICheckbox::ReadIn(KeyProcessing::Keys& keys){
	GUIElement::ReadIn(keys);

}


