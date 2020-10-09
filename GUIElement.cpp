#include "GUIInterface.h"
#include "GUILayerData.h"
#include "Manager_Texture.h"
#include "Manager_Font.h"
#include "Manager_GUI.h"
#include "Window.h"
#include <array>
#include "GUIData.h"

GUIElement::GUIElement(GUIInterface* p,const GUIType& t, const GUILayerType& layer) :type(t),layertype(layer), parent(p) {
}
void GUIElement::Draw(sf::RenderTarget& target, const bool& toparent){
	visual->Draw(target,toparent);
}

void GUIElement::Update(const float& dT) {
	//AdjustPositionToParent();
	//the user may have made changes to the visual, and this change must be communicated to the parent's layers.
	if (name == "MAP") {
		int x = 3;
	}
	visual->Update();
}
void GUIElement::OnElementCreate(Manager_Texture* texturemgr, Manager_Font* fontmgr, KeyProcessing::Keys& attributes, const GUIStateStyles& styles){
	visual = std::make_unique<GUIVisual>(texturemgr, fontmgr, styles);
	ReadIn(attributes);
	SetState(GUIState::NEUTRAL);
}
void GUIElement::OnNeutral(){
	SetState(GUIState::NEUTRAL);
	EventData::GUIEventInfo evntinfo;
	evntinfo.interfacehierarchy = GetHierarchyString();
	evntinfo.elementstate = activestate;
	GetGUIManager()->AddGUIEvent(std::make_pair(EventData::EventType::GUI_CLICK, std::move(evntinfo)));
}
void GUIElement::OnHover() {

}
void GUIElement::OnClick(const sf::Vector2f& mousepos) {
	SetState(GUIState::CLICKED);
	EventData::GUIEventInfo evntinfo;
	evntinfo.interfacehierarchy = GetHierarchyString();
	evntinfo.elementstate = activestate;
	GetGUIManager()->AddGUIEvent(std::make_pair(EventData::EventType::GUI_CLICK, std::move(evntinfo)));
}

void GUIElement::OnFocus(){
	SetState(GUIState::FOCUSED);
	EventData::GUIEventInfo evntinfo;
	evntinfo.interfacehierarchy = GetHierarchyString();
	evntinfo.elementstate = GUIState::FOCUSED;
	GetGUIManager()->AddGUIEvent(std::make_pair(EventData::EventType::GUI_CLICK, std::move(evntinfo)));
}

void GUIElement::AdjustPositionToParent() {
	if (parent == nullptr) return; //if in mid initialisation
	auto overhangs = parent->EltOverhangs(this); //check if this entire elt still lies in interface after pos change
	if (overhangs.first == false) return; //elt still lies within the interface.
	visual->SetPosition(overhangs.second);
}
void GUIElement::SetState(const GUIState& state) {
	/*if (state == activestate) return;*/
	activestate = state;
	visual->SetState(state);
}
void GUIElement::ReadIn(KeyProcessing::Keys& keys) {
	//TO DEFAULT THE KEYS TO ERROR OR TO SEARCH FOR EACH INDIVIDUAL KEY?
	name = keys.find("ELEMENT_NAME")->second;
	(keys.find("ELEMENT_HIDDEN")->second == "FALSE") ? hidden = false : hidden = true;
	(keys.find("ENABLED")->second == "FALSE") ? enabled = false : enabled = true;
	std::string errorstr{ " for GUIElement of name " + name };
	std::string unabletoidentify{ "Unable to identify " };
	const sf::Vector2f parentdimensions = (parent == nullptr) ? sf::Vector2f{ std::stof(keys.find("WINX")->second), std::stof(keys.find("WINY")->second) } : parent->GetSize();
	sf::Vector2f size;
	sf::Vector2f origin;
	sf::Vector2f position = GetLocalPosition();
	bool pendingposition{ false };
	try { position.x = (std::stof(keys.find("POSITION_X")->second) / 100);}
	catch (const std::exception& exc) {}// { LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{POSITIONX,x} / {POSITIONX%,x%} key " + errorstr); }
	try { position.y = (std::stof(keys.find("POSITION_Y")->second) / 100);}
	catch (const std::exception& exc) {}// { LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{POSITIONY,y} / {POSITIONY%,y%} key " + errorstr); }
	try { origin.x = std::stof(keys.find("ORIGIN_X")->second) / 100; }
	catch (const std::exception& exception) {}// { //LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{ORIGINX%,x%} key " + errorstr);}
	try { origin.y = std::stof(keys.find("ORIGIN_Y")->second) / 100; }
	catch (const std::exception& exception) {}// {LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{ORIGINY%,y%} key " + errorstr); }
	position.y *= parentdimensions.y;
	position.x *= parentdimensions.x;
	//as well as an integral, arg can be "=" in which case the PIXEL size of the given dimension will be equivalent to the remaining orthogonal dimension.
	bool sizexeq = false;
	bool sizeyeq = false;
	{
		auto sizexkey = keys.find("SIZE_X")->second;
		auto sizeykey = keys.find("SIZE_Y")->second;
		try { size.x = std::stof(sizexkey) / 100; }
		catch (const std::exception& exc) {
			if (sizexkey == "=") sizexeq = true;
		}// { LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{SIZEX,x} / {SIZEX%,x%} key" + errorstr); }
		try { size.y = std::stof(sizeykey) / 100; }
		catch (const std::exception& exc) {
			if (sizeykey == "=") sizeyeq = true;
		}// {LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{SIZEY,x} / {SIZEY%,x%} key" + errorstr); }
	}
	size.y *= parentdimensions.y;
	size.x *= parentdimensions.x;
	//only one "=" taken into account otherwise both will be 0.
	if (sizexeq) size.x = size.y;
	else if (sizeyeq) size.y = size.x;
	//move the position to acccount for the origin
	position.x -= (origin.x * size.x);
	position.y -= (origin.y * size.y);
	//adjust if elt overhangs parent
	if (position.x < 0) position.x = 0; 
	if (position.y < 0) position.y = 0;
	if (position.x + size.x >= parentdimensions.x) size.x = parentdimensions.x - position.x;
	if (position.y + size.y >= parentdimensions.y) size.y = parentdimensions.y - position.y;
	if (size != sf::Vector2f{0, 0}) SetSize(std::move(size));
	SetPosition(std::move(position));
}
void GUIElement::SetPosition(const sf::Vector2f& position){
	visual->SetPosition(position);
}
void GUIElement::SetSize(const sf::Vector2f& size){
	visual->SetSize(size);
}
std::string GUIElement::GetHierarchyString(){
	if (parent == nullptr) return name;
	GUIInterface* mparent = parent;
	std::vector<std::string> hierarchy{ name };
	while (mparent != nullptr) {
		hierarchy.emplace_back(parent->GetName());
		mparent = mparent->GetParent();
	}	
	return Utility::ConstructGUIHierarchyString(std::move(hierarchy));
}
Manager_GUI* GUIElement::GetGUIManager() {return (GetType() == GUIType::WINDOW) ? static_cast<GUIInterface*>(this)->guimgr : parent->guimgr;}
sf::Vector2f GUIElement::GetGlobalPosition() const{
	if (parent == nullptr) return GetLocalPosition();
	return GetLocalPosition() + GetParent()->GetGlobalPosition(); 
}
bool GUIElement::Contains(const sf::Vector2f& mouseglobal) const noexcept {
	auto globalpos = GetGlobalPosition();
	auto rect = sf::FloatRect{ globalpos, GetSize() };
	return rect.contains(mouseglobal);
}
GUIElement::~GUIElement() {

}

