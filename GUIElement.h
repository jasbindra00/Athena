#ifndef GUIELEMENT_H
#define GUIELEMENT_H
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <vector>
#include "GUIFormattingData.h"
#include "GUIData.h"
#include "KeyProcessing.h"
#include "SharedContext.h"
#include "Manager_Texture.h"
#include "Manager_Font.h"
class GUIInterface;
class Manager_Texture;
class Manager_Font;
class Manager_GUI;

//TURN VISUAL INTO pImpl
using namespace GUIData::GUIStateData;
using namespace GUIData::GUITypeData;
using GUIData::GUILayerType;
using GUIFormattingData::GUIVisual;
using GUIFormattingData::GUIStyle;
using GUIFormattingData::GUIStateStyles;
class GUIElement{ //abstract base class for specialised GUIElements.
	friend class GUIInterface;
	friend class Manager_GUI;
private:
	std::unique_ptr<GUIVisual> visual;

protected:
	GUILayerType layertype;
	mutable GUIState activestate;
	//the active style is applied to the visual
	std::string name;
	mutable GUIInterface* parent{ nullptr };
	GUIType type;

	mutable bool hidden; //an inactive element is not drawn 
	mutable bool enabled; //a disabled element will not respond to any input.
	 //if it's changed, then the layer to which it forms within the interface must be redrawn.
	virtual void OnNeutral();
	virtual void OnHover();
	virtual void OnClick(const sf::Vector2f& mousepos);
	virtual void OnLeave() = 0;
	virtual void OnRelease() = 0;
	void AdjustPositionToParent();
	virtual void SetState(const GUIState& state);
	virtual void Update(const float& dT);
	inline void SetParent(GUIInterface* p) const { parent = p; }
	virtual void OnElementCreate(Manager_Texture* texturemgr, Manager_Font* fontmgr, KeyProcessing::Keys& attributes, const GUIStateStyles& stylemap);
public:
	GUIElement(GUIInterface* parent, const GUIType& type, const GUILayerType& layertype);
	virtual void Draw(sf::RenderTarget& target, const bool& toparent);
	virtual void ReadIn(KeyProcessing::Keys& keys);

	virtual void SetEnabled(const bool& inp) const { enabled = inp; }
	void SetHidden(const bool& inp) const { hidden = inp; }
	virtual void SetPosition(const sf::Vector2f& position);
	virtual void SetSize(const sf::Vector2f& size);

	inline const bool& IsHidden() const { return hidden; }
	inline const bool& IsEnabled() const { return enabled; }

	virtual const bool& PendingParentRedraw() const { return visual->PendingParentRedraw(); }
	inline const std::string& GetName() const { return name; }
	inline const GUIType& GetType() const { return type; }
	inline GUIInterface* GetParent() const { return parent; }
	inline const GUIState& GetActiveState() const { return activestate; }

	inline const sf::Vector2f& GetSize() const { return visual->GetElementSize(); }
	inline const GUIStyle& GetActiveStyle() { return visual->GetStyle(activestate); }

	inline GUIVisual& GetVisual() { return *visual; }
	const virtual sf::Vector2f& GetLocalPosition() const { return visual->GetElementPosition(); }
	sf::Vector2f GetGlobalPosition() const;
	constexpr GUILayerType GetLayerType() { return layertype; }
	virtual sf::FloatRect GetLocalBoundingBox() const {
		if (name == "CONFIRM") {
			int z = 3;
		}
		return sf::FloatRect{ GetLocalPosition(), GetSize() }; }
	std::string GetHierarchyString();

	Manager_GUI* GetGUIManager();
	bool Contains(const sf::Vector2f& mousepos) const noexcept;
	virtual ~GUIElement();
};




#endif