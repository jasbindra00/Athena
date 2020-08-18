#include "GUICheckbox.h"

GUICheckbox::GUICheckbox(GUIInterface* parent, const GUIStateStyles& styles, KeyProcessing::Keys& keys):GUIElement(parent, GUIType::CHECKBOX, styles, keys){
	//specific keys for elements
	using GUIData::GUIStateData::GUIState;
	KeyProcessing::FillMissingKey(KeyProcessing::KeyPair{ "CHECKBOX_TYPE","ERROR" }, keys);
	std::string checkboxstr = keys.find("CHECKBOX_TYPE")->second;
	//default the checkboxtype if invalid
	if (!(checkboxstr == "SQUARE" || checkboxstr == "CIRCLE")) checkboxstr = "SQUARE";
	for (int i = 0; i < checkboxstr.size(); ++i) {
		checkboxtype[i] = checkboxstr[i];
	}
	//override the texture which may have been supplied by the user in the style configuration
	auto overrideniterators = keys.equal_range("TEXTURE_HAS_BEEN_OVERRIDEN");
	//::check which state textures have been overridden.
	std::unordered_multimap<GUIState, bool> textureoverrides = { {GUIState::NEUTRAL, false}, {GUIState::CLICKED, false}, {GUIState::FOCUSED, false} };
	for (auto it = overrideniterators.first; it != overrideniterators.second; ++it) {
		textureoverrides.find(GUIData::GUIStateData::converter(it->second))->second = true;
	}
	//default to system textures if not overriden
	std::string checkedtexture{ "GUICheckbox_Checked_" + checkboxstr + ".png" };
	std::string uncheckedtexture{ "GUICheckbox_Unchecked_" + checkboxstr + ".png" };
	if (!textureoverrides.find(GUIState::FOCUSED)->second) {
		statestyles[GUIState::FOCUSED].background.tbg_name = std::move(checkedtexture);
		statestyles[GUIState::FOCUSED].AdjustForSystemTexture();
	}
	if (!textureoverrides.find(GUIState::NEUTRAL)->second) {
		statestyles[GUIState::NEUTRAL].background.tbg_name = std::move(uncheckedtexture);
		statestyles[GUIState::NEUTRAL].AdjustForSystemTexture();
	}
	elementsize = sf::Vector2f{ 20,20 }; //max checkbox size.
	visual.SetTextStr("");
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


