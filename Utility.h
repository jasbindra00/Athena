#ifndef UTILITY_H
#define UTILITY_H
#define RUNNING_WINDOWS
#include "Log.h"
#include <type_traits>
#include <string>
#include <algorithm>
#include <array>
#include <sstream>
#include <vector>
#include "StreamAttributes.h"
#include "KeyProcessing.h"
#include "EnumConverter.h"
#include "Bitmask.h"
#include "MagicEnum.h"
#include <SFML/Graphics/Color.hpp>
namespace Utility {

	struct Conversions {
		static std::pair<bool, double> ConvertToDouble(const std::string& str) {
			double res{ 0 };
			bool valid = true;
			try { res = std::stod(str); }
			catch (...) { valid = false; }
			return std::make_pair(std::move(valid), std::move(res));
		}
		static auto ProcessColor(const KeyProcessing::Keys& keys, sf::Color defaultc) {
			KeyProcessing::FoundKeys foundkeys = KeyProcessing::GetKeys({ "R", "G", "B", "A" }, keys);
			bool successful = false;
			std::array<unsigned int, 4> rgba{ defaultc.r, defaultc.g,defaultc.b,defaultc.a };
			for (int i = 0; i < 4; ++i) {
				auto& foundkey = foundkeys.at(i);
				if (foundkey.first) {
					auto res = ConvertToDouble(foundkey.second->second);
					if (res.first) {
						rgba.at(i) = std::move(res.second);
						successful = true;
					}
				}
			}
			defaultc.r = std::move(rgba.at(0));
			defaultc.g = std::move(rgba.at(1));
			defaultc.b = std::move(rgba.at(2));
			defaultc.a = std::move(rgba.at(3));
			return std::make_pair(std::move(successful), std::move(defaultc));
		}
	};

	static LOG log;
	template<typename T, typename = typename std::is_enum<T>>
	constexpr static auto ConvertToUnderlyingType(T var)->typename std::underlying_type_t<typename std::decay_t<T>> {
		return static_cast<typename std::underlying_type_t<T>>(var);
	}
	inline unsigned int CountStreamAttributes(std::istream& stream) {
		int nattributes{ 0 };
		for (std::string attribute; !stream.eof(); stream >> attribute) {
			++nattributes;
		}
		stream.seekg(0); //reset input from pos.
		return nattributes;
	}
	//helper for determining if STL container
	template<typename T>
	struct HasConstIterator{
	private:
		template<typename C> static char test(typename C::const_iterator*);
		template<typename C> static int  test(...);
	public:
		enum { value = sizeof(test<T>(0)) == sizeof(char) };
	};
	template <typename Container>
	typename std::enable_if<HasConstIterator<Container>::value,std::string>::type ConstructGUIHierarchyString(const Container& container)
	{
		if (container.cbegin() == container.cend()) return std::string{};
		std::string hierarchystr;
		for (auto& eltname : container) {
			auto x = eltname;
			if constexpr (std::is_same_v<typename std::decay_t<Container::value_type>, std::pair<const std::string,std::string>>) hierarchystr += eltname.second;
			else if constexpr (std::is_same_v<typename std::decay_t<Container::value_type>, std::string>) hierarchystr += eltname;
			hierarchystr += ' ';
		}
		if (!hierarchystr.empty() && hierarchystr.back() == ' ') hierarchystr.pop_back();
		return hierarchystr;
		}

	const bool CheckAABBOverhang(const sf::FloatRect& rect1, const sf::FloatRect& rect2) {

	}
	namespace CharacterManipulation {//REFACTOR THIS.
		static enum class STRING_PREDICATE : long long {
			LOWER_CASE_ALPHABET = 2147483648,
			UPPER_CASE_ALPHABET = 1073741824,
			NUMBER = 536870912,
			SPACE = 268435456,
			SENTENCE_PUNCTUATION = 134217728,
			FILE_NAME = 67108864,
			NULLTYPE = 0
		};
	
		static const std::unordered_map<unsigned int, STRING_PREDICATE> predmap{ {0,STRING_PREDICATE::LOWER_CASE_ALPHABET}, {1, STRING_PREDICATE::UPPER_CASE_ALPHABET},
		{2, STRING_PREDICATE::NUMBER}, {3, STRING_PREDICATE::SPACE}, {4, STRING_PREDICATE::SENTENCE_PUNCTUATION}, {5, STRING_PREDICATE::FILE_NAME} };

		static bool ValidCharacter(const STRING_PREDICATE& pred, const char& c) {
			switch (pred) {
			case STRING_PREDICATE::LOWER_CASE_ALPHABET: { return (c >= 97 && c <= 122); }
			case STRING_PREDICATE::UPPER_CASE_ALPHABET: {return (c >= 65 && c <= 90); }
			case STRING_PREDICATE::NUMBER: {return (c >= 48 && c <= 57); }
			case STRING_PREDICATE::SPACE: {return (c == ' '); }
			case STRING_PREDICATE::SENTENCE_PUNCTUATION: {
				switch (c)
				{
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

			}
			case STRING_PREDICATE::FILE_NAME: {
				if (c >= 58 && c <= 60) return false;
				switch (c) {
				case ('>'): {return false; }
				case ('?'): {return false; }
				case('|'): {return false; }
				case('/'): {return false; }
				case (92): {return false; }
				case('*'): {return false; }
				case (' '): {return false; }
				default: {return true; }
				}
			}
			}
			return false;
		}
		static bool PredicateCheck(const Bitmask& mask, const char& c) {
			bool res = false;
			for (unsigned int i = 0; i < 6; ++i) {
				if (mask.GetBit(i) == true) {
					if (ValidCharacter(predmap.at(i), c)) res = true;
				}
			}
			return res;
		}
		static std::string RemoveBackspace(std::string str) {
			str.erase(std::remove(str.begin(), str.end(), '\b'), str.end());
			for (auto it = str.begin(); it != str.end(); ++it) {
				if (*it == '\\') {
					if (it + 1 != str.end()) {
						if (*(it + 1) != 'n') continue;
						*it = '\n';
						str.erase(it + 1);
					}
				}
			}
			return str;
		}
	}

	
}
#endif							