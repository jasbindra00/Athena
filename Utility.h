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