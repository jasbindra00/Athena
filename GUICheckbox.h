#ifndef GUICHECKBOX_H
#define GUICHECKBOX_H
#include "GUIElement.h"


class GUICheckbox : public GUIElement {
public:
	GUICheckbox(GUIInterface* parent, const GUIStateStyles& styles, const KeyProcessing::Keys& keys);
	void OnNeutral() override;
	void OnHover() override;
	void OnClick(const sf::Vector2f& mousepos) override;
	void OnLeave() override;
	void OnRelease() override;
	void Draw(sf::RenderTexture& texture) override;
	void Update(const float& dT) override;
	void ReadIn(const KeyProcessing::Keys& keys) override;
private:
protected:
	void ApplyLocalPosition() override;
	void ApplySize() override;

};


#endif