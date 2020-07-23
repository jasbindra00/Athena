#ifndef SHAREDCONTEXT_H
#define SHAREDCONTEXT_H

class Manager_Event;
class Manager_Texture;
class Manager_Font;
class Window;
struct SharedContext
{
	Manager_Event* eventmanager;
	Window* window;
	Manager_Texture* texturemgr;
	Manager_Font* fontmgr;
};

#endif