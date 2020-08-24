#ifndef GUITEXTFIELD_H
#define GUITEXTFIELD_H
#include "GUIElement.h"

#include <functional>
#include "EnumConverter.h"
#include "Bitmask.h"
#include <array>
#include "Utility.h"

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
	virtual void OnElementCreate(Manager_Texture* texturemgr, Manager_Font* fontmgr, KeyProcessing::Keys& attributes, const GUIStateStyles& stylemap) override;
public:
	GUITextfield(GUIInterface* parent);
	
	std::string GetCurrentStateString() const { return defaulttextstrings.at(static_cast<int>(activestate)); }
	std::string GetTextfieldString(); //MAKE CONST
	void AppendChar(const char& c);
	void PopChar();
	void SetPredicates(const Bitmask& mask) { predicatebitset = mask; }
	void SetMaxChars(const int& inp) { maxchars = (inp < 0) ? INT_MAX : inp; }
	void AddPredicate(const Utility::CharacterCheckData::STRING_PREDICATE& t) {predicatebitset.TurnOnBits(Utility::ConvertToUnderlyingType(t));}
	bool Predicate(const char& c) { return Utility::CharacterCheckData::PredicateCheck(predicatebitset, c);}
	void OnEnter() {
	}

};


#endif