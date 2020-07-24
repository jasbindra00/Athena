#include "State_Game.h"
#include "Manager_State.h"
#include "Manager_Event.h"
#include "SharedContext.h"
#include <iostream>
#include "GameStateType.h"
State_Game::State_Game(Manager_State* statemgr) : State_Base(statemgr){
	statemgr->GetContext()->eventmanager->RegisterBindingCallable(GameStateType::GAME, "Spacebar", [this](EventDetails* ptr) {this->KeyPress(ptr); });
}

void State_Game::Update(const float& dT)
{

}
void State_Game::KeyPress(EventDetails* details) {
	std::cout << "Spacebar pressed" << std::endl;
}

void State_Game::draw(sf::RenderTarget& target, sf::RenderStates state) const
{
	sf::RectangleShape s;
	s.setFillColor(sf::Color::Green);
	s.setSize(sf::Vector2f{ 500,500 });
	//target.draw(s);
}

void State_Game::OnCreate()
{

}

void State_Game::OnDestroy()
{

}

void State_Game::Activate()
{

}

void State_Game::Deactivate()
{

}

void State_Game::Continue()
{

}

void State_Game::UpdateCamera()
{

}

State_Game::~State_Game()
{

}
