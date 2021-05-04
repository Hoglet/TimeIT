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

namespace libtimeit
{
using namespace std;

IpcServer::IpcServer(string name, Timer& timer) : Timer_observer(timer)
{
	socketName = prepareSocketDir() + name;

	unlink(socketName.c_str());

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
		strcpy(server.sun_path, socketName.c_str());
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

IpcServer::~IpcServer()
{
	close(sock);
	unlink(socketName.c_str());
}

void IpcServer::poll()
{
	constexpr auto BUFFER_SIZE=1024;
	char buf[BUFFER_SIZE];
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
		ssize_t rval=0;
		do
		{
			bzero(buf, sizeof(buf));
			if ((rval = read(msg_socket, buf, BUFFER_SIZE)) < 0)
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

void IpcServer::on_signal_1_second()
{
	poll();
}

void IpcServer::attach(Event_observer *observer)
{
	observers.push_back(observer);
}

void IpcServer::detach(Event_observer *observer)
{
	observers.remove(observer);
}

void IpcServer::on_show_menu()
{
	std::list<Event_observer*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		Event_observer *observer = *iter;
		observer->on_show_main_window();
	}
}
} /* namespace libtimeit */
