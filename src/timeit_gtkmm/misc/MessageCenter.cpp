#include "MessageCenter.h"

#include <utility>
#include "libtimeit/utils.h"
#include "libnotify/notify.h"

namespace gui
{
using namespace std;

Message::Message(MessageType type_, string header_, string message_) : type(type_), header(std::move(header_)), message(std::move(message_))
{

}


Message_center::Message_center(): receiving_thread(Glib::Thread::self())
{
	signal_message.connect(
			[this]()
			{
				this->manage_messages();
			}
			);
}



void Message_center::manage_messages()
{
	while (!message_que.empty())
	{
		Glib::Mutex::Lock lock(mutex);
		Message message = message_que.back();
		message_que.pop_back();
		lock.release();

		notify_init("Hello world!");
		auto* p_notification = notify_notification_new(message.header.c_str(), message.message.c_str(),
													   "dialog-information");
		notify_notification_show(p_notification, nullptr);
		g_object_unref(G_OBJECT(p_notification)); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
		notify_uninit();
	}
}

void Message_center::send_message(Message message)
{
	Glib::Mutex::Lock lock(mutex);
	message_que.push_front(message);
	lock.release();
	if (Glib::Thread::self() == receiving_thread)
	{
		manage_messages();
	}
	else
	{
		signal_message();
	}
}

} /* namespace Utils */
