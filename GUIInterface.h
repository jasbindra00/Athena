#ifndef GUIINTERFACE_H
#define GUIINTERFACE_H
#include "GUIElement.h"

//forward declarations
class Manager_GUI;
class GUIInterface;
class GUIInterfaceLayers;

using GUIElementPtr = std::unique_ptr<GUIElement>;
using GUIElements = std::vector <std::pair<std::string, GUIElementPtr>>;

class GUIInterface : public GUIElement {
	friend class GUIElement;
protected:
	
	GUIElements elements;
	std::unique_ptr<GUIInterfaceLayers> layers; //PImpl
	mutable bool contentredraw;
	mutable bool controlredraw;
	mutable bool parentredraw;
	
	void RedrawBackgroundLayer();
	void RedrawContentLayer();
	void RedrawControlLayer();

	Manager_GUI* guimgr{ nullptr };
	
public:
	GUIInterface(GUIInterface* parent, Manager_GUI* guimgr, const GUIStateStyles& styles, std::stringstream& stream);

	virtual void OnHover() override;
	virtual void OnNeutral() override;
	virtual void OnClick(const sf::Vector2f& pos) override;
	virtual void OnRelease();
	virtual void OnLeave() override {

	}
	void Draw(sf::RenderTexture& texture) override; //draw to another interface.
	void Render();
	void Update(const float& dT) override;

	bool AddElement(const std::string& eltname, std::unique_ptr<GUIElement>& elt);
	bool RemoveElement(const std::string& eltname);
	void AdjustContentSize();


	void MarkBackgroundRedraw(const bool& inp) const { MarkRedraw(inp); }
	void MarkContentRedraw(const bool& inp) const { contentredraw = inp; }
	void MarkControlRedraw(const bool& inp) const { controlredraw = inp; }
	void MarkRedrawToParent(const bool& inp) const { parentredraw = inp; }


	const bool& RequiresContentRedraw() const { return contentredraw; }
	const bool& RequiresControlRedraw() const { return controlredraw; }
	const bool& RequiresBackgroundRedraw() const { return RequiresRedraw(); }
	const bool& RequiresParentRedraw() const { return parentredraw; }
	
	std::pair<bool, sf::Vector2f> EltOverhangs(const GUIElement* const elt);

	virtual void ApplySize() override;
	virtual void ApplyLocalPosition() override;

	virtual void ReadIn(std::stringstream& stream) override;
	Manager_GUI* GetGUIManager() const;

	virtual ~GUIInterface();
	
};


#endif