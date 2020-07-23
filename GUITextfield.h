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
	void UpdateStyle(const GUIState& state, const GUIStyle& style) override;
	void SetState(const GUIState& state) override;
	void Draw(sf::RenderTexture& texture) override;
	void AppendChar(const char& c);
	void PopChar();
	void ClearString();
	void SetLocalPosition(const sf::Vector2f& pos) override;
	void ReadIn(std::stringstream& stream) override;
	void Update(const float& dT) override;



	void SetElementSize(const sf::Vector2f& s) override;

	

};


#endif