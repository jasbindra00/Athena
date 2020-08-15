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
	//REFACTOR THIS.
	name = keys.find("ELEMENTNAME")->second;
	
	if (name == "POP_UP_PANEL") {
		int x = 4;
	}
	std::string errorstr{ " for GUIElement of name " + name };
	//need to check if the sizex% of sizey%
	//need to check if the positionx% or positiony%
	sf::Vector2f position;
	auto videomode = sf::VideoMode::getDesktopMode();
	float wx = std::stof(keys.find("WINX")->second);
	float wy = std::stof(keys.find("WINY")->second);
	sf::Vector2f size{ wx,wy };
	sf::Vector2f origin;


// 	auto windim = static_cast<sf::Vector2f>(win->getSize());
	//const sf::Vector2f parentdimensions = (parent == nullptr) ? sf::Vector2f{ static_cast<float>(videomode.width), static_cast<float>(videomode.height) } : parent->GetSize();
	const sf::Vector2f parentdimensions = (parent == nullptr)? size : parent->GetSize();
	//parentdimensions = { wx,wy };
	(keys.find("HIDDEN")->second == "FALSE") ? hidden = false : hidden = true;
	try { size.x = std::stof(keys.find("SIZEX")->second); }
	catch (const std::exception& exception) {
		
		try {
			size.x = std::stof(keys.find("SIZEX%")->second) / 100;
			size.x *= parentdimensions.x;
		}
		catch (const std::exception& exc) {
			//invalid positionx. invalid positionx%.
			LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify {SIZEX,x} / {SIZEX%,x%} key. DEFAULTING TO 10%.." + errorstr);
			size.x = 0.1 * parentdimensions.x;
		}
		
	}
	try { size.y = std::stof(keys.find("SIZEY")->second); }
	catch (const std::exception& exception) {
		try { 
			size.y = (std::stof(keys.find("SIZEY%")->second) / 100);
			size.y *= parentdimensions.y;
		}
		catch (const std::exception& exc) {
			//invalid positionx. invalid positionx%.
			LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify {SIZEY,x} / {SIZEY%,x%} key. DEFAULTING TO 10%.." + errorstr);
			size.y = 0.1 * parentdimensions.x;
		}
		
	}
	try { origin.x = std::stof(keys.find("ORIGINX%")->second) / 100; }
	catch (const std::exception& exception) {
		LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to read {ORIGINX%,x%} key " + errorstr + "DEFAULTING TO 0%...");
	}
	try { origin.y = std::stof(keys.find("ORIGINY%")->second) / 100; }
	catch (const std::exception& exception) {
		LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to read {ORIGINY%,y%} key " + errorstr + "DEFAULTING TO 0%...");
	}
	try { position.x = std::stof(keys.find("POSITIONX")->second); }
	catch (const std::exception& exception) {
		try {
			position.x = (std::stof(keys.find("POSITIONX%")->second) / 100);
			position.x *= parentdimensions.x;
		}
			catch (const std::exception& exc) {
				//invalid positionx. invalid positionx%.
				LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify {POSITIONX,x} / {POSITIONX%,x%} key. DEFAULTING TO 10%.." + errorstr);
				position.x = 0.1 * parentdimensions.y;
			}
	}
	//move the position to acccount for the origin
	position.x -= (origin.x * size.x);
	if (position.x < 0) position.x = 0;
	//ensure that the element size remains clamped to the interface if overhang. 
	if (position.x + size.x >= parentdimensions.x) size.x = parentdimensions.x - position.x;
	try { position.y = std::stof(keys.find("POSITIONY")->second); }
	catch (const std::exception& exception) {
		try {
			position.y = (std::stof(keys.find("POSITIONY%")->second) / 100);
			position.y *= parentdimensions.y;
		}
		catch (const std::exception& exc) {
			//invalid positionx. invalid positionx%.
			LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify {POSITIONY,y} / {POSITIONY%,y%} key. DEFAULTING TO 10%.." + errorstr);
			position.y = 0.1 * parentdimensions.y;
		}
	}
	position.y -= (origin.y * size.y);
 	if (position.y < 0) position.y = 0;
	if (position.y + size.y >= parentdimensions.y) size.y = parentdimensions.y - position.y;
	//note that these changes aren't applied immediately but will be applied in the next update cycle.
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
	if (name == "RIGHT_SUB_PANEL") {
		int x = 4;
	}
	visual.sbg.setFillColor(currentstyle.background.sbg_color);
	visual.sbg.setOutlineThickness(currentstyle.background.outlinethickness);
	visual.sbg.setOutlineColor(currentstyle.background.outlinecolor);
	visual.text.setFillColor(currentstyle.text.textcolor);
	visual.CalibrateText(GetLocalBoundingBox(), currentstyle.text.originproportion, currentstyle.text.charactersize, currentstyle.text.localpositionproportion);
	//check if the character height exceeds the element.
	//check if the text exceeds the gui.
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

