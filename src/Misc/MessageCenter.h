#ifndef MESSAGECENTER_H_
#define MESSAGECENTER_H_

#include <string>
#include <list>
#include <deque>
#include <glibmm.h>

namespace Utils
{

enum MessageType
{
	ERROR_MESSAGE,
	INFO_MESSAGE
};
struct Message
{
public:
	Message(MessageType type, std::string header, std::string message);
	std::string getHeader();
	std::string getMessage();
	MessageType getType();
private:
	MessageType type;
	std::string header;
	std::string message;
};

class MessageObserver
{
public:
	virtual ~MessageObserver();
	virtual void on_message(Message& message) = 0;
};

class MessageCenter
{
public:
	MessageCenter();
	virtual ~MessageCenter();
	void attach(MessageObserver*);
	void detach(MessageObserver*);
	void sendMessage(Message message);
private:
	void on_sendMessage();
	std::list<MessageObserver*> observers;
	std::deque<Message> messageQue;
	Glib::Dispatcher signal_message;
	Glib::Mutex mutex;
	Glib::Thread* receiving_thread;
};

} /* namespace Utils */

#endif /* MESSAGECENTER_H_ */
