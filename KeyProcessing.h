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


	static bool IsKey(const std::string& key) {
		auto reduction = RemoveWhiteSpaces(key);
		std::string extracted;
		reduction.erase(std::remove_if(reduction.begin(), reduction.end(), [&extracted](char& c) {
			if (c == '{') return false;
			if (c == ',') return false; 
			if (c == '}') return false;
			return true;
			}), reduction.end());
		return reduction == "{,}";
	}
	static std::pair<bool, Key> ExtractKey(const std::string& key) {
		if (!IsKey(key)) return std::make_pair(false, Key{});
		auto attributes = key;
		attributes.erase(std::remove_if(attributes.begin(), attributes.end(), [](char& c) {
			if (c == '{') return true;
			if (c == '}') return true;
			return false;
			}), attributes.end());
		attributes[attributes.find(',')] = ' ';
		Attributes keystream(attributes);
		return std::make_pair(true, Key{ keystream.GetWord(), keystream.GetWord() });
	}
	static std::pair<bool, Keys::const_iterator> KeyExists(const std::string& keyname,const Keys& keys) {
		auto foundkey = std::find_if(keys.begin(), keys.end(), [&keyname](const Key& key) {
			return key.first == keyname;
			});
		return (foundkey == keys.end()) ? std::make_pair(false, foundkey) : std::make_pair(true, foundkey);
	}
	static Keys ExtractValidKeys(const std::string& line) {
		Keys keys;
		Attributes linestream(line);
		while (linestream.NextWord()) {
			std::pair<bool,Key> isvalid = ExtractKey(linestream.ReturnWord());
			if (isvalid.first) keys.emplace_back(std::make_pair(isvalid.second.first, isvalid.second.second));
		}
		return keys;
	}
	static Attributes DistillValuesToStream(const Keys& keys, const char& emptyplaceholder) {
		std::string str;
		for (const auto& key : keys) {
			if (key.second.empty()) str += emptyplaceholder;
			else str += key.second;
			str += ' ';
		}
		return Attributes(std::move(str));
	}
	static Keys SortKeys(const Keys& keyorder, const std::string& keyline, const bool& fill) {
		Keys validkeys = ExtractValidKeys(keyline);
		Keys sortedkeys;
		//loop through the specified order and see if the valid keys contain this given order key.
		for (auto& order : keyorder) {
			auto keyexists = std::find_if(validkeys.begin(), validkeys.end(), [&order](const Key& key) {
				return key.first == order.first;
				});
			if (keyexists != validkeys.end()) sortedkeys.emplace_back(*keyexists); //found key
			else if (fill) {
				sortedkeys.emplace_back(order);
			}
		}

		return sortedkeys;
	}
	static std::string ConstructKey(const std::string & arg1, const std::string & arg2) {
		auto arg1tmp = RemoveWhiteSpaces(arg1);
		auto arg2tmp = RemoveWhiteSpaces(arg2);
		return std::string{ "{" + std::move(arg1tmp) + "," + std::move(arg2tmp) + "}" };
	}
	static Attributes InsertKeysIntoStream(const Keys& keys) {
		std::string str;
		for (auto& key : keys) {
			str += ConstructKey(key.first, key.second) + " ";
		}
		return Attributes(str);
	}
	
}
#endif