#ifndef MANAGER_RESOURCE_H
#define MANAGER_RESOURCE_H
#include <unordered_map>
#include <memory>
#include <string>
template<typename DATATYPE>
using Shared_Resource = std::shared_ptr<DATATYPE>;
template<typename DERIVED, typename DATATYPE>
class Manager_Resource
{
protected:
	std::unordered_map<std::string, Shared_Resource<DATATYPE>> resource_objects;
public:
	Manager_Resource()
	{
	}
	void RequestResourceDeallocation(const std::string& resource_name)
	{
		auto found_resource = resource_objects.find(resource_name);
		if (found_resource == resource_objects.end()) return;
		if (found_resource->second.use_count() != 1) return; //The resource is still in use from another source.
		resource_objects.erase(found_resource);
	}
	Shared_Resource<DATATYPE> RequestResource(const std::string& resourcename) { return static_cast<DERIVED*>(this)->RequestResource(resourcename); } //request an already existing resource.
};
#endif
