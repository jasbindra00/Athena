#include "GUIInterface.h"
#include "Utility.h"
#include "Manager_GUI.h"
#include "Window.h"
class Layers {

protected:
	sf::Sprite backgroundsprite;
	sf::RenderTexture backgroundlayer;

	sf::Sprite contentsprite;
	sf::RenderTexture contentlayer;

	sf::Sprite controlsprite;
	sf::RenderTexture controllayer;
	
public:
	Layers(const sf::Vector2f& eltsize) {
		backgroundlayer.create(eltsize.x, eltsize.y);
		backgroundsprite.setTexture(backgroundlayer.getTexture());

		contentlayer.create(eltsize.x, eltsize.y);
		contentsprite.setTexture(contentlayer.getTexture());

		controllayer.create(eltsize.x, eltsize.y);
		controlsprite.setTexture(controllayer.getTexture());
	}

	sf::RenderTexture* GetControlLayer()  { return &controllayer; }
	sf::RenderTexture* GetBackgroundLayer() { return &backgroundlayer; }
	sf::RenderTexture* GetContentLayer() { return &contentlayer; }

	sf::Sprite* GetBackgroundSprite() { return &backgroundsprite; }
	sf::Sprite* GetContentSprite() { return &contentsprite; }
	sf::Sprite* GetControlSprite() { return &controlsprite; }


};

GUIInterface::GUIInterface(GUIInterface* p, Manager_GUI* mgr, const GUIStateStyles& styles, std::stringstream& attributes) 
	:GUIElement(p, GUIType::WINDOW, styles),
	guimgr(mgr){
	if (parent == nullptr) parent = this;
	attributes >> this;
	SetContentRedraw(true);
	SetControlRedraw(true);
}
bool GUIInterface::AddElement(const std::string& eltname, std::unique_ptr<GUIElement>& elt){
	elements.emplace_back(std::make_pair(eltname, std::move(elt)));
	return true;
}
void GUIInterface::ReadIn(std::stringstream& stream) {
	sf::Vector2f position;
	sf::Vector2f size;
	stream >> position.x >> position.y >> size.x >> size.y;
	layers = std::make_unique<Layers>(size);
	SetElementSize(size);
	SetLocalPosition(position);
	CalibratePosition();
}
bool GUIInterface::RemoveElement(const std::string& eltname) {
	return true;
}
bool GUIInterface::HasElement(const std::string& eltname) const{
	return false;
}
void GUIInterface::SetLocalPosition(const sf::Vector2f& pos){
	/*
	-the background of the interface is rendered relative to the interface layers.
	

	-if the interface has a parent, then the sprites are rendered relative to its layers

	-do I need to move the visual?
	no. because the visual is always drawn relative to this interface layer.
	if i want to move the interface, i simply move the sprite.

	if i did move the visual, that would simply offset the visual relative to the layers from this interface.
	*/
	localposition = pos;

	layers->GetBackgroundSprite()->setPosition(pos);	
	layers->GetContentSprite()->setPosition(pos);
	layers->GetControlSprite()->setPosition(pos);
}
void GUIInterface::RedrawContentLayer() {
	auto contentlayer = layers->GetContentLayer();
	contentlayer->clear(sf::Color::Color(255,255,255,0));
	for (auto& element : elements) {
		if (!element.second->IsControl()) { //then it must be a content elt
			element.second->Draw(*contentlayer);
			element.second->SetRedraw(false);
		}
	}
	contentlayer->display();
	layers->GetContentSprite()->setTexture(layers->GetContentLayer()->getTexture());
	SetContentRedraw(false);
	SetParentRedraw(true);
	
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
	SetControlRedraw(false);	
	SetParentRedraw(true);
}
void GUIInterface::RedrawBackgroundLayer() {
	auto backgroundlayer = layers->GetBackgroundLayer();
	backgroundlayer->clear(sf::Color::Color(255,0,0,255));
	backgroundlayer->draw(visual.sbg);
	backgroundlayer->draw(visual.tbg);
	backgroundlayer->draw(visual.text);
	backgroundlayer->display();
	layers->GetBackgroundSprite()->setTexture(backgroundlayer->getTexture());
	SetBackgroundRedraw(false);
	SetParentRedraw(true);
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
	/*
	-responsible for setting the flags for redraw, and moving the interface;
	-responsible for updating / redrawing the layers.
	*/	
	auto mousepos = static_cast<sf::Vector2f>(sf::Mouse::getPosition(*GetGUIManager()->GetContext()->window->GetRenderWindow()));
	

	for (auto& element : elements) {
		element.second->Update(dT);
		if (dynamic_cast<GUIInterface*>(element.second.get())) { //if its a gui interface
			if (static_cast<GUIInterface*>(element.second.get())->NeedsParentRedraw()) { //check if its own layers have been redrawn (so that we can redraw this control layer)
				SetControlRedraw(true); //its layers have been redrawn, so we need to redraw our control. child interface forms the control layer.
				static_cast<GUIInterface*>(element.second.get())->SetParentRedraw(false); //reset
			}
		}
		else if (element.second->RequiresRedraw()) {
			if (element.second->IsControl()) SetControlRedraw(true);
			else SetContentRedraw(true);
			element.second->SetRedraw(false);
		}
	}
	
	if (NeedsBackgroundRedraw()) RedrawBackgroundLayer();
	if (NeedsContentRedraw()) RedrawContentLayer();
	if (NeedsControlRedraw()) RedrawControlLayer();
	
}
void GUIInterface::OnHover(){

}

void GUIInterface::OnNeutral()
{

}
void GUIInterface::OnClick(const sf::Vector2f& pos){
	SetState(GUIState::CLICKED);
	/*
	-need to check if the click lies within any of the elements.
	*/
	for (auto& element : elements) {
		if (element.second->GetActiveState() != GUIState::CLICKED) { //if it has not already been clicked
			if (element.second->Contains(pos)) {
				element.second->OnClick(pos); //if its a click on a textfield, then the manager will defocus all active interface textfields via event.
			 }
		}
	}
}

void GUIInterface::SetState(const GUIState& state)
{

}





void GUIInterface::SetElementSize(const sf::Vector2f& size) {
	GUIElement::SetElementSize(size); //set the size of the visual.
}


std::pair<bool, sf::Vector2f> GUIInterface::EltOverhangs(const GUIElement* const elt){
	auto eltpos = elt->GetLocalPosition(); //local to me.
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

