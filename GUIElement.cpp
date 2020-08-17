#include "GUIInterface.h"
#include "Manager_Texture.h"
#include "Manager_Font.h"
#include "Manager_GUI.h"
#include "Window.h"
#include <array>


GUIElement::GUIElement(GUIInterface* p, const GUIType& t, const GUIStateStyles& stylemap, KeyProcessing::Keys& attributes) :type(t), parent(p), controlelement(false) {
	statestyles = stylemap;
	ReadIn(attributes);
	SetStyle(activestate);
}

void GUIElement::OnNeutral(){
	SetState(GUIState::NEUTRAL);
	EventData::GUIEventInfo evntinfo;
	evntinfo.interfacehierarchy = GetHierarchyString();
	GetGUIManager()->AddGUIEvent(std::make_pair(EventData::EventType::GUI_CLICK, std::move(evntinfo)));
}
void GUIElement::ApplyLocalPosition(){
	visual.SetPosition(localposition); 
}
void GUIElement::ApplySize() {
	visual.SetSize(elementsize);

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
	visual.text.setString(name);

	//OVERRIDING THE STYLEFILE ATTRIBUTES AFTER IT HAS BEEN APPLIED
	//VISUAL KEYS
	std::array<GUIState, 3> states = { GUIState::NEUTRAL, GUIState::CLICKED, GUIState::FOCUSED };
	//::CUSTOM_TEXT OVERRIDE KEY

	//REFACTOR THIS INTO OVERRIDE KEY FUNCTION
	{
		std::array<std::pair<bool, KeyProcessing::Keys::iterator>, 3> customtextoverrides;
		customtextoverrides[0] = KeyProcessing::GetKey("CUSTOM_TEXT_NEUTRAL", keys);
		customtextoverrides[1] = KeyProcessing::GetKey("CUSTOM_TEXT_CLICKED", keys);
		customtextoverrides[2] = KeyProcessing::GetKey("CUSTOM_TEXT_FOCUSED", keys);
		for (int i = 0; i < 3; ++i) {
			if (customtextoverrides[i].first) {
				std::string& str = customtextoverrides[i].second->second;
				std::replace(str.begin(), str.end(), '+', ' ');
				statestyles[states[i]].text.customtext = str;
			}
		}
		//create getstring fcn to remove spaces.
		visual.text.setString(statestyles[activestate].text.customtext);
		requirestextcalibration = true;
	}

	//::TEXTURE OVERRIDE KEYS
	
		KeyProcessing::EraseKeyOccurrences("TEXTURE_HAS_BEEN_OVERRIDEN", keys, true); //user may have defined this key. we need it for logging purposes.
		std::array<std::pair<bool, KeyProcessing::Keys::iterator>, 3> textureoverrides;
		textureoverrides[0] = KeyProcessing::GetKey("TEXTURE_NEUTRAL", keys);
		textureoverrides[1] = KeyProcessing::GetKey("TEXTURE_CLICKED", keys);
		textureoverrides[2] = KeyProcessing::GetKey("TEXTURE_FOCUSED", keys);
		// alert functions with access to these keys (element ctor) which may subsequently change the texture, that the texture should not be changed any further
		for (int i = 0; i < 3; ++i) {
			auto itpair = textureoverrides[i];
			if (itpair.first) {
				GUIState state = states[i];
				statestyles[state].background.tbg_name = itpair.second->second;
				keys.insert(std::make_pair("TEXTURE_HAS_BEEN_OVERRIDEN", std::to_string(Utility::ConvertToUnderlyingType(states[i])))); 
			}
		}
	SetElementSize(std::move(size));
	SetLocalPosition(std::move(position));
	MarkRedraw(true);
}
Manager_GUI* GUIElement::GetGUIManager() {
	if (GetType() == GUIType::WINDOW) return static_cast<GUIInterface*>(this)->guimgr; //GUITYPE::Window := GUIInterface
	return parent->guimgr;
}
void GUIElement::OnHover(){
	
}
void GUIElement::OnClick(const sf::Vector2f& mousepos){
	SetState(GUIState::CLICKED);
	EventData::GUIEventInfo evntinfo;
	evntinfo.interfacehierarchy = GetHierarchyString();
	GetGUIManager()->AddGUIEvent(std::make_pair(EventData::EventType::GUI_CLICK,std::move(evntinfo)));
}
bool GUIElement::SetState(const GUIState& state){
	if (state == activestate) return false; //no visual change.
	activestate = state;
	ReleaseStyleResources();
	SetStyle(state);
	return true;
}
void GUIElement::ApplyCurrentStyle(){
	if (type == GUIType::CHECKBOX) {
		int y = 4;
	}
	auto& currentstyle = statestyles[activestate];
	//apply dynamically allocated resources to visuals
	if (RequestVisualResource<sf::Texture>()) { //request the required resources for the font and tbg.
		visual.tbg.setTexture(visual.tbg_texture.get());
		sf::Color tmp(255, 255, 255, 255);
		visual.tbg.setFillColor(std::move(tmp));
	}
	else {//if unsucessful texture request, we don't want the default sfml error texture to show.
		sf::Color tmp(255, 255, 255, 0);
		visual.tbg.setFillColor(std::move(tmp));
	}
	if (RequestVisualResource<sf::Font>()) {
		visual.text.setFont(*visual.font);
	}
	sf::FloatRect rect = GetLocalBoundingBox();
	if (type == GUIType::WINDOW) { rect.left = 0; rect.top = 0; }
	visual.ApplyStyle(currentstyle, rect, currentstyle.text.localpositionproportion, currentstyle.text.localpositionproportion, currentstyle.text.charactersize);
	MarkRedraw(true); //now the interface knows to redraw the layer.
}
void GUIElement::SetText(const std::string& str){
	visual.text.setString(str);
	MarkRedraw(true);
}
void GUIElement::Draw(sf::RenderTexture& texture) {
	auto& currentstyle = statestyles[activestate];
	texture.draw(visual.sbg);
	texture.draw(visual.tbg);
	if(!currentstyle.text.texthidden) texture.draw(visual.text);	
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
void GUIElement::SetStyle(const GUIState& state) {//queues a pending style application
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

