#include "MessageCenter.h"

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

MessageObserver::~MessageObserver()
{

}

MessageCenter::MessageCenter()
{
	receiving_thread = Glib::Thread::self();
	signal_message.connect(sigc::mem_fun(*this, &MessageCenter::on_sendMessage));
}

MessageCenter::~MessageCenter()
{
}

void MessageCenter::attach(MessageObserver* observer)
{
	if (observer)
	{
		observers.push_back(observer);
	}
}

void MessageCenter::detach(MessageObserver* observer)
{
	if (observer)
	{
		observers.remove(observer);
	}
}

void MessageCenter::on_sendMessage()
{
	while (!messageQue.empty())
	{
		Glib::Mutex::Lock lock(mutex);
		Message message = messageQue.back();
		messageQue.pop_back();
		lock.release();
		for (MessageObserver* observer : observers)
		{
			observer->on_message(message);
		}
	}
}

void MessageCenter::sendMessage(Message message)
{
	Glib::Mutex::Lock lock(mutex);
	messageQue.push_front(message);
	lock.release();
	if (Glib::Thread::self() == receiving_thread)
	{
		on_sendMessage();
	}
	else
	{
		signal_message();
	}
}

} /* namespace Utils */
