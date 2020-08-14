#ifndef GUIINFO_H
#define GUIINFO_H
#include "EnumConverter.h"

namespace GUIData {
	namespace GUIStateData {
		static enum class GUIState {
			NEUTRAL, FOCUSED, CLICKED, HOVER, NULLSTATE
		};
		static EnumConverter<GUIState> converter([](const std::string& str)->GUIState {
			if (str == "NEUTRAL") return GUIState::NEUTRAL;
			else if (str == "FOCUSED") return GUIState::FOCUSED;
			else if (str == "CLICKED") return GUIState::CLICKED;
			else if (str == "HOVER") return GUIState::HOVER;
			return GUIState::NULLSTATE;
			});
	}
	namespace GUITypeData {
		static enum class GUIType {
			LABEL, TEXTFIELD, SCROLLBAR, WINDOW, CHECKBOX, NULLTYPE
		};
		static EnumConverter<GUIType> converter([](const std::string& str)->GUIType {
			if (str == "LABEL") return GUIType::LABEL;
			else if (str == "CHECKBOX") return GUIType::CHECKBOX;
			else if (str == "TEXTFIELD") return GUIType::TEXTFIELD;
			else if (str == "SCROLLBAR") return GUIType::SCROLLBAR;
			else if (str == "WINDOW") return GUIType::WINDOW;
			return GUIType::NULLTYPE;
			});
	}

}


#endif