#ifndef GUIELEMENT_H
#define GUIELEMENT_H
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include "GUIFormatting.h"
#include "GUIData.h"
#include "KeyProcessing.h"
#include "SharedContext.h"
#include "Manager_Texture.h"
#include "Manager_Font.h"
class GUIInterface;
class Manager_Texture;
class Manager_Font;
class Manager_GUI;

using namespace GUIData::GUIStateData;
using namespace GUIData::GUITypeData;
using GUIData::GUILayerType;
using namespace GUIFormatting;


class GUIElement { //abstract base class for specialised GUIElements.
	friend class GUIInterface;
private:
	GUIVisual visual;
protected:
	GUILayerType layertype;
	mutable GUIState activestate;
	//the active style is applied to the visual
	std::string name;
	mutable GUIInterface* parent{ nullptr };
	GUIType type;

	mutable bool controlelement; //used by the interface in determining which layer to redraw

	mutable bool hidden; //an inactive element is not drawn 
	mutable bool enabled; //a disabled element will not respond to any input.
	 //if it's changed, then the layer to which it forms within the interface must be redrawn.
	virtual void OnNeutral();
	virtual void OnHover();
	virtual void OnClick(const sf::Vector2f& mousepos);
	virtual void OnLeave() = 0;
	virtual void OnRelease() = 0;

	void AdjustPositionToParent();
	void SetState(const GUIState& state);
	virtual void Update(const float& dT);
	inline void SetParent(GUIInterface* p) const { parent = p; }
public:
	GUIElement(GUIInterface* parent, const GUIType& type, const GUILayerType& layertype, const GUIStateStyles& styles,KeyProcessing::Keys& attributes);

	//FIND WORKAROUND FOR CTOR INIT
	virtual void Draw(sf::RenderTarget& target) const;
	virtual void ReadIn(KeyProcessing::Keys& keys);

	virtual void SetEnabled(const bool& inp) const { enabled = inp; }
	void SetHidden(const bool& inp) const { hidden = inp; }


	inline const bool& IsControl() const { return controlelement; }
	inline const bool& IsHidden() const { return hidden; }
	inline const bool& IsEnabled() const { return enabled; }

	const bool& QueuedRedraw() { return visual.pendingredraw; }
	inline const std::string& GetName() const { return name; }
	inline const GUIType& GetType() const { return type; }
	inline GUIInterface* GetParent() const { return parent; }
	inline const GUIState& GetActiveState() const { return activestate; }

	inline const sf::Vector2f& GetSize() const { return visual.GetElementSize(); }
	inline const GUIStyle& GetActiveStyle() const { return visual.GetStyle(activestate); }



	inline const sf::Vector2f& GetLocalPosition() const { return visual.GetLocalPosition(); }
	sf::Vector2f GetGlobalPosition() const;
	constexpr GUILayerType GetLayerType() { return layertype; }
	sf::FloatRect GetLocalBoundingBox() const { return sf::FloatRect{ visual.GetLocalPosition(), visual.GetElementSize() }; }
	std::string GetHierarchyString();

	Manager_GUI* GetGUIManager();
	bool Contains(const sf::Vector2f& mousepos) const noexcept;
	virtual ~GUIElement();
};




#endif