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
	static std::string ToUpperString(const std::string& str) {
		auto tmp = str;
		std::for_each(tmp.begin(), tmp.end(), [](char& c) {
			c = std::toupper(c);
			});
		return tmp;
	}
	static std::string ComputeReduction(const std::string& key, const bool& attributesarestrings) {
		auto reduction = ToLowerString(key);
		reduction.erase(std::remove_if(reduction.begin(), reduction.end(), [&attributesarestrings](char& c) {
			if (attributesarestrings) {
				if (c >= 97 && c <= 122)return true;//remove all letters.
			}
			else if(c >= 48 && c <= 57) return true;//remove all numbers.	
			return false;
			}), reduction.end());
			
		return reduction;
	}
	static bool CheckKeySyntax(const std::string& key, const bool& attributesarestrings) {
		return ComputeReduction(key, attributesarestrings) == "{,}";
	}
	static std::string ExtractAttributesToString(const std::string& key, const bool& attributesarestrings) {
		auto tmp = key;
		std::string extractedattributes;
		if (!CheckKeySyntax(key,attributesarestrings)) return std::string{};
		std::for_each(tmp.begin(), tmp.end(), [&extractedattributes, &attributesarestrings](char& c) {
			if (attributesarestrings) {
				if (c >= 97 && c <= 122) extractedattributes.push_back(c);
				else if (c >= 65 && c <= 90) extractedattributes.push_back(c);
			}
			else if (c >= 48 && c <= 57) extractedattributes.push_back(c);
			if (c == ',') extractedattributes.push_back(' ');
			});
		return extractedattributes;
	}
	static Attributes ExtractAttributesToStream(const std::string& key, const bool& attributesarestrings) {
		return Attributes(ExtractAttributesToString(key, attributesarestrings));
	}

}
#endif