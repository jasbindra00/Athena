#ifndef GUIINTERFACE_H
#define GUIINTERFACE_H
#include "GUIElement.h"
#include "GUIInterfaceLayers.h"
#include <iostream>


using namespace GUILayerData;
//forward declarations
class Manager_GUI;
using GUIElementPtr = std::unique_ptr<GUIElement>;
using GUIElements = std::vector <std::pair<std::string, GUIElementPtr>>;
using GUIElementIterator = GUIElements::iterator;

class GUIInterface : public GUIElement{
private:
	GUIInterfaceLayers layers;
	friend class GUIElement;
	friend class Manager_GUI;
protected:
	mutable bool parentredraw;
	GUIElements elements;

	Manager_GUI* guimgr{ nullptr };

	void Draw(sf::RenderTarget& texture) override; //draw to another interface.
	void Update(const float& dT) override;
	virtual void ApplyLocalPosition();
	std::pair<bool, GUIElementIterator> GetElement(const std::string& elementname);	
	void ResetParentRedraw() {

	}
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