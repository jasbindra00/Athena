#ifndef MANAGED_RESOURCE_H
#define MANAGED_RESOURCE_H
#include <type_traits>
class Manager_Texture;
class Manager_Font;
namespace sf {
	class Font;
	class Texture;
}
namespace ManagedResourceData {
	template<typename RESOURCE>
	using IS_TEXTURE = std::is_same<typename std::decay_t<RESOURCE>, sf::Texture>;
	template<typename RESOURCE>
	using IS_FONT = std::is_same<typename std::decay_t<RESOURCE>, sf::Font>;
	template<typename RESOURCE>
	using ENABLE_IF_MANAGED_RESOURCE = std::enable_if<IS_TEXTURE<RESOURCE>::value || IS_FONT<RESOURCE>::value>;
	template<typename RESOURCE, typename  = typename ENABLE_IF_MANAGED_RESOURCE<RESOURCE>::type>
	using DEDUCE_RESOURCE_MANAGER_TYPE = std::conditional_t<IS_TEXTURE<RESOURCE>::value, Manager_Texture*, Manager_Font*>;
}

#endif