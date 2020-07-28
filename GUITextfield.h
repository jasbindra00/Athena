#ifndef GUITEXTFIELD_H
#define GUITEXTFIELD_H
#include "GUIElement.h"


class GUITextfield : public GUIElement {
protected:
	sf::Text& GetString();
public:
	GUITextfield(GUIInterface* parent, const GUIStateStyles& styles, std::stringstream& attributes);
	void OnNeutral() override;
	void OnHover() override;
	void OnClick(const sf::Vector2f& mousepos) override;
	virtual void OnLeave() override;
	virtual void OnRelease() override;
	void Draw(sf::RenderTexture& texture) override;
	void AppendChar(const char& c);
	void PopChar();
	void ClearString();
	void Update(const float& dT) override;


};


#endif