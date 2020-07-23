#ifndef MANAGER_TEXTURE_H
#define MANAGER_TEXTURE_H
#include "Manager_Resource.h"
#include <SFML\Graphics\Texture.hpp>
//CRTP TEXTUREMGR
using SharedTexture = SharedResource<sf::Texture>;
class Manager_Texture : public Manager_Resource<Manager_Texture, sf::Texture> { 
protected:
	friend class Manager_Resource<Manager_Texture, sf::Texture>;
	SharedTexture LoadResource(const std::string& resourcename) { //loads a resource into the active resource map
		auto foundresourceobject = FindResource(resourcename, resourceobjects);
		if (foundresourceobject != resourceobjects.end()) { //resource object already exists.
			return foundresourceobject->second; 
		}
		auto foundpath = FindResource(resourcename, resourcepaths);
		if (foundpath != resourcepaths.end()) { //path has been registered.
			auto resource = std::make_shared<sf::Texture>(); //try to create resource from registered path
			if (!resource->loadFromFile(foundpath->first)) {//resource path file is not correct
				LOG::Log(LOCATION::MANAGER_TEXTURE, LOGTYPE::ERROR, __FUNCTION__, "Could not create resource of name " + resourcename + " from path " + foundpath->second + ". Deleting path...");
				resource.reset();
				resourcepaths.erase(foundpath);
				return nullptr;
			}
			//resource successfully loaded.
			resourceobjects[resourcename] = resource;
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