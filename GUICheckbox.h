#ifndef GUICHECKBOX_H
#define GUICHECKBOX_H
#include "GUIElement.h"
#include <array>



//checkbox can be a square with a tick, or a nested circle.
//all we have to do, is change the texture.
class GUICheckbox : public GUIElement {
public:
	GUICheckbox(GUIInterface* parent, const GUIStateStyles& styles, KeyProcessing::Keys& keys);
	void OnNeutral() override;
	void OnHover() override;
	void OnClick(const sf::Vector2f& mousepos) override;
	void OnLeave() override;
	void OnFocus();
	void OnRelease() override;
	void ReadIn(KeyProcessing::Keys& keys) override;
private:
	mutable bool checked;
	std::array<char, 6> checkboxtype;
protected:

	const bool& IsChecked() const { return checked; }
	void ToggleChecked();
	void CheckboxCallback() {

	}
	


};


#endif