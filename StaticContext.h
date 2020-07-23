#ifndef STATIC_CONTEXT_H
#define STATIC_CONTEXT_H

extern class Manager_Texture;
extern class Manager_Font;
extern class Manager_Event;
extern class Manager_State;
namespace Context {
	static Manager_Texture* texturemgr;
	static Manager_Font* fontmgr;
	static Manager_Event* eventmgr;
	static Manager_State* statemgr;
}


#endif