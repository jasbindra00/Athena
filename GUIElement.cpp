#include "GUIInterface.h"
#include "Manager_Texture.h"
#include "Manager_Font.h"
#include "Manager_GUI.h"



bool GUIElement::RequestFontResources() {
	auto& activestyle = statestyles[activestate];
	if (activestyle.text.fontname.empty()) return false;
	visual.font = parent->GetGUIManager()->GetContext()->fontmgr->RequestResource(activestyle.text.fontname);
	return (visual.font != nullptr) ? true : false;
}

bool GUIElement::RequestTextureResources() {
	auto& activestyle = statestyles[activestate];
	if (activestyle.background.tbg_name.empty()) return false;
	visual.tbg_texture = parent->GetGUIManager()->GetContext()->texturemgr->RequestResource(activestyle.background.tbg_name);
	return (visual.tbg_texture != nullptr) ? true : false;
}
void GUIElement::ReleaseStyleResources(){
	auto& activestyle = GetActiveStyle();
	if (!activestyle.background.tbg_name.empty()) {
		visual.tbg_texture.reset();
		parent->GetGUIManager()->GetContext()->texturemgr->RequestResourceDealloc(activestyle.background.tbg_name);
	}
	if (!activestyle.text.fontname.empty()) {
		visual.font.reset();
		parent->GetGUIManager()->GetContext()->fontmgr->RequestResourceDealloc(activestyle.text.fontname);
	}
}


GUIElement::GUIElement(GUIInterface* p, const GUIType& t, const GUIStateStyles& stylemap) :type(t), parent(p), controlelement(false) {
	statestyles = stylemap;
	ApplyCurrentStyle();
}


void GUIElement::UpdateStyle(const GUIState& state, const GUIStyle& style){
	if (state != activestate) {
		statestyles[state] = style; return;
	}
	ReleaseStyleResources(); //release current resources before replacing.
	statestyles[state] = style;
	ApplyCurrentStyle();
}

void GUIElement::SetState(const GUIState& state){
//in a state change, we must reflect the change in the visual.
	if (state == activestate) return; //no visual change.
	ReleaseStyleResources();
	activestate = state;
	ApplyCurrentStyle();
}

void GUIElement::ApplyCurrentStyle(){
	auto& currentstyle = statestyles[activestate];
	//apply dynamically allocated resources to visuals
	if (RequestTextureResources()) { //request the required resources for the font and tbg.
		visual.tbg.setTexture(visual.tbg_texture.get());
	}
	if (RequestFontResources()) {
		visual.text.setFont(*visual.font);
	}

	visual.sbg.setFillColor(currentstyle.background.sbg_color);
	visual.sbg.setOutlineThickness(currentstyle.background.outlinethickness);
	visual.sbg.setOutlineColor(currentstyle.background.outlinecolor);

	visual.text.setString(currentstyle.text.textstr);
	visual.text.setFillColor(currentstyle.text.textcolor);
	visual.text.setCharacterSize(currentstyle.text.charactersize);
	SetRedraw(true); //now the interface knows to redraw the layer.
	
}

void GUIElement::Draw(sf::RenderTexture& texture){
	texture.draw(visual.sbg);
	//texture.draw(visual.tbg);
	//texture.draw(visual.text);
}
sf::FloatRect GUIElement::GetLocalBoundingBox() const {
	return sf::FloatRect{ localposition, GetSize() };
}
bool GUIElement::Contains(const sf::Vector2f& mouseglobal) const noexcept{

	auto globalpos = GetGlobalPosition();
	auto rect = sf::FloatRect{ globalpos, visual.elementsize };
	return rect.contains(mouseglobal);

// 	auto globalposition = GetGlobalPosition();
// 	if (mouseglobal.x < globalposition.x) return false;
// 	if (mouseglobal.x > globalposition.x + GetSize().x) return false;
// 	if (mouseglobal.y < globalposition.y) return false;
// 	if (mouseglobal.y > globalposition.y + GetSize().y) return false;
// 	return true;
}

void GUIElement::CalibratePosition() {
	if (parent == this) return;
	auto overhangs = parent->EltOverhangs(this); //check if this entire elt still lies in interface after pos change
	if (overhangs.first == false) return; //elt still lies within the interface.
	SetLocalPosition(overhangs.second);
}
void GUIElement::SetLocalPosition(const sf::Vector2f& pos){
	visual.SetPosition(pos);
	localposition = pos;
	
}
sf::Vector2f GUIElement::GetGlobalPosition() const{
	if (parent == this) return localposition;
	return localposition + GetParent()->GetGlobalPosition(); 
}
void GUIElement::SetElementSize(const sf::Vector2f& s) {
	visual.SetSize(s);
}

GUIElement::~GUIElement() {
	ReleaseStyleResources();
}

