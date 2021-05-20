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
#include <iostream>
#include <fcntl.h>
#include <libtimeit/db/notifier.h>

namespace libtimeit
{
using namespace std;

Ipc_server::Ipc_server(string name, Timer& timer, Notifier& notifier_)
	:
	Timer_observer(timer),
	notifier(notifier_)
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
		strncpy(server.sun_path, socket_name.c_str(), sizeof(server.sun_path)); // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
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

Ipc_server::~Ipc_server()
{
	close(sock);
	unlink(socket_name.c_str());
}

void Ipc_server::poll()
{
	constexpr auto BUFFER_SIZE=1024;
	char buf[BUFFER_SIZE]; // NOLINT(modernize-avoid-c-arrays)
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
			if ((rval = read(msg_socket, buf, BUFFER_SIZE)) < 0) // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
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

void Ipc_server::on_signal_1_second()
{
	poll();
}

void Ipc_server::on_show_menu()
{
	notifier.send_notification(SHOW_MAIN_WINDOW, 0);
}
} /* namespace libtimeit */
