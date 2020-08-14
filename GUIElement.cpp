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
void GUIElement::ReadIn(const KeyProcessing::Keys& keys) {
	//REFACTOR THIS.

	name = keys.find("ELEMENTNAME")->second;
	std::string errorstr{ " for GUIElement of name " + name };
	//need to check if the sizex% of sizey%
	//need to check if the positionx% or positiony%
	sf::Vector2f position;
	auto videomode = sf::VideoMode::getDesktopMode();
	sf::Vector2f size{ static_cast<float>(videomode.width), static_cast<float>(videomode.height) };
	sf::Vector2f origin;
	sf::Vector2f parentdimensions = (parent == nullptr) ? sf::Vector2f{ static_cast<float>(videomode.width), static_cast<float>(videomode.height) } : parent->GetSize();

	(keys.find("HIDDEN")->second == "FALSE") ? hidden = false : hidden = true;
	try { size.x = std::stof(keys.find("SIZEX")->second); }
	catch (const std::exception& exception) {
		
		try {
			size.x = std::stof(keys.find("SIZEX%")->second) / 100;
			size.x*= (parent == nullptr) ? videomode.width : parent->GetSize().x;
		}
		catch (const std::exception& exc) {
			//invalid positionx. invalid positionx%.
			LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify {SIZEX,x} / {SIZEX%,x%} key. DEFAULTING TO 10%.." + errorstr);
			size.x = 0.1*(parent == nullptr) ? videomode.width : parent->GetSize().x;
		}
		
	}
	try { size.y = std::stof(keys.find("SIZEY")->second); }
	catch (const std::exception& exception) {
		try { 
			size.y = (std::stof(keys.find("SIZEY%")->second) / 100);
			size.y*= (parent == nullptr) ? videomode.height : parent->GetSize().y; 
		}
		catch (const std::exception& exc) {
			//invalid positionx. invalid positionx%.
			LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify {SIZEY,x} / {SIZEY%,x%} key. DEFAULTING TO 10%.." + errorstr);
			size.y = 0.1*(parent == nullptr) ? videomode.height : parent->GetSize().y;
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
				position.x*= (parent == nullptr) ? videomode.width : parent->GetSize().x; }
			catch (const std::exception& exc) {
				//invalid positionx. invalid positionx%.
				LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify {POSITIONX,x} / {POSITIONX%,x%} key. DEFAULTING TO 10%.." + errorstr);
				position.x = 0.1*(parent == nullptr) ? videomode.width : parent->GetSize().x;
			}
	}

	position.x -= (origin.x * size.x);
	if (position.x < 0) position.x = 0;
	//ensure that the element size remains clamped to the interface if overhang.
	if (position.x + size.x > parentdimensions.x) size.x = parentdimensions.x - position.x;
	try { position.y = std::stof(keys.find("POSITIONY")->second); }
	catch (const std::exception& exception) {
		try {
			position.y = (std::stof(keys.find("POSITIONY%")->second) / 100);
			position.y *= (parent == nullptr) ? videomode.height : parent->GetSize().y;
		}
		catch (const std::exception& exc) {
			//invalid positionx. invalid positionx%.
			LOG::Log(LOCATION::GUIELEMENT, LOGTYPE::ERROR, __FUNCTION__, "Unable to identify {POSITIONY,y} / {POSITIONY%,y%} key. DEFAULTING TO 10%.." + errorstr);
			position.y = 0.1 * (parent == nullptr) ? videomode.height : parent->GetSize().y;
		}
	}
	position.y -= (origin.y * size.y);
 	if (position.y < 0) position.y = 0;
	if (position.y + size.y >= parentdimensions.y) size.y = parentdimensions.y - position.y;
	position.y + size.y;
	//note that these changes aren't applied immediately but will be applied in the next update cycle.
	SetElementSize(std::move(size));
	SetLocalPosition(std::move(position));
}
Manager_GUI* GUIElement::GetGUIManager() {
	if (dynamic_cast<GUIInterface*>(this)) return dynamic_cast<GUIInterface*>(this)->guimgr;
	return parent->guimgr;
}
GUIElement::GUIElement(GUIInterface* p, const GUIType& t, const GUIStateStyles& stylemap, const KeyProcessing::Keys& attributes) :type(t), parent(p), controlelement(false) {
	statestyles = stylemap;
	ReadIn(attributes);
	ApplyCurrentStyle();
}

void GUIElement::OnClick(const sf::Vector2f& mousepos){
	SetState(GUIState::CLICKED);
	EventData::GUIEventInfo evntinfo;
	evntinfo.interfacehierarchy = GetHierarchyString();
	GetGUIManager()->AddGUIEvent(std::make_pair(EventData::EventType::GUI_CLICK,std::move(evntinfo)));
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
	if (parent == nullptr) return; //if in mid initialisation
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

std::string GUIElement::GetHierarchyString() const{
	if (parent == nullptr) return name;
	std::string str(name);
	GUIInterface* mparent = parent;
	while (mparent != nullptr) {
		//str += " ";
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

