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
	std::vector<std::string> hierarchy; //parent names
	GUIState elementstate;
	GUIType elementtype;
	GUIEventType eventtype;
};



#endif