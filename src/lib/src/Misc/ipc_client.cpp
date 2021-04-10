/*
 * Ipc.cpp
 *
 *  Created on: 11 Jul 2019
 *      Author: hoglet
 */

#include "libtimeit/misc/ipc_client.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

namespace libtimeit
{
using namespace std;

IpcClient::IpcClient(string name)
{
	socketName = prepareSocketDir() + name;

}

IpcClient::~IpcClient()
{
}

void IpcClient::window2front()
{
	struct sockaddr_un server;
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
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
		if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_un)) < 0)
		{
			close(sock);
			perror("connecting stream socket");
		}
		else
		{
			const char *DATA = "{ \"command\": \"SHOW_APPLICATION\" }";
			if (write(sock, DATA, sizeof(DATA)) < 0)
			{
				perror("writing on stream socket");
			}
		}
		close(sock);
	}
}

} /* namespace libtimeit */
