#ifndef GUIINTERFACE_H
#define GUIINTERFACE_H
#include "GUIElement.h"
#include <iostream>


//forward declarations
class Manager_GUI;
class GUIInterface;
class GUIInterfaceLayers;

using GUIElementPtr = std::unique_ptr<GUIElement>;
using GUIElements = std::vector <std::pair<std::string, GUIElementPtr>>;
using GUIElementIterator = GUIElements::iterator;
class GUIInterface : public GUIElement {
	friend class GUIElement;
	friend class Manager_GUI;
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

	void Draw(sf::RenderTexture& texture) override; //draw to another interface.
	void Render();
	void Update(const float& dT) override;

	void MarkContentRedraw(const bool& inp) const { contentredraw = inp; }
	void MarkControlRedraw(const bool& inp) const { controlredraw = inp; }
	void MarkRedrawToParent(const bool& inp) const { parentredraw = inp; }

	virtual void ApplyLocalPosition() override;
	
	std::pair<bool, GUIElementIterator> GetElement(const std::string& elementname);
	
	
public:
	GUIInterface(GUIInterface* parent, Manager_GUI* guimgr, const GUIStateStyles& styles, KeyProcessing::Keys& keys);

	virtual void OnClick(const sf::Vector2f& pos) override;
	virtual void OnRelease();
	virtual void OnLeave() override {

	}
	virtual void SetEnabled(const bool& inp) const override;
	void DefocusTextfields();

	bool AddElement(const std::string& eltname, std::unique_ptr<GUIElement>& elt);
	bool RemoveElement(const std::string& eltname);
	
	const bool& RequiresContentRedraw() const { return contentredraw; }
	const bool& RequiresControlRedraw() const { return controlredraw; }
	const bool& RequiresParentRedraw() const { return parentredraw; }
	
	std::pair<bool, sf::Vector2f> EltOverhangs(const GUIElement* const elt);

	virtual ~GUIInterface();
	
};


#endif