#include "GUIInterface.h"
#include "GUILayerData.h"
#include "Manager_GUI.h"
#include "Window.h"
#include <iostream>




GUIInterface::GUIInterface(GUIInterface* p, Manager_GUI* mgr)
	:guimgr(mgr), GUIElement(p, GUIType::WINDOW, GUILayerType::CONTENT) {
	layers = std::make_unique<GUILayerData::GUILayers>();//init the size to the user defined visual size.
}
void GUIInterface::SetHidden(const bool& inp) const{
	GUIElement::SetHidden(inp);
	layers->QueueParentRedraw();
}

void GUIInterface::SetPosition(const sf::Vector2f& pos) {
	layers->QueuePosition(pos); 
}
void GUIInterface::SetSize(const sf::Vector2f& size) { 
	GUIElement::SetSize(size);
	layers->QueueSize(size); 
}
bool GUIInterface::AddElement(const std::string& eltname, std::unique_ptr<GUIElement>& elt) {
	std::pair<bool, GUIElementIterator> found = GetElement(eltname);
	if (found.first) return false;	
	elements.emplace_back(std::make_pair(eltname, std::move(elt)));
	return true;
}
bool GUIInterface::RemoveElement(const std::string& eltname) {
	std::pair<bool, GUIElementIterator> found = GetElement(eltname);
	if (found.first) {
		switch (found.second->second->GetLayerType()) {
		case GUILayerType::BACKGROUND: {
			
		}
		case GUILayerType::CONTENT: {

		}
		case GUILayerType::CONTROL: {

		}
		}
		
		layers->QueueLayerRedraw(found.second->second->GetLayerType());
		elements.erase(found.second);
		return true;
	}
	return false;
}
const bool& GUIInterface::PendingParentRedraw() const { return layers->PendingParentRedraw(); }
void GUIInterface::SetState(const GUIState& state){
	GUIElement::SetState(state);
	//redrawing one layer will trigger an entire redraw to its parent.

	layers->QueueLayerRedraw(GUILayerType::BACKGROUND); //visual has been redrawn.
	//the interface as an entity has changed appearance. it must be redrawn to its parent content layer.
	if (parent) parent->layers->QueueLayerRedraw(GUILayerType::CONTENT);
	//if it does not have a parent, then it still needs 
}
void GUIInterface::DrawToLayer(const GUILayerType& layer, const sf::Drawable& drawable){
	layers->DrawToLayer(layer,drawable);
}
void GUIInterface::Draw(sf::RenderTarget& target, const bool& toparent){
	layers->RefreshChanges(*visual, elements);
	layers->Rasterize(target, toparent);
}
void GUIInterface::Update(const float& dT) {
	auto mouseposition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*guimgr->GetContext()->window->GetRenderWindow()));
	GUIElement::Update(dT); //apply any pending movement / size changes / state changes
	for (auto& element : elements) {
		if (element.second->IsHidden()) continue;
		if (element.second->Contains(mouseposition) && element.second->GetActiveState() == GUIState::NEUTRAL) element.second->OnHover();
		else if (element.second->GetActiveState() == GUIState::FOCUSED) element.second->OnLeave();
		element.second->Update(dT);
		//non interfaces have no way of communicating with layers.
		//this may be needed if the user produces an additional change to the 
// 		if ((element.second->GetType() != GUIType::WINDOW && element.second->PendingParentRedraw())) { //RESOLVE VIA VIRTUAL 
// 			layers->QueueLayerRedraw(element.second->GetLayerType());
// 		}
	}
}
void GUIInterface::ReadIn(KeyProcessing::Keys& keys){ //called by user at arbritary point in the future.
	GUIElement::ReadIn(keys);

}
void GUIInterface::OnElementCreate(Manager_Texture* texturemgr, Manager_Font* fontmgr, KeyProcessing::Keys& attributes, const GUIStateStyles& stylemap){ 
	GUIElement::OnElementCreate(texturemgr, fontmgr, attributes, stylemap);
	layers->OnCreate(visual->GetElementSize());
}
std::pair<bool, GUIElements::iterator> GUIInterface::GetElement(const std::string& elementname){
	auto it = std::find_if(elements.begin(), elements.end(), [&elementname](const auto& p) {
		return p.first == elementname;
		});
	return(it == elements.end()) ? std::make_pair(false, it) : std::make_pair(true, it);
}
void GUIInterface::OnClick(const sf::Vector2f& pos) {
	GUIElement::OnClick(pos); //dispatches event.
	for (auto& element : elements) {
		if (element.second->IsHidden()) continue;
		if (element.second->GetActiveState() != GUIState::CLICKED) { //if the element has not already been clicked
			if (element.second->Contains(pos)) {
				element.second->OnClick(pos); //if its a click on a textfield, then the manager will defocus all active interface textfields via event.
				if (element.second->GetType() == GUIType::TEXTFIELD) {
					guimgr->SetActiveTextfield(static_cast<GUITextfield*>(element.second.get()));
				}
			}
		}
	}
}
void GUIInterface::OnRelease(){
	//dispatch event here
	OnNeutral();
	for (auto& element : elements) {
		if (element.second->IsHidden()) continue;
		if (element.second->GetActiveState() == GUIState::CLICKED) {
			element.second->OnRelease();
		}
	}
	EventData::GUIEventInfo evntinfo;
	evntinfo.elementstate = GUIState::NEUTRAL;
	evntinfo.interfacehierarchy = GetHierarchyString();
	GetGUIManager()->AddGUIEvent(std::make_pair(EventData::EventType::GUI_RELEASE,std::move(evntinfo)));
}
void GUIInterface::SetEnabled(const bool& inp) const{
	//apply change to all elements.
	GUIElement::SetEnabled(inp);
	for (auto& elt : elements) {
		elt.second->SetEnabled(inp);
	}
}
void GUIInterface::DefocusStickyElements(){
	for (auto& elt : elements) {
		if (elt.second->IsHidden()) continue;
		if (elt.second->GetType() == GUIType::TEXTFIELD || elt.second->GetType() == GUIType::LABEL) { //HAVE TEXTFIELD AND LABEL INHERIT FROM STICKY INTERMEDIAT
			if (elt.second->GetActiveState() != GUIState::NEUTRAL) elt.second->OnNeutral();
		}
		else if (elt.second->GetType() == GUIType::WINDOW) {
			static_cast<GUIInterface*>(elt.second.get())->DefocusStickyElements();
		}
	}
}
std::pair<bool, sf::Vector2f> GUIInterface::EltOverhangs(const GUIElement* const elt){
	auto eltpos = elt->GetLocalPosition(); 
	auto eltsize = elt->GetSize();
	auto mysize = GetSize();
	sf::Vector2f newpos{ 0,0 };
	bool overhangs{ false };
	if (eltpos.x < 0) { newpos.x = 0; overhangs = true; }
	else if (eltpos.x + eltsize.x > mysize.x) { newpos.x = mysize.x - eltsize.x; overhangs = true; }
	if (eltpos.y < 0) { newpos.y = 0; overhangs = true; }
	else if (eltpos.y + eltsize.y > mysize.y) { newpos.y = mysize.y - eltsize.y; overhangs = true; }
	return { overhangs, newpos };
}
const sf::Vector2f& GUIInterface::GetLocalPosition() const { return layers->GetPosition(); }
GUIInterface::~GUIInterface() {

}

