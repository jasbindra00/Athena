#include "Manager_System.h"
#include "System_Base.h"
#include "System_Animation.h"
#include "System_Renderer.h"
#include "System_Movement.h"
#include "System_Collidable.h"
#include "System_Controllable.h"

Manager_System::Manager_System(){
	messagehandler = std::make_unique<MessageHandler>();
	//create all systems here.
}
void Manager_System::HandleEvents(const float& dT){ //responsible for distributing entity events to those systems who are responsible for its given components
	for (auto& p : entityevents) {
		auto& eventqueue = p.second;
		EntityEventType eventtype;
		while (eventqueue.PollEvent(eventtype)) {
			for (auto& system : systemstorage) { //finding the systems which manage this entity.
				if (system.second->ManagesEntity(p.first)) {
					system.second->HandleEvent(p.first, eventtype);
				}
			}
		}
	}
}
void Manager_System::DelegateEntity(const EntityID& id, const Bitmask& b){ //responsible for finding all systems which can handle entity.
	for (auto& system : systemstorage) {
		if (system.second->MatchesRequirements(b)) system.second->AddEntity(id, b);
	}
}
void Manager_System::RemoveEntity(const EntityID& id) {
	for (auto& system : systemstorage) {
		if (system.second->ManagesEntity(id)) {
			system.second->RemoveEntity(id);
		}
	}
	auto found = entityevents.find(id);
	if (found != entityevents.end()) entityevents.erase(found); //DANGEROUS!! MAY STILL NEED TO EXEXCUTE EVENTS -> INCOMPLETE.
	//NEED TO REMOVE AFTER ALL EVENTS HAVE BEEN PROCESSED.
}
void Manager_System::ChangedEntity(const EntityID& id, const Bitmask& b) { //changed entity might result in bitmask incompatibility between entity and the system - need to find new systems.
	for (auto& system : systemstorage) {
		if (!system.second->ManagesEntity(id)) continue;
		if (!system.second->MatchesRequirements(b)) {
			system.second->RemoveEntity(id);
			DelegateEntity(id, b); //search for new systems.
			return;
		}
	}
}
void Manager_System::AddEvent(const EntityID& id, const EntityEventType& type){
	entityevents[id].InsertEvent(type);
}
MessageHandler* Manager_System::GetMessageHandler()const noexcept{ return messagehandler.get(); }


enum class Attribute_Type
{
	Element_Size, Texture_Rect, Fill_Color, Outline_Color, Outline_Thickness, Origin, Local_Position, Scale, Resource_Name, Rotation, Hidden,

	Character_Size, Letter_Spacing, Line_Spacing, Text_Style, Text_String
};