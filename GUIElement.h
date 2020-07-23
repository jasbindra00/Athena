#ifndef GUIELEMENT_H
#define GUIELEMENT_H
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include "GUIFormatting.h"
#include "GUIInfo.h"




class GUIInterface;
class Manager_Texture;
class Manager_Font;

class GUIElement { //abstract base class for specialised GUIElements.
protected:
	GUIStateStyles statestyles; //each GUI state has its own style.
	GUIVisual visual;
	std::string name;
	GUIInterface* parent{ nullptr };

	GUIType type;
	GUIState currentstate;
	mutable GUIState activestate;
	mutable bool controlelement; //used by the interface in determining which layer to redraw
	mutable bool redrawrequired; //if it's changed, then the layer to which it forms within the interface must be redrawn.

	sf::Vector2f localposition;

	void ReleaseStyleResources();
	bool RequestTextureResources();
	bool RequestFontResources();
public:
	GUIElement(GUIInterface* parent, const GUIType& type, const GUIStateStyles& styles);

	virtual void OnNeutral() = 0;
	virtual void OnHover() = 0;
	virtual void OnClick(const sf::Vector2f& mousepos) = 0;

	virtual void SetState(const GUIState& state);
	virtual void Draw(sf::RenderTexture& texture);
	virtual void Update(const float& dT) = 0;
	virtual bool Contains(const sf::Vector2f& pos) const noexcept;
	void ApplyCurrentStyle();
	

	void CalibratePosition();
	bool RequiresRedraw() const { return redrawrequired; }
	virtual void SetElementSize(const sf::Vector2f& s);
	void SetRedraw(const bool& inp) const { redrawrequired = inp; }
	virtual void SetLocalPosition(const sf::Vector2f& pos);
	virtual void ReadIn(std::stringstream& stream) = 0;
	inline const bool& IsControl() const { return controlelement; }

	inline const GUIState& GetActiveState() const { return activestate; }
	inline GUIInterface* GetParent() const { return parent; }
	const sf::Vector2f& GetSize() const { return visual.elementsize; }
	inline const GUIType& GetType() const { return type; }
	GUIStyle& GetActiveStyle() { return statestyles[activestate]; }
	sf::Vector2f GetGlobalPosition() const;
	sf::Vector2f GetLocalPosition() const { return localposition; }
	sf::FloatRect GetLocalBoundingBox() const; //based on the sbg.ss
	std::string GetName() const { return name; }
	friend std::stringstream& operator>>(std::stringstream& stream, GUIElement* elt) {
		elt->ReadIn(stream);
		return stream;
	}
	virtual ~GUIElement();
	
};




#endif