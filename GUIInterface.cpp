#include "GUIInterface.h"
#include "GUIInterfaceLayers.h"
#include "Manager_GUI.h"
#include "Window.h"



GUIInterface::GUIInterface(GUIInterface* p, Manager_GUI* mgr, const GUIStateStyles& styles, KeyProcessing::Keys& keys) 
	:guimgr(mgr),GUIElement(p, GUIType::WINDOW, styles, keys){ //DANGEROUS. TEXTURE INIT REQUIRES GUI MGR. INITIALISATION MAY NOT BE IN ORDER FOR GUIMGR REQUEST.
	layers = std::make_unique<GUIInterfaceLayers>(GetSize());
	MarkContentRedraw(true);
	MarkControlRedraw(true);
}
bool GUIInterface::AddElement(const std::string& eltname, std::unique_ptr<GUIElement>& elt) {
	std::pair<bool, GUIElementIterator> found = GetElement(eltname);
	if (found.first) return false;
	(elt->IsControl()) ? MarkControlRedraw(true) : MarkContentRedraw(true);
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
void GUIInterface::Draw(sf::RenderTexture& texture) { //part of another interface
	texture.draw(*layers->GetBackgroundSprite());
	texture.draw(*layers->GetContentSprite());
	texture.draw(*layers->GetControlSprite());
}
void GUIInterface::Render() {
	/*
	-if this gui interface has a parent, then we don't want to render directly to the window.
	-we need to render this directly to its texture.
	*/
	if (parent != nullptr) return; //we have already drawn our sprites onto parent layers.
	auto winptr = guimgr->GetContext()->window->GetRenderWindow();
	winptr->draw(*layers->GetBackgroundSprite());
	winptr->draw(*layers->GetContentSprite());
	winptr->draw(*layers->GetControlSprite());
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
				MarkControlRedraw(true); //its layers have been redrawn, so we need to redraw our control. child interface forms the control layer.
				static_cast<GUIInterface*>(element.second.get())->MarkRedrawToParent(false); //reset
			}
		}
		else if (element.second->RequiresBackgroundRedraw()) { //element has been changed
			if (element.second->IsControl()) MarkControlRedraw(true); //if it was a control elt, need to redraw control layer
			else MarkContentRedraw(true);
			element.second->MarkBackgroundRedraw(false);
		}
	}
	if (RequiresBackgroundRedraw()) RedrawBackgroundLayer();
	if (RequiresContentRedraw()) RedrawContentLayer();
	if (RequiresControlRedraw()) RedrawControlLayer();
}
void GUIInterface::ApplyLocalPosition(){
	layers->GetBackgroundSprite()->setPosition(localposition);
	layers->GetContentSprite()->setPosition(localposition);
	layers->GetControlSprite()->setPosition(localposition);
}

std::pair<bool, GUIElements::iterator> GUIInterface::GetElement(const std::string& elementname){
	auto it = std::find_if(elements.begin(), elements.end(), [&elementname](const auto& p) {
		return p.first == elementname;
		});
	return(it == elements.end()) ? std::make_pair(false, it) : std::make_pair(true, it);
}

void GUIInterface::RedrawContentLayer() {
	auto contentlayer = layers->GetContentLayer();
	contentlayer->clear(sf::Color::Color(255,255,255,0));
	for (auto& element : elements) {
		if (element.second->IsHidden()) continue;
		if (!element.second->IsControl()) { //then it must be a content elt
			element.second->Draw(*contentlayer);
			element.second->MarkBackgroundRedraw(false);
			if (element.second->GetType() == GUIType::TEXTFIELD) {
				auto ptr = static_cast<GUITextfield*>(element.second.get());
				if (ptr->requirestextcalibration) {
					auto& style = ptr->GetActiveStyle();
					ptr->visual.CalibrateText(ptr->GetLocalBoundingBox(), style.text.localpositionproportion, style.text.originproportion, style.text.charactersize);
					ptr->requirestextcalibration = false;
				}
			}
		}
	}
	contentlayer->display();
	layers->GetContentSprite()->setTexture(layers->GetContentLayer()->getTexture());
	MarkContentRedraw(false);
	MarkRedrawToParent(true);
}
void GUIInterface::RedrawControlLayer() {
	auto controllayer = layers->GetControlLayer();
	controllayer->clear(sf::Color::Color(255, 0, 0, 0));
	for (auto& element : elements) {
		if (element.second->IsHidden()) continue;
		if (element.second->IsControl() || dynamic_cast<GUIInterface*>(element.second.get())) { //draw nested interfaces onto the control layer.
			element.second->Draw(*controllayer);
		}
	}
	controllayer->display();
	layers->GetControlSprite()->setTexture(layers->GetControlLayer()->getTexture());
	MarkControlRedraw(false);	
	MarkRedrawToParent(true);
}
void GUIInterface::RedrawBackgroundLayer() {
	auto backgroundlayer = layers->GetBackgroundLayer();
	backgroundlayer->clear(sf::Color::Color(255,0,0,255));
	backgroundlayer->draw(visual.sbg);
	backgroundlayer->draw(visual.tbg);
	if(!statestyles[activestate].text.texthidden) backgroundlayer->draw(visual.text);
	backgroundlayer->display();
	layers->GetBackgroundSprite()->setTexture(backgroundlayer->getTexture());
	MarkBackgroundRedraw(false);
	MarkRedrawToParent(true); //if nested interface, this change in interface needs to be reflected in the layer of its parent
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

