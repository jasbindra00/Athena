#ifndef GUIINTERFACE_H
#define GUIINTERFACE_H
#include "GUIElement.h"
#include <functional>

class Manager_GUI;
class GUIInterface;
class Layers;

using GUIElementPtr = std::unique_ptr<GUIElement>;
using GUIElements = std::vector <std::pair<std::string, GUIElementPtr>>;

class GUIInterface : public GUIElement {
	friend class GUIElement;
protected:
	
	Manager_GUI* guimgr{ nullptr };
	GUIElements elements;
	
	std::unique_ptr<Layers> layers;
	mutable bool contentredraw;
	mutable bool controlredraw;
	mutable bool parentredraw;
	
	void RedrawBackgroundLayer();
	void RedrawContentLayer();
	void RedrawControlLayer();
	
public:
	GUIInterface(GUIInterface* parent, Manager_GUI* guimgr, const GUIStateStyles& styles, std::stringstream& stream);
	
	bool AddElement(const std::string& eltname, std::unique_ptr<GUIElement>& elt);
	bool RemoveElement(const std::string& eltname);
	bool HasElement(const std::string& eltname) const;



	void SetContentRedraw(const bool& inp) const { contentredraw = inp; }
	void SetControlRedraw(const bool& inp) const { controlredraw = inp; }
	void SetBackgroundRedraw(const bool& inp) const { redrawrequired = inp; }
	void SetParentRedraw(const bool& inp) const { parentredraw = inp; }


	bool NeedsContentRedraw() const { return contentredraw; }
	bool NeedsControlRedraw() const { return controlredraw; }
	bool NeedsBackgroundRedraw() const { return redrawrequired; }
	bool NeedsParentRedraw() const { return parentredraw; }
	
	std::pair<bool, sf::Vector2f> EltOverhangs(const GUIElement* const elt);

	//void FitToContentSize();

	
	void Draw(sf::RenderTexture& texture) override;
	void Render();
	void Update(const float& dT) override;


	virtual void SetLocalPosition(const sf::Vector2f& pos) override;
	virtual void ReadIn(std::stringstream& stream) override;
	virtual void OnHover() override;
	virtual void OnNeutral() override;
	virtual void OnClick(const sf::Vector2f& pos) override;
	virtual void SetElementSize(const sf::Vector2f& s) override;
	void SetState(const GUIState& state) override;

	Manager_GUI* GetGUIManager() const;
	virtual ~GUIInterface();
	
};


#endif