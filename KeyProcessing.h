#ifndef KEYPROCESSING_H
#define KEYPROCESSING_H
#include <vector>
#include <string>
#include "StreamAttributes.h"
namespace KeyProcessing {
	static std::vector<std::string> SeparateKeys(const std::string& line) {
		return std::vector<std::string>{};
	}
	static std::vector<std::string> SeparateKeys(Attributes& stream) {
		return std::vector<std::string>{};
		
	}
	static std::vector<int> KeyValsToIntegrals(const std::string& key) {
		return std::vector<int>{};
	}
	static std::vector<std::string> KeyValsToStrings(const std::string& key) {
		return std::vector<std::string>{};
	}
	static std::string KeyValsToString(const std::string& key) {
		return std::string{};
	}
};




#endif