#ifndef GUIINTERFACE_H
#define GUIINTERFACE_H
#include "GUIElement.h"
#include <iostream>

//forward declarations
class Manager_GUI;
using GUIElementPtr = std::unique_ptr<GUIElement>;
using GUIElements = std::vector <std::pair<std::string, GUIElementPtr>>;
using GUIElementIterator = GUIElements::iterator;

namespace GUILayerData { class GUILayers;} //pImpl

class GUIInterface : public GUIElement{
	friend class GUIElement;
private:
	std::unique_ptr<GUILayerData::GUILayers> layers;
	friend class GUIElement;
	friend class Manager_GUI;
protected:
	mutable bool parentredraw;
	GUIElements elements;

	Manager_GUI* guimgr{ nullptr };
	virtual void SetState(const GUIState& state) override;
	void DrawToLayer(const GUILayerType& layer,const sf::Drawable& drawable);
	void Draw(sf::RenderTarget& target, const bool& toparent) override;
	void Update(const float& dT) override;
	virtual void ReadIn(KeyProcessing::Keys& keys) override;
	virtual void OnElementCreate(Manager_Texture* texturemgr, Manager_Font* fontmgr, KeyProcessing::Keys& attributes, const GUIStateStyles& stylemap) override;
	std::pair<bool, GUIElementIterator> GetElement(const std::string& elementname);	
public:
	GUIInterface(GUIInterface* parent, Manager_GUI* guimgr);

	virtual void SetPosition(const sf::Vector2f& pos) override;
	virtual void SetSize(const sf::Vector2f& size) override;
	virtual void OnClick(const sf::Vector2f& pos) override;
	virtual void OnRelease();
	virtual void OnLeave() override {
	}
	virtual void SetEnabled(const bool& inp) const override;
	void DefocusTextfields();

	bool AddElement(const std::string& eltname, std::unique_ptr<GUIElement>& elt);
	bool RemoveElement(const std::string& eltname);
	
	virtual const bool& PendingParentRedraw() const override; //may have been redrawn to its layer already, in which case the visual will deactivate parent redraw. but the layer redraw will still be activated.

	std::pair<bool, sf::Vector2f> EltOverhangs(const GUIElement* const elt);
	const virtual sf::Vector2f& GetLocalPosition() const override;
	
	virtual ~GUIInterface();
	
};


#endif