#ifndef MESSAGE_CENTER_H
#define MESSAGE_CENTER_H

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
	Message( MessageType op_type, string header_text, string message_text);
	const MessageType type;
	const string header;
	const string message;
};

class message_center
{
public:
	message_center();
	void send_message(Message message);
private:
	void manage_messages();
	std::deque<Message> message_que;
	Glib::Dispatcher signal_message;
	Glib::Mutex mutex;
	Glib::Thread* receiving_thread;
};

} /* namespace Utils */

#endif // MESSAGE_CENTER_H
