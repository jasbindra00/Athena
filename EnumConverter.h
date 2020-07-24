#ifndef ENUMCONVERTER_H
#define ENUMCONVERTER_H
#include <functional>
#include <type_traits>
template<typename T, typename = typename std::enable_if_t<std::is_enum_v<typename std::decay_t<T>>>>
struct EnumConverter {
	using Converter = std::function<T(std::string)>;
	Converter converter;
public:
	explicit EnumConverter(const Converter& c) :converter(c) {
	}
	T operator()(const std::string& str) {
		return converter(str);
	}
};
#endif