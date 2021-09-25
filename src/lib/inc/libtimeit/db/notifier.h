#ifndef NOTIFIER_H
#define NOTIFIER_H

#include <cstdint>
#include <list>
#include <string>
#include <functional>
#include <variant>
#include <libtimeit/event_observer.h>
#include <libtimeit/db/message_type.h>

namespace libtimeit
{
using namespace std;

struct notification_message
{
	message_type        type;
	int64_t             id;
	string              name;
};

class notification_manager
{
	friend class event_observer;
public:
	void             try_send_notification(message_type type, int64_t item_id, string name = "");
	void             send_notification(message_type type, int64_t item_id, string name = "");
	void             is_enabled(bool op_enabled);
	void             send( event_type type, string headline, string message);
	void             broadcast(function<void(event_observer*)>);
	unsigned long    size();
protected:
	void attach(event_observer*);
	void detach(event_observer*);
private:
	void send_message(notification_message);

	bool enabled = true;
	bool missed_notification = false;

	list<event_observer*> observers = {};
};

}
#endif