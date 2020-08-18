#ifndef GUITEXTFIELD_H
#define GUITEXTFIELD_H
#include "GUIElement.h"
#include <stdint.h>
#include <iostream>
#include <functional>
#include "EnumConverter.h"
#include "Bitmask.h"
#include <array>
#include <unordered_map>
#include "Utility.h"
namespace PredicateData {
	using PredicateCallable = std::function<bool(char)>;
	static enum class PredicateType : long long {
		LOWER_CASE_ALPHABET = 2147483648,
		UPPER_CASE_ALPHABET = 1073741824,
		NUMBER = 536870912,
		SPACE = 268435456,
		SENTENCE_PUNCTUATION = 134217728,
		FILE_NAME = 67108864,
		NULLTYPE = 0
	};
	static EnumConverter<PredicateType> predicateconverter([](const std::string& str)->PredicateType {
		if (str == "LOWER_CASE_ALPHABET") return PredicateType::LOWER_CASE_ALPHABET;
		else if (str == "UPPER_CASE_ALPHABET") return PredicateType::UPPER_CASE_ALPHABET;
		else if (str == "NUMBER") return PredicateType::NUMBER;
		else if (str == "SPACE") return PredicateType::SPACE;
		else if (str == "SENTENCE_PUNCTUATION") return PredicateType::SENTENCE_PUNCTUATION;
		else if (str == "FILE_NAME") return PredicateType::FILE_NAME;
		return PredicateType::NULLTYPE;
		});
	using PredicateMap = std::unordered_map<PredicateType, PredicateCallable>;
	static PredicateMap _InitPredicates() {
		PredicateMap tmp;
		tmp.insert(std::make_pair(PredicateType::LOWER_CASE_ALPHABET, [](const char& c)->bool {
			return (c >= 97 && c <= 122);
			}));
		tmp.insert(std::make_pair(PredicateType::UPPER_CASE_ALPHABET, [](const char& c)->bool {
			return (c >= 65 && c <= 90);
			}));
		tmp.insert(std::make_pair(PredicateType::NUMBER, [](const char& c)->bool {
			return (c >= 48 && c <= 57);
			}));
		tmp.insert(std::make_pair(PredicateType::SPACE, [](const char& c)->bool {
			return (c == ' ');
			}));
		tmp.insert(std::make_pair(PredicateType::SENTENCE_PUNCTUATION, [](const char& c)->bool {
			switch (c) {
			case ('!'): { return true; }
			case ('"'): { return true; }
			case ('('): { return true; }
			case (')'): { return true; }
			case ('`'): { return true; }
			case ('.'): { return true; }
			case ('?'): { return true; }
			case (39): { return true; }
			case (','): { return true; }
			default: {return false; }
			}
			}));
		tmp.insert(std::make_pair(PredicateType::FILE_NAME, [](const char& c)->bool {
			if ((c >= 58 && c <= 60) || c == '>' || c == '?' || c == '|' || c == '/' || c == 92 || c == '*' || c == ' ') return false;
			return true;
			}));
		return tmp;
	}
	static PredicateMap predicatemap = _InitPredicates();
}
class GUITextfield : public GUIElement {
	friend class GUIInterface;
protected:
	Bitmask predicatebitset;
	int maxchars;

	void OnNeutral() override;
	void OnHover() override;
	void OnClick(const sf::Vector2f& mousepos) override;
	virtual void OnLeave() override;
	virtual void OnRelease() override;
	void Draw(sf::RenderTexture& texture) override;
	void Update(const float& dT) override;
public:
	GUITextfield(GUIInterface* parent, const GUIStateStyles& styles, KeyProcessing::Keys& attributes);

	std::string GetTextfieldStr() const { return visual.GetTextStr(); }
	void AppendChar(const char& c);
	void PopChar();
	void SetPredicates(const Bitmask& mask) { predicatebitset = mask; }
	void SetMaxChars(const int& inp) { maxchars = (inp < 0) ? INT_MAX : inp; }
	void AddPredicate(const PredicateData::PredicateType& t) {predicatebitset.TurnOnBits(Utility::ConvertToUnderlyingType(t));}
	bool Predicate(const char& c) {
		using namespace PredicateData;
		std::unordered_map<unsigned int, PredicateType> conv = { {0,PredicateType::LOWER_CASE_ALPHABET},{1,PredicateType::UPPER_CASE_ALPHABET}, {2, PredicateType::NUMBER},{3,PredicateType::SPACE}, {4, PredicateType::SENTENCE_PUNCTUATION}, 
			{5,PredicateType::FILE_NAME} };
		bool characcepted = false;
		for (unsigned int i = 0; i < 6; ++i) {
			if (predicatebitset.GetBit(i) == true) {
				if (predicatemap[conv[i]](c) == true) return true;
			}
		}
		return characcepted;
	}
	void OnEnter() {
	}
	void ApplyFieldString(const std::string& str);
	sf::Text& GetText();
};


#endif