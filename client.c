#include "./metroid.h"
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <fcntl.h>

typedef struct
{
	t_server_data	server;
	int				in_use;
	int				questing;
	uint32_t		timer_wood;
} Client;

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

typedef struct ServerData
{
	u32 data_type;
	u32 id;
	u32 filler;
	u32 filler2;
	u8	*transfered_data;
}	t_data;

void	handle_received_data(u8 *data, t_server_data *server)
{
	i32	data_identifier = *(i32 *)data;

	printf("Data identifier: %d\n", data_identifier);
	switch (data_identifier)
	{
		case RECT:
			SDL_LockMutex(server->mutex);
			server->data_type = RECT;
			server->transfered_data = data + 16;
			server->was_set = true;
			server->was_read = false;
			print_rect(server->transfered_data);
			SDL_UnlockMutex(server->mutex);
			break;
		default:
			break;
	}
}

int	client(void *data)
{
	int server_socket;
	struct sockaddr_in server_address;
	t_server_data *server = (t_server_data *)data;

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

	const char *message = "Hello server\n";
	ssize_t sent_bytes = sendto(server_socket, message, strlen(message), 0, (struct sockaddr *)&server_address, sizeof(server_address));
	if (sent_bytes < 0)
	{
		fprintf(stderr, "sendto failed: %s\n", strerror(errno));
		close(server_socket);
		return -1;
	}

	else
	{
		printf("Message sent to server: %s\n", message);
		server->connection_status = 1;
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

				while (server->was_read == false)
					usleep(1000);
			}
		}
	}
	return (0);
}