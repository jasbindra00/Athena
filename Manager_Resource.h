#ifndef MANAGER_RESOURCE_H
#define MANAGER_RESOURCE_H
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include "Log.h"
//CRTP resource manager
//add resource path in txt file


template<typename DataType>
using SharedResource = std::shared_ptr<DataType>;

template<typename T>
using ResourceMap = std::unordered_map<std::string, T>;

using ResourcePathMap = ResourceMap<std::string>; //maps each resourcename to the file name

template<typename DataType>
using ResourceObjectMap = ResourceMap<SharedResource<DataType>>; //maps each resourcename to the object.


template<typename Derived, typename DataType>
class Manager_Resource 
{
protected:
	
	ResourcePathMap resourcepaths;
	ResourceObjectMap<DataType> resourceobjects;
	template<typename T, typename = typename std::enable_if_t<std::is_same_v<typename std::decay_t<T>, ResourcePathMap> || std::is_same_v<typename std::decay_t<T>, ResourceObjectMap<DataType>>>>
	auto FindResource(const std::string& resourcename, T&& container )->typename std::decay_t<decltype(container)>::iterator {
		return container.find(resourcename);
	}
	SharedResource<DataType> LoadResource(const std::string& resname) { //Load resource from path.
		return static_cast<Derived*>(this)->LoadResource(resname);
	}
	void RegisterResourcePaths(const std::string& pathname) {//reads text document which contains the name : path for each resource.
		std::fstream file(pathname, std::ios::in);
		if (!file.is_open()) {
			LOG::Log(LOCATION::MANAGER_RESOURCE, LOGTYPE::ERROR, __FUNCTION__, "Could not open resource path file of name " + pathname);
			return;
		}
		int linenumber{ 1 };
		while (!file.eof()) {
			std::string line;
			std::getline(file, line);
			std::stringstream wordstream;
			wordstream << line;
			std::string resourcename;
			wordstream >> resourcename;
			std::string resourcepath;
			std::getline(wordstream, resourcepath);
			if (!wordstream.eof()) {
				LOG::Log(LOCATION::MANAGER_RESOURCE, LOGTYPE::ERROR, __FUNCTION__, "Too many arguments on line " + std::to_string(linenumber));
			}
			//must test if this resource is valid by creating it.
			resourcepaths[resourcename] = resourcepath;
			auto resource = LoadResource(resourcename);
			if (resource == nullptr) continue;
			RequestResourceDealloc(resourcename); //if was able to successfully create, then deallocate it.
			++linenumber;
		}

	}

public:
	Manager_Resource(const std::string& resourcepathfile) {
		RegisterResourcePaths(resourcepathfile);
	}
	SharedResource<DataType> RequestResource(const std::string& resourcename) { return static_cast<Derived*>(this)->RequestResource(resourcename);} //request an already existing resource.
	bool RequestResourceDealloc(const std::string& resourcename) {
		auto foundresource = FindResource(resourcename, resourceobjects);
		if (foundresource == resourceobjects.end()) {
			LOG::Log(LOCATION::MANAGER_RESOURCE, LOGTYPE::ERROR, __FUNCTION__, "Could not find resource object of name " + resourcename);
			return false;
		}
		if (foundresource->second.use_count() == 1) { //if this is the stored resource in only in use by this manager, destroy it 
			resourceobjects.erase(foundresource); //shared ptr raii
			LOG::Log(LOCATION::MANAGER_RESOURCE, LOGTYPE::STANDARDLOG, __FUNCTION__, "Shared Resource of name " + resourcename + " has been fully deallocated.");
			return true;
		}
		return false; //resource is still in use by others.
	}
	~Manager_Resource() {
	}
};



#endif
