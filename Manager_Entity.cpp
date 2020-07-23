#include <algorithm>
#include <fstream>
#include <sstream>
#include "Manager_Entity.h"
#include "Component_Animation.h"
#include "Component_CollisionAABB.h"
#include "Component_Motion.h"
#include "Component_Sprite.h"
#include "Component_Retrievable.h"
#include "Component_Position.h"
#include "Manager_System.h"
#include "Manager_Texture.h"
Manager_Entity::Manager_Entity(Manager_System* mgr, Manager_Texture* tmgr):systemmgr(mgr), texturemgr(tmgr) {
	RegisterComponentProducer<Component_Animation>(ComponentType::ANIMATION);
	RegisterComponentProducer<Component_CollisionAABB>(ComponentType::COLLIDABLE);
	RegisterComponentProducer<Component_Motion>(ComponentType::MOTION);
	RegisterComponentProducer<Component_Sprite>(ComponentType::SPRITE);
	RegisterComponentProducer<Component_Retrievable>(ComponentType::RETRIEVABLE);
	RegisterComponentProducer<Component_Position>(ComponentType::POSITION);
}
void Manager_Entity::CreateComponent(const EntityID& entityid, const ComponentType& t, const std::string& attributes) { //private helper. creates component for an already created entity.
	try{ auto componentproducer = componentfactory.at(t);}
	catch (const std::exception& e) {
		LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::ERROR, __FUNCTION__, "Producer for component " + std::to_string(Utility::ConvertToUnderlyingType(t)) + " has not been registered.");
		return;
	}
	auto component = componentfactory[t]();
	//CONFIGURE DEFAULT INITIALISATIONS HERE.
	if (t == ComponentType::SPRITE) {
		dynamic_cast<Component_Sprite*>(component.get())->SetTextureMgr(texturemgr);
	}
	if (!attributes.empty()) { std::stringstream{ attributes } >> *component; } //initialising component with attributes
	entitycomponents[entityid].second.emplace(t, std::move(component));
}



void Manager_Entity::RegisterEntity(const std::string& entityfile) { //registers an entity configuration file.
	std::fstream file(entityfile, std::ios::in);
	if (!file.is_open()) {
		LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::ERROR, __FUNCTION__, "Could not open the entity file of name " + entityfile);
		return;
	}
	int linenumber{ 1 };
	Bitmask entitybitmask;
	ComponentAttributes attributeinfo;
	while (!file.eof()) {
		std::string line;
		std::getline(file, line);
		std::stringstream wordstream;
		wordstream << line;

		std::string word;
		wordstream >> word;
		if (word != "Component") {
			LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::ERROR, __FUNCTION__,"Entity name : " + entityfile + ". Line " + std::to_string(linenumber) + " Invalid Syntax : Entity component information should begin with 'Component' ");
			++linenumber;
			continue; //go onto the next line
		}
		wordstream >> word; //word is now component type.
		int componenttype;
		try { componenttype = std::stoi(word); }
		catch (const std::exception&) {
			LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::ERROR, __FUNCTION__, "Entity name : " + entityfile + ". Line " + std::to_string(linenumber) + " Invalid Syntax : cannot convert " + word + " into component type");
			continue;
		}
		if (entitybitmask.GetBit(componenttype)) {
			LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::ERROR, __FUNCTION__, "Entity name : " + entityfile + ". Line " + std::to_string(linenumber) + " Entity Component " + std::to_string(componenttype) + " has already been registered");
			++linenumber;
			continue;
		}
		entitybitmask.TurnOnBit(componenttype);
		std::string remainder;
		std::getline(wordstream, remainder);//remainder of stream is component attribute info.
		attributeinfo.push_back({ static_cast<ComponentType>(componenttype),std::move(remainder) });
		++linenumber;
	}
	loadedentities[entityfile] = { std::move(entitybitmask), std::move(attributeinfo) };
	LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::STANDARDLOG, __FUNCTION__, "Successfully registered entity from file " + entityfile);

}
EntityID Manager_Entity::CreateEntity(const Bitmask& b, const ComponentAttributes& attributes) { 
	entitycomponents[maxid] = std::pair<Bitmask, ComponentStorage>{ Bitmask{}, ComponentStorage{} }; //register the entity.
	int nattribute = 0;
	for (unsigned int i = 0; i < MAXIMUM_COMPONENTS; ++i) {
		if (b.GetBit(i) == true) {
			entitycomponents[maxid].first.TurnOnBit(i);
			if (!attributes.empty()) {
				auto foundattributes = std::find_if(attributes.begin(), attributes.end(), [i](const auto& p) {
					return p.first == static_cast<ComponentType>(i);
					});
				if (foundattributes == attributes.end()) {
					LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::ERROR, __FUNCTION__, "Could not fetch attribute  [" + std::to_string(nattribute) + "] for component [" + std::to_string(i) + "]");
					CreateComponent(maxid, static_cast<ComponentType>(i)); //add component without attribute intialisation
					continue;
				}
				CreateComponent(maxid, static_cast<ComponentType>(i), foundattributes->second); //else add component with attribute initialisation
				++nattribute; //attribute used up.
			}
		}
	}
	systemmgr->DelegateEntity(maxid, b); //must alert entity manager of entity creation.
	LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::STANDARDLOG, __FUNCTION__, "Successfully created an entity of ID " + std::to_string(maxid));
	++maxid;
	return maxid - 1;
} 
EntityID Manager_Entity::CreateEntity(const std::string& entityfile) {
	auto founddata = loadedentities.find(entityfile);
	if (founddata == loadedentities.end()) { //entity file has not been loaded.
		LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::ERROR, __FUNCTION__, "Entity file of name " + entityfile + " has not been registered.");
		return -1;
	}
	return CreateEntity(founddata->second.first, founddata->second.second);
}

