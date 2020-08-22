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
private:
	std::unique_ptr<GUILayerData::GUILayers> layers;
	friend class GUIElement;
	friend class Manager_GUI;
protected:
	mutable bool parentredraw;
	GUIElements elements;

	Manager_GUI* guimgr{ nullptr };
	void DrawToLayer(const GUILayerType& layer,const sf::Drawable& drawable);
	void Render(sf::RenderTarget& target, const bool& toparent);
	void Update(const float& dT) override;
	virtual void ReadIn(KeyProcessing::Keys& keys) override;
	virtual void OnElementCreate(Manager_Texture* texturemgr, Manager_Font* fontmgr, KeyProcessing::Keys& attributes) override;
	std::pair<bool, GUIElementIterator> GetElement(const std::string& elementname);	
public:
	GUIInterface(GUIInterface* parent, Manager_GUI* guimgr, const GUIStateStyles& styles);

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
	
	const bool& PendingInterfaceRedraw() const;
	
	std::pair<bool, sf::Vector2f> EltOverhangs(const GUIElement* const elt);

	virtual ~GUIInterface();
	
};


#endif