#ifndef GUITEXTFIELD_H
#define GUITEXTFIELD_H
#include "GUIElement.h"
#include <stdint.h>

class GUITextfield : public GUIElement {
protected:
	
public:
	GUITextfield(GUIInterface* parent, const GUIStateStyles& styles, const KeyProcessing::Keys& attributes);
	void OnNeutral() override;
	void OnHover() override;
	void OnClick(const sf::Vector2f& mousepos) override;
	virtual void OnLeave() override;
	virtual void OnRelease() override;
	void Draw(sf::RenderTexture& texture) override;
	void AppendChar(const std::uint32_t& code);
	void PopChar();
	void ClearString();
	void Update(const float& dT) override;
	sf::Text& GetText();
	std::string GetStdString();


};


#endif