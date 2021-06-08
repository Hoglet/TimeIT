#include "libtimeit/misc/ipc_client.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <libtimeit/utils.h>
#include <libtimeit/logging.h>

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

		if(safe_strcpy(server.sun_path, socket_name.c_str(), sizeof(server.sun_path)))
		{
			logg("Path to socket is too long");
			return;
		}
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
