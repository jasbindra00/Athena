#include "GUIInterface.h"
#include "GUIInterfaceLayers.h"
#include "Manager_GUI.h"
#include "Window.h"



GUIInterface::GUIInterface(GUIInterface* p, Manager_GUI* mgr, const GUIStateStyles& styles, std::stringstream& stream) 
	:GUIElement(p, GUIType::WINDOW, styles, stream),
	guimgr(mgr){
	if (parent == nullptr) parent = this;
	layers = std::make_unique<GUIInterfaceLayers>(GetSize());
	MarkContentRedraw(true);
	MarkControlRedraw(true);
}
bool GUIInterface::AddElement(const std::string& eltname, std::unique_ptr<GUIElement>& elt){
	auto eltexists = std::find_if(elements.begin(), elements.end(), [eltname](const auto& p) {
		return p.first == eltname;
		});
	if (eltexists != elements.end()) return false;
	elt->SetParent(this);
	elements.emplace_back(std::make_pair(eltname, std::move(elt)));
	return true;
}
void GUIInterface::ReadIn(std::stringstream& stream) {
	GUIElement::ReadIn(stream);
}
bool GUIInterface::RemoveElement(const std::string& eltname) {
	return true;
}
void GUIInterface::ApplyLocalPosition(){
	layers->GetBackgroundSprite()->setPosition(localposition);
	layers->GetContentSprite()->setPosition(localposition);
	layers->GetControlSprite()->setPosition(localposition);
}
void GUIInterface::ApplySize() {
	GUIElement::ApplySize();
}
void GUIInterface::RedrawContentLayer() {
	auto contentlayer = layers->GetContentLayer();
	contentlayer->clear(sf::Color::Color(255,255,255,0));
	for (auto& element : elements) {
		if (!element.second->IsControl()) { //then it must be a content elt
			element.second->Draw(*contentlayer);
			element.second->MarkRedraw(false);
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
	backgroundlayer->draw(visual.text);
	backgroundlayer->display();
	layers->GetBackgroundSprite()->setTexture(backgroundlayer->getTexture());
	MarkBackgroundRedraw(false);
	MarkRedrawToParent(true); //if nested interface, this change in interface needs to be reflected in the layer of its parent
}
void GUIInterface::Draw(sf::RenderTexture& texture) { //part of another interface
	texture.draw(*layers->GetBackgroundSprite());
	texture.draw(*layers->GetContentSprite());
	texture.draw(*layers->GetControlSprite());
}
void GUIInterface::Render(){
	/*
	-if this gui interface has a parent, then we don't want to render directly to the window.
	-we need to render this directly to its texture.
	*/
	if (parent != this) return; //we have already drawn our sprites onto parent layers.
	auto winptr = guimgr->GetContext()->window->GetRenderWindow();
	winptr->draw(*layers->GetBackgroundSprite());
	winptr->draw(*layers->GetContentSprite());
	winptr->draw(*layers->GetControlSprite());
}
void GUIInterface::Update(const float& dT){
	auto mouseposition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*guimgr->GetContext()->window->GetRenderWindow()));
	GUIElement::Update(dT); //apply any pending movement / size changes
	for (auto& element : elements) {
		if (element.second->Contains(mouseposition)) {
			if (element.second->GetActiveState() == GUIState::NEUTRAL) {
				element.second->OnHover();
			}
		}
		else if (element.second->GetActiveState() == GUIState::FOCUSED) element.second->OnLeave();
		element.second->Update(dT);
		if (element.second->GetType() == GUIType::WINDOW){
			if (static_cast<GUIInterface*>(element.second.get())->RequiresParentRedraw()) { //check if its own layers have been redrawn (so that we can redraw this control layer)
				MarkControlRedraw(true); //its layers have been redrawn, so we need to redraw our control. child interface forms the control layer.
				static_cast<GUIInterface*>(element.second.get())->MarkRedrawToParent(false); //reset
			}
		}
		else if (element.second->RequiresRedraw()) { //element has been changed
			if (element.second->IsControl()) MarkControlRedraw(true); //if it was a control elt, need to redraw control layer
			else MarkContentRedraw(true);
			element.second->MarkRedraw(false);
		}
	}
	if (RequiresBackgroundRedraw()) RedrawBackgroundLayer(); 
	if (RequiresContentRedraw()) RedrawContentLayer();
	if (RequiresControlRedraw()) RedrawControlLayer();
}
void GUIInterface::OnHover(){

}

void GUIInterface::OnNeutral(){

}
void GUIInterface::OnClick(const sf::Vector2f& pos){
	SetState(GUIState::CLICKED);
	for (auto& element : elements) {
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
	SetState(GUIState::NEUTRAL);
	for (auto& element : elements) {
		if (element.second->GetActiveState() == GUIState::CLICKED) {
			element.second->OnRelease();
			std::cout << "element release" << std::endl;
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


Manager_GUI* GUIInterface::GetGUIManager() const { return guimgr; }
GUIInterface::~GUIInterface() {
}

