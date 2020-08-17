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
using namespace GUIFormatting;
using GUIStateStyles = std::unordered_map<GUIState, GUIStyle>;
class GUIElement { //abstract base class for specialised GUIElements.
	friend class GUIInterface;
protected:
	GUIStateStyles statestyles; //each GUI state has its own style.
	GUIVisual visual;
	std::string name;
	GUIInterface* parent{ nullptr };

	GUIType type;
	GUIState currentstate;
	mutable bool controlelement; //used by the interface in determining which layer to redraw
	mutable GUIState activestate;
	mutable bool backgroundredraw; //if it's changed, then the layer to which it forms within the interface must be redrawn.
	mutable bool hidden; //an inactive element is hidden from the GUI.
	mutable bool enabled;

	mutable bool pendingpositionapply;
	mutable bool pendingsizeapply;
	mutable bool pendingstyleapply;
	mutable bool pendingchange;
	mutable bool requirestextcalibration;

	sf::Vector2f localposition; //position relative to its parent. this will be the position of its background textures.
	sf::Vector2f elementsize;

	template<typename T, typename = typename ManagedResourceData::ENABLE_IF_MANAGED_RESOURCE<T>>
	bool RequestVisualResource() {
		auto& activestyle = GetActiveStyle();
		std::string *resname;
		if constexpr (std::is_same_v<typename std::decay_t<T>, sf::Texture>) resname = &activestyle.background.tbg_name;
		else if constexpr (std::is_same_v<typename std::decay_t<T>, sf::Font>) resname = &activestyle.text.fontname;
		if (resname->empty()) return false;
		auto newres = GetGUIManager()->GetContext()->GetResourceManager<T>()->RequestResource(*resname);
		if (newres == nullptr) {
			resname->clear();
			return false;
		}
		visual.GetResource<T>() = std::move(newres);
		return true;
	}
	void ReleaseStyleResources();
	virtual void ApplyLocalPosition();
	virtual void ApplySize();
	void CalibratePosition();
	bool SetState(const GUIState& state);
	virtual void Draw(sf::RenderTexture& texture);
	virtual void Update(const float& dT);
	void ApplyCurrentStyle();
public:
	GUIElement(GUIInterface* parent, const GUIType& type, const GUIStateStyles& styles,KeyProcessing::Keys& attributes);

	virtual void OnNeutral();
	virtual void OnHover();
	virtual void OnClick(const sf::Vector2f& mousepos);
	virtual void OnLeave() = 0;
	virtual void OnRelease() = 0;

	

	bool Contains(const sf::Vector2f& mousepos) const noexcept;


	
	virtual void SetEnabled(const bool& inp) const { enabled = inp; }
	void SetStyle(const GUIState& state);
	void SetText(const std::string& str);
	void SetElementSize(const sf::Vector2f& s);
	void SetLocalPosition(const sf::Vector2f& pos);
	void SetParent(GUIInterface* p) { parent = p; }

	void SetHidden(const bool& inp) const { hidden = inp; }
	void MarkRedraw(const bool& inp) const { backgroundredraw = inp; }
	
	bool RequiresRedraw() const { return backgroundredraw; }
	inline const bool& IsControl() const { return controlelement; }
	inline const bool& IsHidden() const { return hidden; }


	inline const bool& IsEnabled() const { return enabled; }
	inline const GUIState& GetActiveState() const { return activestate; }
	inline const sf::Vector2f& GetSize() const { return elementsize; }
	inline const GUIType& GetType() const { return type; }
	inline const std::string& GetName() const { return name; }
	inline GUIStyle& GetActiveStyle() { return statestyles[activestate]; }
	inline GUIInterface* GetParent() const { return parent; }
	inline const sf::Vector2f& GetLocalPosition() const { return localposition; }
	std::string GetHierarchyString() const;

	
	sf::Vector2f GetGlobalPosition() const;
	sf::FloatRect GetLocalBoundingBox() const;
	
	virtual void ReadIn(KeyProcessing::Keys& keys);
	Manager_GUI* GetGUIManager();
	virtual ~GUIElement();
};




#endif