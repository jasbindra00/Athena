#include "GUIInterface.h"
#include "GUIInterfaceLayers.h"
#include "Manager_GUI.h"
#include "Window.h"



GUIInterface::GUIInterface(GUIInterface* p, Manager_GUI* mgr, const GUIStateStyles& styles, KeyProcessing::Keys& keys) 
	:guimgr(mgr),GUIElement(p, GUIType::WINDOW, GUILayerType::CONTENT, styles, keys), GUIInterfaceLayers(){ //DANGEROUS. TEXTURE INIT REQUIRES GUI MGR. INITIALISATION MAY NOT BE IN ORDER FOR GUIMGR REQUEST.
	InitLayers(GetSize());
	QueueLayerRedraw<GUILayerType::CONTROL>();
	QueueLayerRedraw<GUILayerType::CONTENT>();
	//BG?
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
		(found.second->second->GetType() == GUIType::WINDOW || !found.second->second->IsControl()) ? MarkContentRedraw(true) : MarkControlRedraw(true);
		elements.erase(found.second);
		return true;
	}
	return false;
}
void GUIInterface::Update(const float& dT) {
	auto mouseposition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*guimgr->GetContext()->window->GetRenderWindow()));
	GUIElement::Update(dT); //apply any pending movement / size changes
	for (auto& element : elements) {
		if (element.second->IsHidden()) continue;
		if (element.second->Contains(mouseposition)) {
			if (element.second->GetActiveState() == GUIState::NEUTRAL) {
				element.second->OnHover();
			}
		}
		else if (element.second->GetActiveState() == GUIState::FOCUSED) element.second->OnLeave();
		element.second->Update(dT);
		if (element.second->GetType() == GUIType::WINDOW) {
			if (static_cast<GUIInterface*>(element.second.get())->RequiresParentRedraw()) { //check if its own layers have been redrawn (so that we can redraw this control layer)
				layers.QueueLayerRedraw<GUIData::GUILayerType::CONTENT>();
				 //its layers have been redrawn, so we need to redraw our content. child interface forms the content layer.
				static_cast<GUIInterface*>(element.second.get())->ResetParentRedraw(); //reset
			}
		}
		else if (element.second->QueuedRedraw()) { //element has been changed
			
		
			layers.QueueLayerRedraw<element.second->GetLayerType()>();
			QueueLayerRedraw<element.second->GetLayerType()>();
			element.second->ResetRedraw();
		}
	}
	if (QueuedRedraw()) RedrawBackgroundLayer();
	if (RequiresContentRedraw()) RedrawContentLayer();
	if (RequiresControlRedraw()) RedrawControlLayer();
}
void GUIInterface::ApplyLocalPosition() {
	layers->GetBackgroundSprite()->setPosition(GetLocalPosition());
	layers->GetContentSprite()->setPosition(GetLocalPosition());
	layers->GetControlSprite()->setPosition(GetLocalPosition());
}
std::pair<bool, GUIElements::iterator> GUIInterface::GetElement(const std::string& elementname){
	auto it = std::find_if(elements.begin(), elements.end(), [&elementname](const auto& p) {
		return p.first == elementname;
		});
	return(it == elements.end()) ? std::make_pair(false, it) : std::make_pair(true, it);
}


void GUIInterface::OnClick(const sf::Vector2f& pos){
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
	SetState(GUIState::NEUTRAL);
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

void GUIInterface::DefocusTextfields(){
	for (auto& elt : elements) {
		if (elt.second->IsHidden()) continue;
		if (elt.second->GetType() == GUIType::TEXTFIELD) {
			if (elt.second->GetActiveState() != GUIState::NEUTRAL) elt.second->OnNeutral();
		}
		else if (elt.second->GetType() == GUIType::WINDOW) {
			static_cast<GUIInterface*>(elt.second.get())->DefocusTextfields();
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
GUIInterface::~GUIInterface() {
}

