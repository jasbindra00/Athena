#ifndef GUISCROLLBAR_H
#define GUISCROLLBAR_H
#include "GUIElement.h"
class GUIScrollbar : public GUIElement {
private:
public:
	GUIScrollbar(GUIInterface* parent, const GUIStateStyles& styles, KeyProcessing::Keys& keys);
	void OnNeutral() override;
	void OnHover() override;
	void OnClick(const sf::Vector2f& mousepos) override;
	void OnLeave() override;
	void OnRelease() override;
	void Draw(sf::RenderTexture& texture) override;
	void Update(const float& dT) override;
	void ReadIn(KeyProcessing::Keys& keys) override;
protected:
	void ApplyLocalPosition() override;


	void ApplySize() override;

};



#endif