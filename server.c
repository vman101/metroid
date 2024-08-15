#include "./metroid.h"

typedef struct
{
	t_server_data	server;
	int				in_use;
	int				questing;
	uint32_t		timer_wood;
} Client;

void	server()
{
	TCPsocket	server_socket;
	IPaddress	ip_server;
	char		*host_name;

	if (SDLNet_Init() == -1)
	{
		fprintf(stderr, "SDL_Net: Failed Initializing SDLNet: %s\n", SDLNet_GetError());
		exit (1);
	}

	if (SDLNet_ResolveHost(&ip_server, NULL, 6969) == -1)
	{
		fprintf(stderr, "SDLNet: Failed resolving Host: %s\n", SDLNet_GetError());
		SDLNet_Quit();
		exit (1);
	}

	if ((server_socket = SDLNet_TCP_Open(&ip_server)) == NULL)
	{
		fprintf(stderr, "SDLNet: Failed opening TCP_Socket: %s\n", SDLNet_GetError());
		SDLNet_Quit();
		exit (1);
	}

	TCPsocket new_socket;

	srand((int)time(NULL));

	while (1)
	{
		new_socket = SDLNet_TCP_Accept(server_socket);
		if (new_socket)
		{
			printf("connected\n");
			while (1)
			{
				if (time(NULL) % 30 == 0)
				{
					u8 *plat = (u8 []){0, rand() % 1600, rand() % 900, 70, 50};
					printf("Platform send\n");
					SDLNet_TCP_Send(new_socket, &plat, 17);
					SDL_Delay(1000);
				}
			}
			SDL_Delay(100);
		}
	}

	SDLNet_TCP_Close(new_socket);
	SDLNet_TCP_Close(server_socket);
	SDLNet_Quit();
}

int main()
{
	server();
}
