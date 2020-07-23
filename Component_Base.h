#ifndef COMPONENT_BASE_H
#define COMPONENT_BASE_H
#include <sstream>
#include "ECSTypes.h"
#include "Log.h"
class Manager_Entity;
class Component_Base {
protected:
	ComponentType type;
	int attributerequirement{ 0 }; //the number of attributes that a component expects to read.
public:
	Component_Base(const ComponentType& t) :type(t) {
	}
	inline ComponentType GetType() const { return type; }
	inline int GetAttributeRequirement() const noexcept { return attributerequirement; }
	inline void SetAttributeRequirement(const int& v) noexcept { attributerequirement = v; }
	virtual void ReadIn(std::stringstream& stream) = 0;
	friend std::stringstream& operator >>(std::stringstream& stream, Component_Base& b) {
		//for reading the initialising values for the given component. 
		//we must check if the stream contains enough values to initialise the component, by counting the attributes.
		auto CountStreamAttributes = [&b, &stream]()->int {
			int nattributes{ 0 };
			for (std::string attribute; !stream.eof(); stream >> attribute) {
				++nattributes;
			}
			stream.seekg(0); //reset input from pos.
			return nattributes;
		};
		auto nstreamattributes = CountStreamAttributes();
		if (nstreamattributes != b.GetAttributeRequirement()) {
			LOG::Log(LOCATION::COMPONENT_BASE, LOGTYPE::ERROR, __FUNCTION__, "Component attribute requirements mismatch. | Required attributes = " + std::to_string(b.attributerequirement) + "| Input attributes = " + std::to_string(nstreamattributes));
		}
		else b.ReadIn(stream);
		return stream;
	}
	virtual ~Component_Base() {

	}
};



#endif