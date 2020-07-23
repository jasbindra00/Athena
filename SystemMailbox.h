#ifndef SYSTEMMAILBOX_H
#define SYSTEMMAILBOX_H
#include "Message.h"
class SystemMailbox { //abstract base for systems : message recieve functionality.
public:
	SystemMailbox() {

	}
	virtual void SendToMailbox(const Message& l_message) = 0; 
	virtual ~SystemMailbox() {
	}
};
#endif