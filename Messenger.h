#ifndef MESSENGER_H
#define MESSENGER_H
#include "Message.h"
#include "SystemMailbox.h"
#include <unordered_map>
#include <unordered_set>
using Mailboxes = std::unordered_set<SystemMailbox*>; //a set of system mailboxes that will recieve the mail  in their mail box
struct Messenger{//class to send messages to mailing lists.
	std::unordered_map<SystemMessage, std::unordered_set<SystemMailbox*>> mailinglists; //set of system mailboxes
	Messenger() {
// 		mailinglists[SystemMessage::ATTACK_ACTION] = Mailboxes{};
// 		mailinglists[SystemMessage::DEAD] = Mailboxes{};
// 		mailinglists[SystemMessage::DIRECTION_CHANGED] = Mailboxes{};
// 		mailinglists[SystemMessage::IS_MOVING] = Mailboxes{};
// 		mailinglists[SystemMessage::MOVE] = Mailboxes{};
// 		mailinglists[SystemMessage::STATE_CHANGED] = Mailboxes{};
// 		mailinglists[SystemMessage::SWITCH_STATE] = Mailboxes{};
	}
	void Broadcast(const Message& msg) {
		auto messagetype = msg.messagetype;
		auto& subscribers = mailinglists[messagetype];
		for (auto& subscriber : subscribers) {
			subscriber->SendToMailbox(msg); //send the message to all those who have subscribed to it.
		}
	}
};


#endif