#ifndef GUITEXTFIELD_H
#define GUITEXTFIELD_H
#include "GUIElement.h"
#include <stdint.h>
#include <iostream>

class GUITextfield : public GUIElement {
	friend class GUIInterface;
protected:
	std::string textfieldstr;
public:
	GUITextfield(GUIInterface* parent, const GUIStateStyles& styles,KeyProcessing::Keys& attributes);
	void OnNeutral() override;
	void OnHover() override;
	void OnClick(const sf::Vector2f& mousepos) override;
	virtual void OnLeave() override;
	virtual void OnRelease() override;
	void Draw(sf::RenderTexture& texture) override;
	void AppendChar(const char& c);
	void PopChar();

	void Update(const float& dT) override;
	void OnEnter() {
		std::cout << "ENTER TEXTFIELD" << std::endl;
	}
	sf::Text& GetText();
	


};


#endif