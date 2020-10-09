#ifndef GUITEXTFIELD_H
#define GUITEXTFIELD_H
#include "GUIElement.h"
#include "GUISticky.h"
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
	mutable std::string default_text_string;
	void OnNeutral() override;
	void OnHover() override;
	void OnClick(const sf::Vector2f& mousepos) override;
	virtual void OnLeave() override;
	virtual void OnRelease() override;
	virtual void ReadIn(KeyProcessing::Keys& keys) override;
	void SetCurrentStateString(const std::string& str);
	virtual void OnElementCreate(Manager_Texture* texturemgr, Manager_Font* fontmgr, KeyProcessing::Keys& attributes, const GUIStateStyles& stylemap) override;
	std::string GetCurrentStateString() {
		return std::get<std::string>(GetVisual().GetStyle(activestate).GetAttribute(STYLE_ATTRIBUTE::TEXT_STRING));
	}
public:
	GUITextfield(GUIInterface* parent);

	std::string GetTextfieldString(); //MAKE CONST

	void AppendChar(const char& c);
	void PopChar();
	void SetDefaultTextString(const std::string& str) const {
		default_text_string = str;
	}
	const std::string& GetDefaultTextfieldString() const{
		return default_text_string;
	}

	void SetPredicates(const Bitmask& mask) { predicatebitset = mask; }
	void SetMaxChars(const int& inp) { maxchars = (inp < 0) ? INT_MAX : inp; }
	void AddPredicate(const Utility::CharacterManipulation::STRING_PREDICATE& t) {predicatebitset.TurnOnBits(Utility::ConvertToUnderlyingType(t));}
	bool Predicate(const char& c) { return Utility::CharacterManipulation::PredicateCheck(predicatebitset, c);}
	void OnEnter() {
	}

};


#endif