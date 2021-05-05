#include "MessageCenter.h"
#include "libtimeit/utils.h"
#include "libnotify/notify.h"

namespace Utils
{

Message::Message(MessageType op_type, std::string op_header, std::string op_message)
{
	type = op_type;
	header = op_header;
	message = op_message;
}

std::string Message::getHeader()
{
	return header;
}

std::string Message::getMessage()
{
	return message;
}

MessageType Message::getType()
{
	return type;
}


MessageCenter::MessageCenter()
{
	receiving_thread = Glib::Thread::self();
	signal_message.connect(sigc::mem_fun(*this, &MessageCenter::manageMessages));
}



void MessageCenter::manageMessages()
{
	while (!messageQue.empty())
	{
		Glib::Mutex::Lock lock(mutex);
		Message message = messageQue.back();
		messageQue.pop_back();
		lock.release();

		notify_init("Hello world!");
		NotifyNotification* Hello = notify_notification_new(message.getHeader().c_str(), message.getMessage().c_str(),
				"dialog-information");
		notify_notification_show(Hello, NULL);
		g_object_unref(G_OBJECT(Hello));
		notify_uninit();
	}
}

void MessageCenter::sendMessage(Message message)
{
	Glib::Mutex::Lock lock(mutex);
	messageQue.push_front(message);
	lock.release();
	if (Glib::Thread::self() == receiving_thread)
	{
		manageMessages();
	}
	else
	{
		signal_message();
	}
}

} /* namespace Utils */
