#ifndef MANAGER_ENTITY_H
#define MANAGER_ENTITY_H
#define MAXIMUM_COMPONENTS 32
#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include "Bitmask.h"
#include "Component_Base.h"
#include "Utility.h"


class Manager_Texture;
class Manager_System;
using EntityID = unsigned int;
using ComponentPtr = std::unique_ptr<Component_Base>;
using ComponentStorage = std::unordered_map<ComponentType, ComponentPtr>;


using EntityComponents = std::unordered_map<EntityID, std::pair<Bitmask,ComponentStorage>>;

using ComponentProducer = std::function <ComponentPtr(void)>;
using ComponentFactory = std::unordered_map<ComponentType, ComponentProducer>;; //produce a single component

using ComponentAttributes = std::vector <std::pair<ComponentType, std::string>>;//characteristics of each component from read file, stored in a string for each active component
using LoadedEntities = std::unordered_map<std::string, std::pair<Bitmask, ComponentAttributes>>;

class Manager_Entity {
protected:
	EntityID maxid;
	EntityComponents entitycomponents;
	ComponentFactory componentfactory;
	LoadedEntities loadedentities;

	Manager_Texture* texturemgr{ nullptr };
	Manager_System* systemmgr{ nullptr };

	template<typename T>
	void RegisterComponentProducer(const ComponentType& type) {
		componentfactory[type] = [this]() {return std::make_unique<T>(); };
	}
	auto FindEntity(const EntityID& entityid)->EntityComponents::iterator {
		auto foundentity = entitycomponents.find(entityid);
		if (foundentity == entitycomponents.end()) {
			LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::ERROR, __FUNCTION__, "Could not find entity of ID " + std::to_string(entityid));
		}
		return foundentity;
	}
	bool HasComponent(const EntityID& entityid, const ComponentType& component);
	void CreateComponent(const EntityID& entityid, const ComponentType& t, const std::string& componentattributes = {}); //creating a component for a new entity.
	
public:
	Manager_Entity(Manager_System* mgr, Manager_Texture* tmgr);
	void RegisterEntity(const std::string& path); //read entity file containing component(s) info
	EntityID CreateEntity(const Bitmask& b, const ComponentAttributes& attributes = ComponentAttributes{}); //add entity based on constructed bitmask.
	EntityID CreateEntity(const std::string& filename); //creates an entity based on an already registered entity file.
	
	//Component_Base* GetComponent(const EntityID& entityid, const ComponentType& t) const;
	void AddComponent(const EntityID& entityid, const ComponentType& t, const std::string& componentattributes = {}); //adds component to an already existing entity.
	void RemoveComponent(const EntityID& entityid, const ComponentType& t); 
	void RemoveEntity(const EntityID& entityid); //deletes all com

	template<typename T>
	T* GetComponent(const EntityID& entityid, const ComponentType& t)  {
		auto foundentity = FindEntity(entityid);
		if (foundentity == entitycomponents.end()) return nullptr;
		auto& bitmask = foundentity->second.first;
		if (!bitmask.GetBit(Utility::ConvertToUnderlyingType(t))) return nullptr;
		auto componentptr = foundentity->second.second.at(t).get();
		return dynamic_cast<T*>(componentptr);
	}
	
};

#endif