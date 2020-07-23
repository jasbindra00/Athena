#ifndef UTILITY_H
#define UTILITY_H
#define RUNNING_WINDOWS
#include "Log.h"
#include <type_traits>
#include <string>
#include <algorithm>
#include <sstream>
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
	
}
#endif							