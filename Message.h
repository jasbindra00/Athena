#ifndef MESSAGE_H
#define MESSAGE_H
#include "EntityMessages.h"
struct Message {
	SystemMessage messagetype;
	int entityID;
	int sentfromID;
	union {
		int intdata;
		bool booldata;
	};
};
#endif