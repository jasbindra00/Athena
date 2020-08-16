#include "GUIInterface.h"
#include "Manager_Texture.h"
#include "Manager_Font.h"
#include "Manager_GUI.h"
#include "Window.h"



GUIElement::GUIElement(GUIInterface* p, const GUIType& t, const GUIStateStyles& stylemap, const KeyProcessing::Keys& attributes) :type(t), parent(p), controlelement(false) {
	statestyles = stylemap;
	ReadIn(attributes);
	SetStyle(activestate);
}
bool GUIElement::RequestFontResources() {
	auto& activestyle = statestyles[activestate];
	if (activestyle.text.fontname.empty()) return false;
	visual.font = GetGUIManager()->GetContext()->fontmgr->RequestResource(activestyle.text.fontname);
	if (visual.font == nullptr) {

		//dont default the font. they should know that something is wrong.
// 		activestyle.text.fontname = std::string{ "arial.ttf" }; //default the font
// 		GetGUIManager()->GetContext()->fontmgr->RequestResource(activestyle.text.fontname);
		return false;
	}
	return true;
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
	visual.tbg_texture = GetGUIManager()->GetContext()->texturemgr->RequestResource(activestyle.background.tbg_name);
	if (visual.tbg_texture == nullptr) {
		activestyle.background.tbg_name.clear();
		return false;
	}
	return true;
}
void GUIElement::ReleaseStyleResources(){
	auto& activestyle = GetActiveStyle();
	if (!activestyle.background.tbg_name.empty()) {
		visual.tbg_texture.reset();
		GetGUIManager()->GetContext()->texturemgr->RequestResourceDealloc(activestyle.background.tbg_name);
	}
	if (!activestyle.text.fontname.empty()) {
		visual.font.reset();
		GetGUIManager()->GetContext()->fontmgr->RequestResourceDealloc(activestyle.text.fontname);
	}
}
void GUIElement::ReadIn(const KeyProcessing::Keys& keys) {
	name = keys.find("ELEMENTNAME")->second;
	(keys.find("HIDDEN")->second == "FALSE") ? hidden = false : hidden = true;
	std::string errorstr{ " for GUIElement of name " + name };
	std::string unabletoidentify{ "Unable to identify " };
	const sf::Vector2f parentdimensions = (parent == nullptr) ? sf::Vector2f{ std::stof(keys.find("WINX")->second), std::stof(keys.find("WINY")->second) } : parent->GetSize();
	sf::Vector2f size;
	sf::Vector2f origin;
	sf::Vector2f position;
	try {size.x = std::stof(keys.find("SIZEX%")->second) / 100;}
	catch (const std::exception& exc) { LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{SIZEX,x} / {SIZEX%,x%} key" + errorstr);}
	try { size.y = (std::stof(keys.find("SIZEY%")->second) / 100); }
	catch (const std::exception& exc) {LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{SIZEY,x} / {SIZEY%,x%} key" + errorstr);}
	try { position.x = (std::stof(keys.find("POSITIONX%")->second) / 100); }
	catch (const std::exception& exc) { LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{POSITIONX,x} / {POSITIONX%,x%} key "+ errorstr); }
	try { position.y = (std::stof(keys.find("POSITIONY%")->second) / 100); }
	catch (const std::exception& exc) { LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{POSITIONY,y} / {POSITIONY%,y%} key " + errorstr); }
	try { origin.x = std::stof(keys.find("ORIGINX%")->second) / 100; }
	catch (const std::exception& exception) { LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{ORIGINX%,x%} key " + errorstr);}
	try { origin.y = std::stof(keys.find("ORIGINY%")->second) / 100; }
	catch (const std::exception& exception) {LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify +"{ORIGINY%,y%} key " + errorstr);}
	position.y *= parentdimensions.y;
	position.x *= parentdimensions.x;
	size.y *= parentdimensions.y;
	size.x *= parentdimensions.x;
	//move the position to acccount for the origin
	position.x -= (origin.x * size.x);
	position.y -= (origin.y * size.y);
	//adjust if elt overhangs parent
	if (position.x < 0) position.x = 0; 
	if (position.y < 0) position.y = 0;
	if (position.x + size.x >= parentdimensions.x) size.x = parentdimensions.x - position.x;
	if (position.y + size.y >= parentdimensions.y) size.y = parentdimensions.y - position.y;
	visual.text.setString(name);
	SetElementSize(std::move(size));
	SetLocalPosition(std::move(position));
}
Manager_GUI* GUIElement::GetGUIManager() {
	if (GetType() == GUIType::WINDOW) return static_cast<GUIInterface*>(this)->guimgr; //GUITYPE::Window := GUIInterface
	return parent->guimgr;
}

void GUIElement::OnHover(){


}

void GUIElement::OnClick(const sf::Vector2f& mousepos){
	if (name == "RIGHT_SUB_PANEL") {
		int x = 4;
	}
	SetState(GUIState::CLICKED);
	EventData::GUIEventInfo evntinfo;
	evntinfo.interfacehierarchy = GetHierarchyString();
	GetGUIManager()->AddGUIEvent(std::make_pair(EventData::EventType::GUI_CLICK,std::move(evntinfo)));
}
void GUIElement::SetState(const GUIState& state){
	if (state == activestate) return; //no visual change.
	ReleaseStyleResources();
	activestate = state;
	SetStyle(state);
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
	if (name == "M_PANEL") {
		int x = 4;
		visual.text.setPosition(sf::Vector2f{ 50,50 });
	}
	visual.sbg.setFillColor(currentstyle.background.sbg_color);
	visual.sbg.setOutlineThickness(currentstyle.background.outlinethickness);
	visual.sbg.setOutlineColor(currentstyle.background.outlinecolor);
	visual.text.setFillColor(currentstyle.text.textcolor);

	sf::Vector2f pos;
	//nested interface
	sf::FloatRect rect = GetLocalBoundingBox();
	//if its an interface, we must draw the text relative to our own co-ordinate system since we have our own layers
	//if its an element, text is drawn relative to its parent, and thus local position (position relative to its parent) must be taken into account.
	if (type == GUIType::WINDOW) { rect.left = 0; rect.top = 0; }
	visual.CalibrateText(rect, currentstyle.text.originproportion, currentstyle.text.charactersize, currentstyle.text.localpositionproportion, name);
	MarkRedraw(true); //now the interface knows to redraw the layer.
	
}

void GUIElement::SetText(const std::string& str){
	visual.text.setString(str);
}

void GUIElement::Draw(sf::RenderTexture& texture) {
	auto& currentstyle = statestyles[activestate];
	texture.draw(visual.sbg);

	//texture.draw(visual.tbg);
	texture.draw(visual.text);	
}
void GUIElement::Update(const float& dT){
	if (pendingchange) {
		if (pendingsizeapply) {
			ApplySize();
			pendingsizeapply = false;
		}
		if (pendingpositionapply) {
			ApplyLocalPosition();
			pendingpositionapply = false;
		}		
		if (pendingstyleapply) {
			ApplyCurrentStyle();
			pendingstyleapply = false;
		}
		CalibratePosition();
		pendingchange = false;
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
	if (parent == nullptr) return; //if in mid initialisation
	auto overhangs = parent->EltOverhangs(this); //check if this entire elt still lies in interface after pos change
	if (overhangs.first == false) return; //elt still lies within the interface.
	SetLocalPosition(overhangs.second);
	ApplyLocalPosition();
}
void GUIElement::SetLocalPosition(const sf::Vector2f& pos){
	localposition = pos;
	pendingchange = true;
	pendingpositionapply = true;
}
void GUIElement::SetElementSize(const sf::Vector2f& s) {
	elementsize = s;
	pendingchange = true;
	pendingsizeapply = true;
}
void GUIElement::SetStyle(const GUIState& state) {
	pendingchange = true;
	pendingstyleapply = true;
}

std::string GUIElement::GetHierarchyString() const{
	if (parent == nullptr) return name;
	std::string str(name);
	GUIInterface* mparent = parent;
	while (mparent != nullptr) {
		str += mparent->GetName();
		mparent = mparent->GetParent();
	}	
	return str;
}

sf::Vector2f GUIElement::GetGlobalPosition() const{
	if (parent == nullptr) return localposition;
	return localposition + GetParent()->GetGlobalPosition(); 
}

GUIElement::~GUIElement() {
	ReleaseStyleResources();
}

