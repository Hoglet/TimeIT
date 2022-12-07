/*
 * Ipc.cpp
 *
 *  Created on: 11 Jul 2019
 *      Author: hoglet
 */

#include "libtimeit/misc/ipc_server.h"

#include <sys/socket.h>
#include <sys/un.h>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <libtimeit/db/notifier.h>
#include <libtimeit/utils.h>

namespace libtimeit
{
using namespace std;

ipc_server::ipc_server( string name, timer_base& timer, notification_manager& op_notifier)
	:
		timer_observer(timer),
		notifier( op_notifier)
{
	socket_name = prepare_socket_dir() + name;

	unlink(socket_name.c_str());

	struct sockaddr_un server{};
	sock = socket(AF_UNIX, SOCK_STREAM, 0);  // NOLINT
	int flags = fcntl(sock, F_GETFL);                     // NOLINT
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);              // NOLINT

	if (sock < 0)
	{
		perror("opening stream socket");
	}
	else
	{
		server.sun_family = AF_UNIX;
		safe_strcpy(server.sun_path, socket_name.c_str(), sizeof(server.sun_path)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
		if (bind(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_un)))   // NOLINT
		{
			if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_un)) < 0) // NOLINT
			{
				close(sock);
				perror("connecting stream socket");
			}
		}
		else
		{
			listen(sock, 5); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
		}
	}
}

ipc_server::~ipc_server()
{
	close(sock);
	unlink(socket_name.c_str());
}

void ipc_server::poll()
{
	constexpr auto buffer_size=1024;
	char buf[buffer_size]; // NOLINT(modernize-avoid-c-arrays)
	int msg_socket = accept(sock, nullptr, nullptr);
	if (msg_socket == -1)
	{
		if (errno != EWOULDBLOCK)
		{
			perror("accept");
		}
	}
	else
	{
		ssize_t rval; // NOLINT
		do
		{
			memset(buf, '\0', sizeof(buf));   // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			if ((rval = read( msg_socket, buf, buffer_size)) < 0) // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
			{
				perror("reading stream message");
			}
		}
		while (rval > 0);

		//Always show main window. (for now)
		on_show_menu();
	}
	close(msg_socket);
}

void ipc_server::on_signal_1_second()
{
	poll();
}

void ipc_server::on_show_menu()
{
	notifier.send_notification( show_main_window, 0);
}
} /* namespace libtimeit */
