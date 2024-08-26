#include "./metroid.h"
#include <SDL2/SDL_mutex.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 8080
#define BUFFER_SIZE 2048

static bool	set_nonblocking_mode(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		perror("fcntl");
		return (false);
	}
	return (true);
}

void	print_rect(u8 *received_data)
{
	SDL_Rect rect = *(SDL_Rect *)received_data;

	printf("x: %d\ny: %d\nwidth: %d\nheight %d\n", rect.x, rect.y, rect.w, rect.h);
}

void	handle_received_data(u8 *data, t_server *server)
{
	i32	data_identifier = *(i32 *)data;
	t_message message;

	printf("Data identifier: %d\n", data_identifier);
	switch (data_identifier)
	{
		case MSG_RECT:
			message.type = MSG_RECT;
			message.data.platform = *(t_platform *)(data + sizeof(i32));
			server_data_set(server->buffer, &message);
			break;
		case MSG_PLAYER:
			message.type = MSG_PLAYER;
			message.data.player = *(t_player *)(data + sizeof(i32));
			server_data_set(server->buffer, &message);
			break;
		default:
			message.type = MSG_UNKNOWN;
			message.data.unknown = data + sizeof(i32);
			server_data_set(server->buffer, &message);
			break;
	}
}

t_client client_local_create(t_server *server, int server_socket, struct sockaddr_in server_address)
{
	t_client client = {0};

	client.server = server;
	const u32 message = htonl(42691337);
	if (set_nonblocking_mode(server_socket))
	{
		while (1)
		{
			char buffer[256];

			ssize_t sent_bytes = sendto(server_socket, &message, sizeof(message), 0, (struct sockaddr *)&server_address, sizeof(server_address));
			if (sent_bytes < 0)
			{
				fprintf(stderr, "sendto failed: %s\n", strerror(errno));
				close(server_socket);
				return (client);
			}
			SDL_LockMutex(server->mutex);
			if (server->connection_status == -1)
			{
				SDL_UnlockMutex(server->mutex);
				break ;
			}
			SDL_UnlockMutex(server->mutex);
			socklen_t addr_len = sizeof(server_address);
			ssize_t received_bytes = recvfrom(server_socket, buffer, 4, 0, (struct sockaddr *)&server_address, &addr_len);
			if (received_bytes > 0)
			{
				t_message message;
				client.id = *(u32 *)buffer;
				if ((client.mutex = SDL_CreateMutex()) == NULL)
				{
					fprintf(stderr, "Failed creating mutex: %s\n", SDL_GetError());
					close(server_socket);
					exit(1);
				}
				server->connection_status = 1;
				client.is_connected = true;
				client.server = server;
				message.type = MSG_CONNECT;
				message.data.client = client;
				server_data_set(server->buffer, &message);
				printf("Connection Established!\nRemote Client ID: %d\n", client.id);
				break ;
			}
			usleep(1000);
		}
	}
	return (client);
}

int	client(void *data)
{
	int server_socket;
	struct sockaddr_in server_address;
	t_server *server = (t_server *)data;

	server_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_socket < 0)
	{
		fprintf(stderr, "Failed creating socket: %s\n", strerror(errno));
		return (-1);
	}
	printf("Socket opened successfully!\n");

	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT);

	if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
	{
		fprintf(stderr, "Invalid address/ Address not supported: %s\n", strerror(errno));
		close(server_socket);
		return (-1);
	}


	t_client client = client_local_create(server, server_socket, server_address);

	if (set_nonblocking_mode(server_socket))
	{
		while (1)
		{
			char buffer[BUFFER_SIZE];

			SDL_LockMutex(server->mutex);
			if (server->connection_status == -1)
			{
				SDL_UnlockMutex(server->mutex);
				break ;
			}
			SDL_UnlockMutex(server->mutex);

			socklen_t addr_len = sizeof(server_address);
			ssize_t received_bytes = recvfrom(server_socket, buffer, BUFFER_SIZE - 1, 0, (struct sockaddr *)&server_address, &addr_len);

			if (received_bytes < 0)
			{
				if (errno == EAGAIN)
				{
					usleep(100000);
					continue ;
				}
				SDL_LockMutex(server->mutex);
				fprintf(stderr, "recvfrom failed: %s\n", strerror(errno));
				server->connection_status = -1;
				SDL_UnlockMutex(server->mutex);
				break;
			}
			else if (received_bytes > 0)
			{
				buffer[received_bytes] = '\0';
				handle_received_data((u8 *)buffer, server);
			}
			else
			{
				usleep(100000);
			}

				SDL_LockMutex(server->mutex);
				if (server->was_read == true)
				{
					server->was_read = false;
					SDL_UnlockMutex(server->mutex);
				}
				SDL_UnlockMutex(server->mutex);
				usleep(100000);
		}
	}
	return (0);
}