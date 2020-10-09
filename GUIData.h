#ifndef GUIDATA_H
#define GUIDATA_H
#include <cstdint>
namespace GUIData {
	static enum class GUILayerType : uint8_t {
		BACKGROUND, CONTENT, CONTROL
	};
	static enum class GUIState : uint8_t {
		NEUTRAL, FOCUSED, CLICKED, NULLSTATE
	};
	static enum class GUIElementType : uint8_t {
		INTERFACE, LABEL, TEXTFIELD, SCROLLBAR
	};
}
#endif