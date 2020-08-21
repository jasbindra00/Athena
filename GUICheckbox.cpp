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
	//default to system textures if not overriden
	std::string checkedtexture{ "GUICheckbox_Checked_" + checkboxstr + ".png" };
	std::string uncheckedtexture{ "GUICheckbox_Unchecked_" + checkboxstr + ".png" };

	//every time we change the tbg, we need to toggle style apply.
	//otherwise, the user may forget to do so.

	{
		auto focusedstyle = visual.GetStyle(GUIState::FOCUSED);
		focusedstyle.background.tbg_name = std::move(checkedtexture);
		focusedstyle.AdjustForSystemTexture();
		visual.ChangeStyle(GUIState::FOCUSED, std::move(focusedstyle));
	}
	{
		auto 
	}
	visual.GetStyle(activestate).background.tbg_name = std::move(checkedtexture);
	statestyles[GUIState::FOCUSED].AdjustForSystemTexture();

	statestyles[GUIState::NEUTRAL].background.tbg_name = std::move(uncheckedtexture);
	statestyles[GUIState::NEUTRAL].AdjustForSystemTexture();
	
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


