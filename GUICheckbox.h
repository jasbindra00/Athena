#ifndef GUICHECKBOX_H
#define GUICHECKBOX_H
#include "GUIElement.h"
#include <array>



//checkbox can be a square with a tick, or a nested circle.
//all we have to do, is change the texture.
class GUICheckbox : public GUIElement {
public:
	GUICheckbox(GUIInterface* parent,Manager_GUI* mgr, const GUIStateStyles& styles, KeyProcessing::Keys& keys);
	void OnNeutral() override;
	void OnHover() override;
	void OnClick(const sf::Vector2f& mousepos) override;
	void OnLeave() override;
	void OnFocus();
	void OnRelease() override;
	void ReadIn(KeyProcessing::Keys& keys) override;
private:
	mutable bool checked;
	mutable bool checkboxsquare{ true}; //false means circle check
protected:

	const bool& IsChecked() const { return checked; }
	void ToggleChecked();
	void CheckboxCallback() {

	}
	


};


#endif