#ifndef GUIEVENTS_H
#define GUIEVENTS_H
#include <string>
#include "GUIInfo.h"
#include "GameStateType.h"
namespace GUIEventData {
	static enum class GUIEventType {
		HOVER, CLICK, RELEASE, LEAVE, NULLTYPE
	};
	static EnumConverter<GUIEventType> converter([](const std::string& str)->GUIEventType {
		if (str == "HOVER") return GUIEventType::HOVER;
		else if (str == "CLICK") return GUIEventType::CLICK;
		else if (str == "RELEASE") return GUIEventType::RELEASE;
		else if (str == "LEAVE") return GUIEventType::LEAVE;
		return GUIEventType::NULLTYPE;
		});
	struct GUIEventInfo {
		std::string interfacehierarchy;
		GUIState elementstate;
		GUIEventType eventtype;
	};
}
#endif