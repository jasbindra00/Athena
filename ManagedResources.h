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
	template<typename T>
	using ENABLE_IF_MANAGED_RESOURCE = std::enable_if<std::is_same_v<T, sf::Font> || std::is_same_v<T, sf::Texture>>;
	template<typename T, typename  = typename ENABLE_IF_MANAGED_RESOURCE<T>::type>
	using DEDUCE_RESOURCE_MANAGER_TYPE = std::conditional_t<std::is_same_v<typename std::decay_t<T>, sf::Texture>, Manager_Texture*, Manager_Font*>;
}
#endif