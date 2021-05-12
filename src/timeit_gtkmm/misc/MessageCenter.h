#ifndef MESSAGE_CENTER_H_
#define MESSAGE_CENTER_H_

#include <string>
#include <list>
#include <deque>
#include <glibmm.h>

namespace gui
{

using namespace std;

enum MessageType
{
	ERROR_MESSAGE,
	INFO_MESSAGE
};
struct Message
{
	Message(MessageType type_, string header_, string message_);
	const MessageType type;
	const string header;
	const string message;
};

class Message_center
{
public:
	Message_center();
	void send_message(Message message);
private:
	void manage_messages();
	std::deque<Message> message_que;
	Glib::Dispatcher signal_message;
	Glib::Mutex mutex;
	Glib::Thread* receiving_thread;
};

} /* namespace Utils */

#endif /* MESSAGE_CENTER_H_ */
