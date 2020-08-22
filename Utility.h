#ifndef UTILITY_H
#define UTILITY_H
#define RUNNING_WINDOWS
#include "Log.h"
#include <type_traits>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include "StreamAttributes.h"
#include "KeyProcessing.h"
#include "EnumConverter.h"
#include "Bitmask.h"

namespace Utility {
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
	typename std::enable_if<HasConstIterator<Container>::value,std::string>::type
		ConstructGUIHierarchyString(const Container& container){
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
	namespace CharacterCheck {
		static enum class STRING_PREDICATE : long long {
			LOWER_CASE_ALPHABET = 2147483648,
			UPPER_CASE_ALPHABET = 1073741824,
			NUMBER = 536870912,
			SPACE = 268435456,
			SENTENCE_PUNCTUATION = 134217728,
			FILE_NAME = 67108864,
			NULLTYPE = 0
		};
		static EnumConverter<STRING_PREDICATE> StringPredicateConv([](const std::string& str)->STRING_PREDICATE {
			if (str == "LOWER_CASE_ALPHABET") return STRING_PREDICATE::LOWER_CASE_ALPHABET;
			else if (str == "UPPER_CASE_ALPHABET") return STRING_PREDICATE::UPPER_CASE_ALPHABET;
			else if (str == "NUMBER") return STRING_PREDICATE::NUMBER;
			else if (str == "SPACE") return STRING_PREDICATE::SPACE;
			else if (str == "SENTENCE_PUNCTUATION") return STRING_PREDICATE::SENTENCE_PUNCTUATION;
			else if (str == "FILE_NAME") return STRING_PREDICATE::FILE_NAME;
			return STRING_PREDICATE::NULLTYPE;
			});
		static bool CharacterChecker(const STRING_PREDICATE& pred, const char& c) {
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

		static bool Predicate(const Bitmask& b, const char& c) {
			return true;
		}
		}


	namespace EnumChecker {
		template<typename EnumType, EnumType... Values>
		class EnumCheck;
		template<typename EnumType> class EnumCheck<EnumType>
		{
		public:
			template<typename IntType>
			static bool constexpr Is_Value(IntType) { return false; }
		};
		template<typename EnumType, EnumType V, EnumType... Next>
		class EnumCheck<EnumType, V, Next...> : private EnumCheck<EnumType, Next...>
		{
			using super = EnumCheck<EnumType, Next...>;

		public:
			template<typename IntType>
			static bool constexpr is_value(IntType v)
			{
				return v == static_cast<IntType>(V) || super::Is_Value(v);
			}
		};
	}
}
#endif							