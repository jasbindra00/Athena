#include "GUIInterface.h"
#include "Manager_Texture.h"
#include "Manager_Font.h"
#include "Manager_GUI.h"
#include "Window.h"
#include <array>


GUIElement::GUIElement(GUIInterface* p, Manager_GUI* guimgr, GUIType& t, const GUILayerType& layer,const GUIStateStyles& stylemap) :type(t),layertype(layer), parent(p) {

}
void GUIElement::Render(sf::RenderTarget& target, const bool& toparent){
	visual->Render(target,toparent);
}
void GUIElement::Update(const float& dT) {
	AdjustPositionToParent();
	visual->Update(GetLocalBoundingBox());
}
void GUIElement::OnNeutral(){
	SetState(GUIState::NEUTRAL);
	EventData::GUIEventInfo evntinfo;
	evntinfo.interfacehierarchy = GetHierarchyString();
	GetGUIManager()->AddGUIEvent(std::make_pair(EventData::EventType::GUI_CLICK, std::move(evntinfo)));
}
void GUIElement::OnHover() {

}
void GUIElement::OnClick(const sf::Vector2f& mousepos) {
	SetState(GUIState::CLICKED);
	EventData::GUIEventInfo evntinfo;
	evntinfo.interfacehierarchy = GetHierarchyString();
	GetGUIManager()->AddGUIEvent(std::make_pair(EventData::EventType::GUI_CLICK, std::move(evntinfo)));
}
void GUIElement::AdjustPositionToParent() {
	if (parent == nullptr) return; //if in mid initialisation
	auto overhangs = parent->EltOverhangs(this); //check if this entire elt still lies in interface after pos change
	if (overhangs.first == false) return; //elt still lies within the interface.
	visual->QueuePosition(overhangs.second);
}

void GUIElement::SetState(const GUIState& state) {
	if (state == activestate) return;
	activestate = state;
	visual->QueueState(state);
}
void GUIElement::ReadIn(KeyProcessing::Keys& keys) {
	//TO DEFAULT THE KEYS TO ERROR OR TO SEARCH FOR EACH INDIVIDUAL KEY?
	name = keys.find("ELEMENTNAME")->second;
	(keys.find("ELEMENT_HIDDEN")->second == "FALSE") ? hidden = false : hidden = true;
	(keys.find("ENABLED")->second == "FALSE") ? enabled = false : enabled = true;
	std::string errorstr{ " for GUIElement of name " + name };
	std::string unabletoidentify{ "Unable to identify " };
	const sf::Vector2f parentdimensions = (parent == nullptr) ? sf::Vector2f{ std::stof(keys.find("WINX")->second), std::stof(keys.find("WINY")->second) } : parent->GetSize();
	sf::Vector2f size;
	sf::Vector2f origin;
	sf::Vector2f position;
	try { position.x = (std::stof(keys.find("POSITIONX%")->second) / 100); }
	catch (const std::exception& exc) {}// { LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{POSITIONX,x} / {POSITIONX%,x%} key " + errorstr); }
	try { position.y = (std::stof(keys.find("POSITIONY%")->second) / 100); }
	catch (const std::exception& exc) {}// { LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{POSITIONY,y} / {POSITIONY%,y%} key " + errorstr); }
	try { origin.x = std::stof(keys.find("ORIGINX%")->second) / 100; }
	catch (const std::exception& exception) {}// { //LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{ORIGINX%,x%} key " + errorstr);}
	try { origin.y = std::stof(keys.find("ORIGINY%")->second) / 100; }
	catch (const std::exception& exception) {}// {LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, unabletoidentify + "{ORIGINY%,y%} key " + errorstr); }
	position.y *= parentdimensions.y;
	position.x *= parentdimensions.x;
	//as well as an integral, arg can be "=" in which case the PIXEL size of the given dimension will be equivalent to the remaining orthogonal dimension.
	bool sizexeq = false;
	bool sizeyeq = false;
	{
		auto sizexkey = keys.find("SIZEX%")->second;
		auto sizeykey = keys.find("SIZEY%")->second;
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

	visual->ReadIn<GUIFormattingData::Text>(activestate, KeyProcessing::Keys{ {"STRING", std::move(name)} });
	visual->QueuePosition(std::move(position));
	visual->QueueSize(std::move(size));
}

void GUIElement::SetPosition(const sf::Vector2f& position){
	visual->QueuePosition(position);
}

void GUIElement::SetSize(const sf::Vector2f& size){
	visual->QueueSize(size);
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

