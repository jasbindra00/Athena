#ifndef KEYPROCESSING_H
#define KEYPROCESSING_H
#include <string>
#include <algorithm>
#include "StreamAttributes.h"
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
namespace KeyProcessing {
	using KeyPair = std::pair<std::string, std::string>;
	using Keys = std::multimap<std::string, std::string>;
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
	static std::pair<bool, KeyPair> ExtractKey(const std::string& key) {
		if (!IsKey(key)) return std::make_pair(false, KeyPair{});
		auto attributes = key;
		attributes.erase(std::remove_if(attributes.begin(), attributes.end(), [](char& c) {
			if (c == '{') return true;
			if (c == '}') return true;
			return false;
			}), attributes.end());
		attributes[attributes.find(',')] = ' ';
		Attributes keystream(attributes);
		return std::make_pair(true, KeyPair{ keystream.GetWord(), keystream.GetWord() });
	}
	static std::pair<bool, Keys::const_iterator> GetKey(const std::string& keyname,const Keys& keys) {
		auto foundkey = keys.find(keyname);
		return (foundkey == keys.end()) ? std::make_pair(false, foundkey) : std::make_pair(true, foundkey);
	}
	static Keys ExtractValidKeys(const std::string& line) {
		Keys keys;
		Attributes linestream(line);
		while (linestream.NextWord()) {
			std::pair<bool, KeyPair> isvalid = ExtractKey(linestream.ReturnWord());
			if (isvalid.first) keys.insert(std::move(isvalid.second));
		}
		return keys;
	}
	static Keys OrderKeys(const std::vector<KeyPair>& order, const Keys& keys) {
		Keys tmp;
		for (const auto& orderkey : order) {
			auto foundkey = keys.find(orderkey.first);
			(foundkey == keys.end()) ? tmp.insert(orderkey) : tmp.insert(*foundkey);
		}
		return tmp;
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
	static bool FillMissingKey(const KeyPair& missingkey, Keys& keys) {
		if (GetKey(missingkey.first, keys).first) return false;
		keys.insert(missingkey);
		return true;
	}
	static std::vector<KeyPair> FillMissingKeys(const std::vector<KeyPair>& missingkeys, Keys& keys) {
		std::vector<KeyPair> filledkeys;
		for (auto& missingkey : missingkeys) {
			if (!FillMissingKey(missingkey, keys)) continue;
			filledkeys.emplace_back(missingkey);
		}
		return filledkeys;
	}
// 	static std::vector<KeyPair> FillMissingKeys(const std::vector<KeyPair>& missingkeys, Keys& keys) {
// 		std::vector<KeyPair> filledkeys;
// 		for (const auto& key : missingkeys) {
// 			if (GetKey(key.first, keys).first) continue;
// 			keys.insert(key);
// 			filledkeys.emplace_back(key);
// 		}
// 		return filledkeys;
// 	}
	static std::string ConstructKeyStr(const std::string & arg1, const std::string & arg2) {
		auto arg1tmp = RemoveWhiteSpaces(arg1);
		auto arg2tmp = RemoveWhiteSpaces(arg2);
		return std::string{ "{" + std::move(arg1tmp) + "," + std::move(arg2tmp) + "}" };
	}
	static Attributes InsertKeysIntoStream(const Keys& keys) {
		std::string str;
		for (auto& key : keys) {
			str += ConstructKeyStr(key.first, key.second) + " ";
		}
		return Attributes(str);
	}	
}
#endif