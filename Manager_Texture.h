#ifndef MANAGER_TEXTURE_H
#define MANAGER_TEXTURE_H
#include "Manager_Resource.h"
#include <SFML\Graphics\Texture.hpp>
//CRTP TEXTUREMGR
class Manager_Texture : public Manager_Resource<Manager_Texture, sf::Texture> { 
protected:
	friend class Manager_Resource<Manager_Texture, sf::Texture>;

	Shared_Resource<sf::Texture> LoadResource(const std::string& resource_name) 
	{ //loads a resource into the active resource map
		sf::Texture texture;
		if (!texture.loadFromFile(resource_name)) return nullptr;
		resource_objects
		resource->loadFromFile(foundpath->first);
		resource_objects[resourcename] = resource;
		return resource; //RVO
	}
		//if resource has no path
		LOG::Log(LOCATION::MANAGER_TEXTURE, LOGTYPE::ERROR, __FUNCTION__, "Could not find registered path for resource of name " + resourcename);
		return nullptr;
	}
public:
	
	Manager_Texture(const std::string& pathfile) : Manager_Resource(pathfile) {
	}
	SharedTexture RequestResource(const std::string& resourcename) {
		return LoadResource(resourcename);
	}
	~Manager_Texture() {
	}
};



#endif