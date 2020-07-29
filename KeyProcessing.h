#ifndef KEYPROCESSING_H
#define KEYPROCESSING_H
#include <string>
#include <algorithm>
#include "StreamAttributes.h"
namespace KeyProcessing {
	static std::string ToLowerString(const std::string& str) {
		auto tmp = str;
		std::for_each(tmp.begin(), tmp.end(), [](char& c) {
			c = std::tolower(c);
			});
		return tmp;
	}
	static bool IsLetter(const char& c) {
		auto tmp = tolower(c);
		return (tmp >= 97 && tmp <= 122);
	}
	static bool IsNumber(const char& c) { return (c >= 48 && c <= 57); }
	static std::string RemoveWhiteSpaces(const std::string& str) {
		auto tmp = str;
		tmp.erase(std::remove_if(tmp.begin(), tmp.end(), [](char& c) {
			return c == ' ';
			}), tmp.end());
		return tmp; //rvo
	}
	static std::string ToUpperString(const std::string& str) {
		auto tmp = str;
		std::for_each(tmp.begin(), tmp.end(), [](char& c) {
			c = std::toupper(c);
			});
		return tmp;
	}
	static bool IsOnlyNumeric(const std::string& arg) {
		bool numeric = true;
		std::for_each(arg.begin(), arg.end(), [&numeric](const char& c) {
			if (!(c >= 48 && c <= 57)) numeric = false;
			});
		return numeric;
	}
	static bool IsOnlyCharacters(const std::string& arg) {
		auto tmp = ToLowerString(arg);
		bool character = true;
		std::for_each(tmp.begin(), tmp.end(), [&character](const char& c) {
			if (!(c >= 97 && c <= 122)) character = false;
			});
		return character;
	}
	static std::pair<bool, bool> IsAlphaNumeric(const std::string& arg) {
		if (!IsOnlyNumeric(arg)) {
			if (!IsOnlyCharacters(arg)) return{ true,true };
			else return{ true,false };
		}
		return { false,true };
	}
	static std::pair<bool, std::string> CheckKeySyntax(const std::string& key) {
		auto reduction = RemoveWhiteSpaces(key);
		std::string extracted;
		reduction.erase(std::remove_if(reduction.begin(), reduction.end(), [&extracted](char& c) {
			if (c == '{') return false;
			if (c == ',') { extracted.push_back(' '); return false; }
			if (c == '}') return false;
			else { extracted.push_back(c); return true; }
			}), reduction.end());
		return (reduction != "{,}") ? std::make_pair(false, std::move(extracted)) : std::make_pair(true, std::move(extracted));
	}
	static Attributes ExtractAttributesToStream(const std::string& key) {
		return Attributes(CheckKeySyntax(key).second);
	}
	static std::string ConstructKey(const std::string& arg1, const std::string& arg2) {
		auto arg1tmp = RemoveWhiteSpaces(arg1);
		auto arg2tmp = RemoveWhiteSpaces(arg2);
		return std::string{ "{" + std::move(arg1tmp) + "," + std::move(arg2tmp) + "}" };
	}
}
#endif