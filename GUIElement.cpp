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
void GUIElement::ApplyLocalPosition(){
	visual.SetPosition(localposition); 
}
void GUIElement::ApplySize() {
	visual.SetSize(elementsize);
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
void GUIElement::ReadIn(std::stringstream& stream) {
	sf::Vector2f position;
	sf::Vector2f size;
	stream >> name >> position.x >> position.y >> size.x >> size.y;
	//note that these changes aren't applied immediately but will be applied in the next update cycle.
	SetElementSize(size);
	SetLocalPosition(position);
}
GUIElement::GUIElement(GUIInterface* p, const GUIType& t, const GUIStateStyles& stylemap, std::stringstream& attributes) :type(t), parent(p), controlelement(false) {
	statestyles = stylemap;
	attributes >> this;
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
	MarkRedraw(true); //now the interface knows to redraw the layer.
	
}
void GUIElement::Draw(sf::RenderTexture& texture){
	texture.draw(visual.sbg);
	sf::RectangleShape x;
	x.setFillColor(sf::Color::Red);
	x.setOutlineColor(sf::Color::White);
	x.setOutlineThickness(5);
	x.setPosition(localposition);
	x.setSize(elementsize);
	texture.draw(x);
	//texture.draw(visual.tbg);
	//texture.draw(visual.text);
}
void GUIElement::Update(const float& dT){
	if (pendingcalibration) {
		if (pendingsizeapply) {
			ApplySize();
			pendingsizeapply = false;
		}
		if (pendingpositionapply) {
			ApplyLocalPosition();
			pendingpositionapply = false;
		}
		CalibratePosition();
		pendingcalibration = false;
	}
}

sf::FloatRect GUIElement::GetLocalBoundingBox() const {
	return sf::FloatRect{ localposition, GetSize() };
}
bool GUIElement::Contains(const sf::Vector2f& mouseglobal) const noexcept{
	auto globalpos = GetGlobalPosition();
	auto rect = sf::FloatRect{ globalpos, elementsize };
	return rect.contains(mouseglobal);
}
void GUIElement::CalibratePosition() {
	return;
	if (parent == this || parent == nullptr) return; //if in mid initialisation
	auto overhangs = parent->EltOverhangs(this); //check if this entire elt still lies in interface after pos change
	if (overhangs.first == false) return; //elt still lies within the interface.
	SetLocalPosition(overhangs.second);
	ApplyLocalPosition();
}
void GUIElement::SetLocalPosition(const sf::Vector2f& pos){
	localposition = pos;
	pendingcalibration = true;
	pendingpositionapply = true;
}
void GUIElement::SetElementSize(const sf::Vector2f& s) {
	elementsize = s;
	pendingcalibration = true;
	pendingsizeapply = true;
}
sf::Vector2f GUIElement::GetGlobalPosition() const{
	if (parent == this) return localposition;
	return localposition + GetParent()->GetGlobalPosition(); 
}
GUIElement::~GUIElement() {
	ReleaseStyleResources();
}

