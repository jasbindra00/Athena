#ifndef MANAGER_FONT_H
#define MANAGER_FONT_H
#include "Manager_Resource.h"
#include <SFML\Graphics\Font.hpp>


using SharedFont = Shared_Resource<sf::Font>;
class Manager_Font : public Manager_Resource < Manager_Font, sf::Font>{
protected:
public:
	Manager_Font(const std::string& resourcepaths):Manager_Resource<Manager_Font,sf::Font>(resourcepaths) {

	}
	SharedFont LoadResource(const std::string& resourcename) {
		auto foundresourceobject = FindResource(resourcename, resource_objects);
		if (foundresourceobject != resource_objects.end()) { //resource object already exists.
			return foundresourceobject->second;
		}
		auto foundpath = FindResource(resourcename, resourcepaths);
		if (foundpath != resourcepaths.end()) { //path has been registered.
			auto resource = std::make_shared<sf::Font>(); //try to create resource from registered path
			if (!resource->loadFromFile(foundpath->first)) {//resource path file is not correct
				LOG::Log(LOCATION::MANAGER_TEXTURE, LOGTYPE::ERROR, __FUNCTION__, "Could not create resource of name " + resourcename + " from path " + foundpath->second + ". Deleting path...");
				resource.reset();
				resourcepaths.erase(foundpath);
				return nullptr;
			}
			//resource successfully loaded.
			resource_objects[resourcename] = resource;
			return resource; //RVO
		}
		//if resource has no path
		LOG::Log(LOCATION::MANAGER_TEXTURE, LOGTYPE::ERROR, __FUNCTION__, "Could not find registered path for resource of name " + resourcename);
		return nullptr;
	}
	SharedFont RequestResource(const std::string& resname) {
		return LoadResource(resname);
	}
};



#endif