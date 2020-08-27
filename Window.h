#ifndef WINDOW_H
#define WINDOW_H
#include <SFML/Graphics.hpp>
#include "Manager_State.h"
#include "Manager_Event.h"

using RenderWindow = std::unique_ptr<sf::RenderWindow>;
class Window
{
private:
	RenderWindow renderwindow;
	Manager_Event* eventmanager;
	float winwidth;
	float winheight;
	std::string winname;
public:
	Window(Manager_Event* evntmgr, const float& windowwidth, const float& windowheight, const std::string& windowname):eventmanager(evntmgr),winwidth(windowwidth), winheight(windowheight), winname(windowname) {
		//renderwindow = std::make_unique<sf::RenderWindow>(sf::VideoMode::getDesktopMode(), winname, sf::Style::Default);
		renderwindow = std::make_unique<sf::RenderWindow>(sf::VideoMode(800,800), winname, sf::Style::Default);
	}
	void Update(const float& dT) {
		sf::Event e;
		while (renderwindow->pollEvent(e)) {
			if (e.type == sf::Event::EventType::Closed) renderwindow->close();
			eventmanager->HandleEvent(e, renderwindow.get());
		}
		eventmanager->Update(renderwindow.get());
	}
	bool IsOpen() const { return renderwindow->isOpen(); }
	
	sf::RenderWindow* GetRenderWindow() { return renderwindow.get(); }
};

#endif