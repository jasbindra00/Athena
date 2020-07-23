#ifndef GAME_H
#define GAME_H
#include "Manager_System.h"
#include "Window.h"
#include "Manager_Event.h"
#include "Manager_State.h"
#include "SharedContext.h"
#include "State_Base.h"
#include "GUITextfield.h"
#include "Manager_GUI.h"
#include "Manager_Texture.h"
#include "Manager_Font.h"

class Game
{
protected:
	SharedContext context;
	std::unique_ptr<Manager_Event> eventmanager;
	std::unique_ptr<Manager_State> statemanager;
	std::unique_ptr<Window> window;
	std::unique_ptr<Manager_GUI> guimgr;
	std::unique_ptr<Manager_Font> fontmgr;
	std::unique_ptr<Manager_Texture> texturemgr;
	
	
	sf::Clock clock;
public:
	Game(){
		eventmanager = std::make_unique<Manager_Event>(); //event manager needs to be made before statemanager.
		context.eventmanager = eventmanager.get();
		
		statemanager = std::make_unique<Manager_State>(&context);
		window = std::make_unique<Window>(eventmanager.get(), 1000, 1000, "MyWindow");
		context.window = window.get();

		texturemgr = std::make_unique<Manager_Texture>("ResourcePaths.txt");
		context.texturemgr = texturemgr.get();
		fontmgr = std::make_unique<Manager_Font>("FontPaths.txt");
		context.fontmgr = fontmgr.get();
		
		guimgr = std::make_unique<Manager_GUI>(&context);
		guimgr->RegisterInterface(StateType::GAME, "MyInterface", "MyInterface.txt");
		guimgr->SetActiveState(StateType::GAME);
		
	}
	void Update() {
		
		float dT = clock.getElapsedTime().asSeconds();
		window->Update(dT);
		statemanager->Update(dT);
		guimgr->Update(dT);
		
	
	}
	void Draw() {
		window->GetRenderWindow()->clear();
		statemanager->Draw();
		guimgr->Draw();
		window->GetRenderWindow()->display();
	
	}
	sf::RenderWindow* GetRenderWindow() const { return window->GetRenderWindow(); }
};


#endif