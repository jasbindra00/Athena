#ifndef GUITEXTFIELD_H
#define GUITEXTFIELD_H
#include "GUIElement.h"

#include <functional>
#include "EnumConverter.h"
#include "Bitmask.h"
#include <array>
#include "Utility.h"

using Utility::CharacterCheck::STRING_PREDICATE;
class GUITextfield : public GUIElement {
	friend class GUIInterface;
protected:
	Bitmask predicatebitset;
	int maxchars{ INT_MAX };
	std::array<std::string, 3> defaulttextstrings;
	void OnNeutral() override;
	void OnHover() override;
	void OnClick(const sf::Vector2f& mousepos) override;
	virtual void OnLeave() override;
	virtual void OnRelease() override;
	virtual void ReadIn(KeyProcessing::Keys& keys) override;
	void SetCurrentStateString(const std::string& str);
public:
	GUITextfield(GUIInterface* parent);
	
	std::string GetCurrentStateString() const { return defaulttextstrings.at(static_cast<int>(activestate)); }
	std::string GetTextfieldString(); //MAKE CONST
	void AppendChar(const char& c);
	void PopChar();
	void SetPredicates(const Bitmask& mask) { predicatebitset = mask; }
	void SetMaxChars(const int& inp) { maxchars = (inp < 0) ? INT_MAX : inp; }
	void AddPredicate(const STRING_PREDICATE& t) {predicatebitset.TurnOnBits(Utility::ConvertToUnderlyingType(t));}
	bool Predicate(const char& c) {
		return Utility::CharacterCheck::Predicate(predicatebitset, c);
	}
	void OnEnter() {
	}

};


#endif