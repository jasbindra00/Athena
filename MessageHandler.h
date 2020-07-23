#ifndef MESSAGE_HANDLER_H
#define MESSAGE_HANDLER_H
#include "Message.h"
#include "Messenger.h"
//messages get relayed to the post office(communicator).
//the post office sends the message to the correct port.
//the port sends the message to the cities that require it(observers).

class MessageHandler{
protected:
	Messenger messenger; 
public:
	bool Subscribe(const SystemMessage& type, SystemMailbox* mailbox) {
		messenger.mailinglists[type].insert(mailbox);
	}
	bool Unsubscribe(const SystemMessage& type, SystemMailbox* mailbox){
		auto& mailinglist = messenger.mailinglists[type];
		auto foundmailbox = mailinglist.find(mailbox);
		if (foundmailbox != messenger.mailinglists[type].end()) {
			mailinglist.erase(foundmailbox);
		}
	}
	bool Dispatch(const Message& msg) {
		messenger.Broadcast(msg);
		return true;
	}
};


#endif