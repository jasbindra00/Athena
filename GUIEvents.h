#ifndef GUIEVENTS_H
#define GUIEVENTS_H
#include <string>
#include <vector>
#include "GUIInfo.h"
#include "GameStateType.h"
class GUIInterface;
enum class GUIEventType {
	TEXTFIELDCLICK
};
struct GUIEvent {
	std::string interfacehierarchy;
	GUIState elementstate;
	GUIType elementtype;
	GUIEventType eventtype;
};



#endif