/*
 * Ipc.cpp
 *
 *  Created on: 11 Jul 2019
 *      Author: hoglet
 */

#include "libtimeit/misc/IpcServer.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

namespace libtimeit
{
using namespace std;

IpcServer::IpcServer(string name, Timer& op_timer) : timer(op_timer)
{
	socketName = prepareSocketDir() + name;

	unlink(socketName.c_str());

	struct sockaddr_un server;
	sock = socket(AF_UNIX, SOCK_STREAM, 0);
	int flags = fcntl(sock, F_GETFL);
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);

	if (sock < 0)
	{
		perror("opening stream socket");
	}
	else
	{
		server.sun_family = AF_UNIX;
		strcpy(server.sun_path, socketName.c_str());
		if (bind(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_un)))
		{
			if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_un)) < 0)
			{
				close(sock);
				perror("connecting stream socket");
			}
		}
		else
		{
			listen(sock, 5);
		}
	}
	timer.attach(this);
}

IpcServer::~IpcServer()
{
	close(sock);
	unlink(socketName.c_str());
	timer.detach(this);
}

void IpcServer::poll()
{
	int msgsock, rval;
	char buf[1024];
	msgsock = accept(sock, 0, 0);
	if (msgsock == -1)
	{
		if (errno != EWOULDBLOCK)
		{
			perror("accept");
		}
	}
	else
	{
		do
		{
			bzero(buf, sizeof(buf));
			if ((rval = read(msgsock, buf, 1024)) < 0)
			{
				perror("reading stream message");
			}
		}
		while (rval > 0);

		//Always show main window. (for now)
		on_show_menu();
	}
	close(msgsock);
}

void IpcServer::on_signal_1_second()
{
	poll();
}

void IpcServer::attach(EventObserver *observer)
{
	observers.push_back(observer);
}

void IpcServer::detach(EventObserver *observer)
{
	observers.remove(observer);
}

void IpcServer::on_show_menu()
{
	std::list<EventObserver*>::iterator iter;
	for (iter = observers.begin(); iter != observers.end(); ++iter)
	{
		EventObserver *observer = *iter;
		observer->on_action_showMainWindow();
	}
}
} /* namespace libtimeit */
