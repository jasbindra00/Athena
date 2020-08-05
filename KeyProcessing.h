#ifndef KEYPROCESSING_H
#define KEYPROCESSING_H
#include <string>
#include <algorithm>
#include "StreamAttributes.h"
#include <vector>

namespace KeyProcessing {
	using Key = std::pair<std::string, std::string>;
	using Keys = std::vector<Key>;

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
	static std::pair<bool, std::string> VerifyKey(const std::string& key) {
		auto reduction = RemoveWhiteSpaces(key);
		std::string extracted;
		reduction.erase(std::remove_if(reduction.begin(), reduction.end(), [&extracted](char& c) {
			if (c == '{') return false;
			if (c == ',') { extracted.push_back(' '); return false; }
			if (c == '}') return false;
			else { extracted.push_back(c); return true; }
			}), reduction.end());
		return(reduction != "{,}") ? std::make_pair(false, std::move(extracted)) : std::make_pair(true, std::move(extracted));
	}
	static Keys ExtractValidKeys(const std::string& line) {
		Keys keys;
		Attributes linestream(line);
		while (linestream.NextWord()) {
			std::pair<bool, std::string> isvalid = VerifyKey(linestream.ReturnWord());
			if (isvalid.first) {
				Attributes keystream(isvalid.second);
				std::string arg1 = keystream.GetWord();
				std::string arg2 = keystream.GetWord();
				keys.emplace_back(std::make_pair(std::move(arg1), std::move(arg2)));
			}
		}
		return keys;
	}
	static Attributes DistillValuesToStream(const Keys& keys, const char& emptyplaceholder) {
		Attributes stream;
		for (const auto& key : keys) {
			if (key.second.empty()) stream << emptyplaceholder;
			else stream << key.second;
			stream << " ";
		}
	}
	static Keys SortKeys(const std::string& keytypeorder, const std::string& line, const bool& fill) {
		Keys validkeys = ExtractValidKeys(line);
		Keys sortedkeys;
		Attributes keyorderstream(keytypeorder);
		while (keyorderstream.NextWord()) {
			std::string keytype = keyorderstream.ReturnWord();
			auto keyexists = std::find_if(validkeys.begin(), validkeys.end(), [&keytype](const Key& key) {
				return key.first == keytype;
				});
			if (keyexists != validkeys.end()) sortedkeys.emplace_back(*keyexists); //key exists
			else if (fill) sortedkeys.emplace_back(Key{ keytype,"" }); //if the required key doesnt exist, fill it with an empty key.
		}
		return sortedkeys;
	}
	static std::string ConstructKey(const std::string & arg1, const std::string & arg2) {
		auto arg1tmp = RemoveWhiteSpaces(arg1);
		auto arg2tmp = RemoveWhiteSpaces(arg2);
		return std::string{ "{" + std::move(arg1tmp) + "," + std::move(arg2tmp) + "}" };
	}
	
}
#endif