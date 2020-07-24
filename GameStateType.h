#ifndef STATETYPE_H
#define STATETYPE_H
#include "EnumConverter.h"

namespace GameState {
	static enum class GameStateType {
		INTRO,MAINMENU, OPTIONS, GAME, GAMELOST, NULLSTATE
	};
	static EnumConverter<GameStateType> converter([](const std::string& str)->GameStateType {
		if (str == "INTRO") return GameStateType::INTRO;
		else if (str == "MAINMENU") return GameStateType::MAINMENU;
		else if (str == "OPTIONS") return GameStateType::OPTIONS;
		else if (str == "GAME") return GameStateType::GAME;
		else if (str == "GAMELOST") return GameStateType::GAMELOST;
		return GameStateType::NULLSTATE;
		});
}
using GameState::GameStateType;
#endif