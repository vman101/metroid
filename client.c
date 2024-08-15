#include "./metroid.h"
#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_net.h>
#include <stdio.h>

typedef struct
{
	t_server_data	server;
	int				in_use;
	int				questing;
	uint32_t		timer_wood;
} Client;

void	handle_sent(void *data_ptr, t_server_data *server, TCPsocket server_socket)
{
	u8	*data = data_ptr;

	switch (data[0])
	{
		case 0:
			server->transfered_data = &data[1];
			server->was_read = false;
			server->was_set = true;
			break ;
		case 1:
			if (SDLNet_TCP_Send(server_socket, (i8 []){1, 0}, 1) != 1)
				server->connection_status = -1;
			break ;
		default:
			printf("Unknown packet\n");
	}
}

int	client(void *data)
{
	TCPsocket	server_socket;
	IPaddress	ip_server;
	t_server_data *server = data;

	if (SDLNet_Init() == -1)
	{
		fprintf(stderr, "Failed Initializing SDLNet: %s\n", SDLNet_GetError());
		return (0);
	}

	while (server->connection_status != -1)
	{
		if (SDLNet_ResolveHost(&ip_server, "127.0.0.1", 6969) == -1)
		{
			fprintf(stderr, "SDLNet: Failed resolving Host: %s\n", SDLNet_GetError());
		}
		else
		{
			server->connection_status = 1;
			if ((server_socket = SDLNet_TCP_Open(&ip_server)) != NULL)
			{
				printf("Connection established!!\n");
				break;  // Successfully connected, exit the loop
			}
			else
				fprintf(stderr, "SDLNet: Failed opening TCP_Socket: %s\n", SDLNet_GetError());
		}
		printf("Retrying connection in 2 seconds...\n");
		SDL_Delay(2000);
	}

	if (SDLNet_ResolveHost(&ip_server, "127.0.0.1", 6969) == -1)
	{
		fprintf(stderr, "SDLNet: Failed resolving Host: %s\n", SDLNet_GetError());
		SDLNet_Quit();
		return (0);
	}
	else
		server->connection_status = 1;

	if ((server_socket = SDLNet_TCP_Open(&ip_server)) == NULL)
	{
		fprintf(stderr, "SDLNet: Failed opening TCP_Socket: %s\n", SDLNet_GetError());
		SDLNet_Quit();
		return (0);
	}

	while (server->connection_status != -1)
	{
		u8	buffer[255] = {0};
		i32	bytes_read = 0;
		bytes_read = SDLNet_TCP_Recv(server_socket, buffer, 255);
		if (bytes_read > 0)
		{
			SDL_LockMutex(server->mutex);
			handle_sent(buffer, server, server_socket);
			SDL_UnlockMutex(server->mutex);
			while (server->was_read == false)
				;
		}
		else if (bytes_read == 0)
		{
			printf("connection severed by host\n");
			break ;
		}
		else if (bytes_read < 0)
		{
			fprintf(stderr, "SDLNet_TCP_Recv error: %s", SDLNet_GetError());
			break ;
		}
		SDL_LockMutex(server->mutex);
		if (server->connection_status == -1 )
		{
			SDL_UnlockMutex(server->mutex);
			break ;
		}
		SDL_UnlockMutex(server->mutex);
		SDL_Delay(100);
	}

	SDLNet_Quit();
	return (1);
}
