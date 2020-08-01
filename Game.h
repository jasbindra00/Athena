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
#include "GameStateData.h"
#include "Map.h"

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
	std::unique_ptr<Map> map;
	
	sf::Clock clock;
public:
	Game(){
		texturemgr = std::make_unique<Manager_Texture>("ResourcePaths.txt");
		context.texturemgr = texturemgr.get();
		fontmgr = std::make_unique<Manager_Font>("FontPaths.txt");
		context.fontmgr = fontmgr.get();

		
		guimgr = std::make_unique<Manager_GUI>(&context);
		guimgr->CreateStateInterface(GameStateType::GAME, "MyInterface", "MyInterface.txt");
		guimgr->SetActiveState(GameStateType::GAME);

		eventmanager = std::make_unique<Manager_Event>(guimgr.get()); //event manager needs to be made before statemanager.
		context.eventmanager = eventmanager.get();
		
		statemanager = std::make_unique<Manager_State>(&context);
		window = std::make_unique<Window>(eventmanager.get(), 1000, 1000, "MyWindow");
		context.window = window.get();

		map = std::make_unique<Map>(texturemgr.get());

		

		
	}
	void Update() {
		float dT = clock.getElapsedTime().asSeconds();
		window->Update(dT);
		statemanager->Update(dT);
		guimgr->Update(dT);
		clock.restart();
		
	
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