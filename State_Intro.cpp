#include "State_Intro.h"
#include "Manager_State.h"
#include "GameStateData.h"

using GameStateData::GameStateType;
State_Intro::State_Intro(Manager_State* statemgr) :State_Base(statemgr) {
	transparency = false;
	transcendency = false;
	
}
void State_Intro::OnDestroy(){

}

void State_Intro::OnCreate(){
	shape.setFillColor(sf::Color::Red);
	shape.setSize(sf::Vector2f{ 300,300 });
	shape.setPosition(sf::Vector2f{ 200,200 });
	tick.restart();
}

void State_Intro::Update(const float& dT){
	if (tick.getElapsedTime().asSeconds() > 1) {
		Continue();
	}

}

void State_Intro::draw(sf::RenderTarget& target, sf::RenderStates state) const{
	target.draw(shape);
}

void State_Intro::UpdateCamera()
{

}

void State_Intro::Continue(){
	statemgr->QueueInsertion(GameStateType::GAME);
	statemgr->QueueDestruction(GameStateType::INTRO);
	
}

void State_Intro::Activate()
{

}

void State_Intro::Deactivate()
{

}
