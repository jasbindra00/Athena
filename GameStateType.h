#ifndef STATETYPE_H
#define STATETYPE_H
#include "EnumConverter.h"
namespace GameState {
	enum Type {
		INTRO,MAINMENU, OPTIONS, GAME, GAMELOST, NULLSTATE
	};
}
enum class StateType {
	INTRO, MAINMENU, OPTIONS, GAME, GAMELOST, NULLSTATE
};


#endif