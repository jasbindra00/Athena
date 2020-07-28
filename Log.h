#ifndef LOG_H
#define LOG_H
#include <SFML/System/Clock.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

static enum class LOGTYPE {
	ERROR = 0, STANDARDLOG
};
enum class LOCATION {
	MANAGER_SYSTEM,
	SYSTEM_ANIMATION,
	SYSTEM_COLLIDABLE,
	SYSTEM_CONTROLLABLE,
	SYSTEM_MOVEMENT,
	SYSTEM_RENDERER,
	SYSTEM_STATE,
	MESSAGE_HANDLER,
	MANAGER_EVENT,
	MANAGER_STATE,
	MANAGER_ENTITY,
	MANAGER_RESOURCE,
	MANAGER_TEXTURE,
	MANAGER_GUI,
	COMPONENT_BASE,
	COMPONENT_COLLISIONAABB,
	COMPONENT_SPRITE,
	COMPONENT_ANIMATION,
	COMPONENT_MOTION,
	COMPONENT_POSITION,
	COMPONENT_RETRIEVABLE,
	COMPONENT_STATE,
	GUI_INTERFACE,
	FILEREADER,
	STANDARDBINDING,
	GUIBINDING,
	MAP,
	LOOKUP
};

using Detail = std::pair<float, std::string>; //{time of occurence, errormsg}
using Logs = std::vector<Detail>;
using LocationLogs = std::unordered_map<LOCATION, Logs>;
struct LOG {
private:
	static std::unordered_map<LOGTYPE, LocationLogs> logs;
	static sf::Clock time;
public:
	static std::string GetLocationString(const LOCATION& loc) {
		std::string location;
		switch (loc) {
		case LOCATION::MANAGER_EVENT: {location = "MANAGER_EVENT"; break; }
		case LOCATION::MANAGER_SYSTEM: {location = "MANAGER_SYSTEM"; break; }
		case LOCATION::MANAGER_STATE: {location = "MANAGER_STATE"; break; }
		case LOCATION::MANAGER_ENTITY: {location = "MANAGER_ENTITY"; break; }
		case LOCATION::COMPONENT_SPRITE: {location = "COMPONENT_SPRITE"; break; }
									   
		}
		return location;
	}
	static void Log(const LOCATION& location,const LOGTYPE& logtype, const std::string& fcnname, const std::string& msg) {
		std::string message{
			"|TIME : " + std::to_string(time.getElapsedTime().asSeconds()) + "|" +
			"|LOCATION : " + GetLocationString(location) + "|" +
			"|FUNCTION : " + fcnname + "|" +
			"|DETAILS : " + msg + "|" };
		if (logtype == LOGTYPE::ERROR) message = "-ERROR : " + message;
		else if (logtype == LOGTYPE::STANDARDLOG) message = "-LOG : " + message;
		logs[logtype][location].push_back(Detail{time.getElapsedTime().asSeconds(), message });
		std::cout <<"\n"<< message <<"\n"<< std::endl;
	}

};
#endif