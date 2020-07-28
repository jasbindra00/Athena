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
		auto reduction = key;
		std::string extracted;
		reduction.erase(std::remove_if(reduction.begin(), reduction.end(), [&extracted](char& c) {
			if (c != '{') return true;
			if (c != ',') { extracted.push_back(' '); return true;}
			if (c != '}') return true;
			else extracted.push_back(c);
			return false;
			}), reduction.end());
		if (reduction != "{,}") return std::make_pair(false, std::move(reduction));
		//check if the keys are alpha numeric.
		Attributes stream(extracted);




	}
	static std::pair<bool, std::string> CheckKeySyntax(const std::string& key, const bool& arg1str, const bool& arg2str) {
		auto reduction = ToLowerString(key);
		bool firstarg = true;
		auto foundseperation = std::find(reduction.begin(), reduction.end(), ',');
		if (foundseperation == reduction.end()) return std::make_pair(false, std::string{});
		std::string extractedattributes;
		reduction.erase(std::remove_if(reduction.begin(), reduction.end(), [&arg1str, &arg2str, &firstarg,&extractedattributes](char& c) {
			if ((firstarg && arg1str) || (!firstarg && arg2str)) {//if attribute type is str
				if (IsLetter(c)) {
					extractedattributes.push_back(c);
					return true; //remove if letter
				}
			}
			if ((firstarg && !arg1str) || (!firstarg && !arg2str)) { //if attribute type in num
				if (IsNumber(c)) {
					extractedattributes.push_back(c);
					return true; //remove if num
				}
			}
			if (c == ',') {
				firstarg = false;
				extractedattributes.push_back(' ');
			}
			return false;
			}), reduction.end());
		return (reduction == "{,}") ? std::make_pair(true, extractedattributes) : std::make_pair(false, extractedattributes);
	}
	static Attributes ExtractAttributesToStream(const std::string& key, const bool& arg1str, const bool& arg2str) {
		return Attributes(CheckKeySyntax(key, arg1str, arg2str).second);
	}

}
#endif