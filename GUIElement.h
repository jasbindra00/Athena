#ifndef GUIELEMENT_H
#define GUIELEMENT_H
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include "GUIFormatting.h"
#include "GUIData.h"

class GUIInterface;
class Manager_Texture;
class Manager_Font;

using namespace GUIData::GUIStateData;
using namespace GUIData::GUITypeData;
using namespace GUIFormatting;
using GUIStateStyles = std::unordered_map<GUIState, GUIStyle>;
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

	mutable bool pendingpositionapply;
	mutable bool pendingsizeapply;
	mutable bool pendingcalibration;

	sf::Vector2f localposition;
	sf::Vector2f elementsize;

	void ReleaseStyleResources();
	bool RequestTextureResources();
	bool RequestFontResources();

	virtual void ApplyLocalPosition();
	virtual void ApplySize();
	void CalibratePosition();
public:
	GUIElement(GUIInterface* parent, const GUIType& type, const GUIStateStyles& styles, std::stringstream& attributes);

	virtual void OnNeutral() = 0;
	virtual void OnHover() = 0;
	virtual void OnClick(const sf::Vector2f& mousepos) = 0;
	virtual void OnLeave() = 0;
	virtual void OnRelease() = 0;

	void SetState(const GUIState& state);
	virtual void Draw(sf::RenderTexture& texture);
	virtual void Update(const float& dT);
	bool Contains(const sf::Vector2f& mousepos) const noexcept;


	void ApplyCurrentStyle();

	void SetElementSize(const sf::Vector2f& s);
	void SetLocalPosition(const sf::Vector2f& pos);
	void SetParent(GUIInterface* p) { parent = p; }


	void MarkRedraw(const bool& inp) const { redrawrequired = inp; }
	
	bool RequiresRedraw() const { return redrawrequired; }
	inline const bool& IsControl() const { return controlelement; }

	inline const GUIState& GetActiveState() const { return activestate; }
	inline const sf::Vector2f& GetSize() const { return elementsize; }
	inline const GUIType& GetType() const { return type; }
	inline const std::string& GetName() const { return name; }
	inline GUIStyle& GetActiveStyle() { return statestyles[activestate]; }
	inline GUIInterface* GetParent() const { return parent; }
	inline const sf::Vector2f& GetLocalPosition() const { return localposition; }
	
	sf::Vector2f GetGlobalPosition() const;
	sf::FloatRect GetLocalBoundingBox() const;
	
	
	friend std::stringstream& operator>>(std::stringstream& stream, GUIElement* elt) {
		elt->ReadIn(stream);
		return stream;
	}
	virtual void ReadIn(std::stringstream& stream);

	virtual ~GUIElement();
	
};




#endif