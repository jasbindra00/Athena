#ifndef SHAREDCONTEXT_H
#define SHAREDCONTEXT_H
#include "ManagedResources.h"
class Manager_Event;
class Manager_Texture;
class Manager_Font;
class Window;
struct SharedContext{
	Manager_Event* eventmanager;
	Window* window;
	Manager_Texture* texturemgr;
	Manager_Font* fontmgr;
	template<typename T, typename  = ManagedResourceData::ENABLE_IF_MANAGED_RESOURCE<T>::type>
	auto GetResourceManager()->ManagedResourceData::DEDUCE_RESOURCE_MANAGER_TYPE<T> {
		if constexpr (std::is_same_v<typename std::decay_t<T>, sf::Font>) return fontmgr;
		else if constexpr (std::is_same_v<typename std::decay_t<T>, sf::Texture > ) return texturemgr;
	} 
};

#endif