// Component_Base* Manager_Entity::GetComponent(const EntityID& entityid, const ComponentType& t) const{
// 	auto foundentity = entitycomponents.find(entityid);
// 	if (foundentity == entitycomponents.end()) return nullptr;
// 	if (!foundentity->second.first.GetBit(Utility::ConvertToUnderlyingType(t))) return nullptr;
// 	return foundentity->second.second.at(t).get();
// }

void Manager_Entity::AddComponent(const EntityID& entityid, const ComponentType& t, const std::string& componentattributes) { //user function.
	//need to produce additional checks for entity existence.
	auto foundentity = FindEntity(entityid);
	auto& bitmask = foundentity->second.first;
	if (foundentity == entitycomponents.end()) return;
	if(bitmask.GetBit(Utility::ConvertToUnderlyingType(t))){
		LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::ERROR, __FUNCTION__, "Entity of ID " + std::to_string(entityid) + " already has a registered component " + std::to_string(Utility::ConvertToUnderlyingType(t)) + ". Overwriting component...");
	}
	else bitmask.TurnOnBit(Utility::ConvertToUnderlyingType(t));
	auto component = componentfactory[t]();
	if (!componentattributes.empty()) std::stringstream{ componentattributes } >> *component;
	entitycomponents[entityid].second.emplace(t, std::move(component));
	systemmgr->ChangedEntity(entityid, bitmask);
	LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::STANDARDLOG, __FUNCTION__, "Successfully created an entity of ID " + std::to_string(entityid));
}
void Manager_Entity::RemoveComponent(const EntityID& entityid, const ComponentType& t) {
	auto foundentity = FindEntity(entityid);
	if (foundentity == entitycomponents.end()) return;
	if (!HasComponent(entityid, t)) {//if the component doesn't exist
		Utility::log.Log(LOCATION::MANAGER_ENTITY, LOGTYPE::ERROR, __FUNCTION__, "Entity of ID " + std::to_string(entityid) + "does not have component " + std::to_string(Utility::ConvertToUnderlyingType(t)));
		return;
	}
	foundentity->second.first.ClearBit(Utility::ConvertToUnderlyingType(t)); //otherwise, clear it.
	auto& components = foundentity->second.second;
	auto foundcomponentobj = components.find(t);
	
	components.erase(foundcomponentobj); //RAII
	systemmgr->ChangedEntity(entityid, foundentity->first);
}
void Manager_Entity::RemoveEntity(const EntityID& entityid) {
	auto found = FindEntity(entityid);
	if (found == entitycomponents.end()) return;
	entitycomponents.erase(found);
	LOG::Log(LOCATION::MANAGER_ENTITY, LOGTYPE::STANDARDLOG, __FUNCTION__, "Successfully removed entity of ID " + std::to_string(entityid));
}
bool Manager_Entity::HasComponent(const EntityID& entityid, const ComponentType& component)  {
	return entitycomponents[entityid].first.GetBit(Utility::ConvertToUnderlyingType(component));
}