/*
 * Ipc.cpp
 *
 *  Created on: 11 Jul 2019
 *      Author: hoglet
 */

#include "libtimeit/misc/ipc_client.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

namespace libtimeit
{
using namespace std;

Ipc_client::Ipc_client(string name)
{
	socket_name = prepare_socket_dir() + name;

}


void Ipc_client::window_2_front()
{
	struct sockaddr_un server{};
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	int flags = fcntl(sock, F_GETFL);                        // NOLINT
	fcntl(sock, F_SETFL, flags | O_NONBLOCK);                // NOLINT

	if (sock < 0)
	{
		perror("opening stream socket");
	}
	else
	{
		server.sun_family = AF_UNIX;

		strncpy(server.sun_path, socket_name.c_str(), sizeof(server.sun_path));    // NOLINT(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
		if (connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_un)) < 0) // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
		{
			close(sock);
			perror("connecting stream socket");
		}
		else
		{
			const char *data = R"({ "command": "SHOW_APPLICATION" })";
			if (write(sock, data, strlen(data)) < 0)
			{
				perror("writing on stream socket");
			}
		}
		close(sock);
	}
}

} /* namespace libtimeit */
