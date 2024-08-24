#include "./metroid.h"

int32_t g_signal_flag = 0;

int main(int argc, char **argv)
{
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	u32 window_width = WINDOW_WIDTH_INITIAL;
	u32 window_height = WINDOW_HEIGTH_INITIAL;

	(void)argc;
	(void)argv;
	SDL_Texture *sprite = NULL;
	SDL_Texture *background = NULL;
	char *sprite_path = "./assets/sprites/sprite.png";
	char *background_path = "./assets/oak_woods/background/background_layer_1.png";


	initialize_sdl(&renderer, &window, WINDOW_WIDTH_INITIAL, WINDOW_HEIGTH_INITIAL);

	if ((sprite = IMG_LoadTexture(renderer, sprite_path)) == NULL) {
		SDL_Log("Error Loading Texture from file: %s: %s", sprite_path,
			SDL_GetError());
		exit(1);
	}

	if ((background = IMG_LoadTexture(renderer, background_path)) == NULL)
	{
		SDL_Log("Error Loading Texture from file: %s: %s", background_path,
			SDL_GetError());
		exit(1);
	}

	t_server_data	server =
	{
		.was_read = false,
		.connection_status = 0,
	};

	server.mutex = SDL_CreateMutex();
	if (server.mutex == NULL)
	{
		SDL_Log("Error SDL_CreateMutex: %s", SDL_GetError());
		exit(1);
	}

	SDL_Thread	*server_thread = SDL_CreateThread(client, "Thread 1", &server);

	t_player	player = { .is_grounded = false, .is_jumping = false, .x = 200, .y = 300, .width = 36, .height = 56 };
	t_platform	ground = { .x = 0, .y = window_height - 150, .width = window_width, .height = 150 };
	t_platform	ledges[101];

	SDL_Rect	rectangle_texture_sprite_destination = { .x = 0, .y = 0, .w = 64, .h = 64 };
	SDL_Rect	rectangle_background_dest1 = { 0, 0, window_width, window_height };
	SDL_Rect	rectangle_background_dest2 = { window_width, 0, window_width, window_height };
	SDL_Rect	rectangle_background_source = { 0, 0, 320, 180 };

	rectangle_texture_sprite_destination.x = 20;
	rectangle_texture_sprite_destination.y = window_height - ground.height - rectangle_texture_sprite_destination.h;

	int program_is_running = 1;
	i32 start_time, frame_time, delay_time;

	srand((int)time(NULL));

	u32	ledge_cursor = 0;
	ledges[ledge_cursor++] = ground;
	while (program_is_running && g_signal_flag != 1)
	{
		start_time = SDL_GetTicks64();
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				program_is_running = 0;
		}

		const u8	*keyboard_state = SDL_GetKeyboardState(NULL);

		if (keyboard_state[SDL_SCANCODE_A])
			player.x -=  4;
		if (keyboard_state[SDL_SCANCODE_D])
			player.x +=  4;
		if (keyboard_state[SDL_SCANCODE_W])
		{
			if (player.is_jumping == false && player.is_grounded == true)
			{
				player.is_jumping = true;
				player.is_grounded = false;
			}
		}
		if (keyboard_state[SDL_SCANCODE_ESCAPE])
			break;

		SDL_Rect	player_rect =
		{
			.x = player.x,
			.y = player.y,
			.w = 48,
			.h = 56,
		};

		SDL_LockMutex(server.mutex);
		if (server.was_set)
		{
			ledges[ledge_cursor++] = *(t_platform *)server.transfered_data;
			bzero(server.transfered_data, 255);
			server.was_read = true;
			server.was_set = false;
		}
		SDL_UnlockMutex(server.mutex);

		SDL_Rect rect;

		correct_player(&player, ledges, ledge_cursor);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, background, &rectangle_background_source, &rectangle_background_dest1);
		SDL_RenderCopy(renderer, background, &rectangle_background_source, &rectangle_background_dest2);
		SDL_RenderCopy(renderer, sprite, NULL, &player_rect);
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);

		frame_time = SDL_GetTicks64() - start_time;
		delay_time = (frame_time - start_time) / 1000;

		for (u32 i = 0; i < ledge_cursor; i++)
		{
			rect = (SDL_Rect){ledges[i].x, ledges[i].y, ledges[i].width, ledges[i].height};
			SDL_RenderFillRect(renderer, &rect);
		}
		if (delay_time > 0)
			SDL_Delay(delay_time);
		SDL_RenderPresent(renderer);
	}

	int status = 0;

	SDL_LockMutex(server.mutex);
	server.connection_status = -1;
	SDL_UnlockMutex(server.mutex);
	SDL_WaitThread(server_thread, &status);
	SDL_DestroyTexture(sprite);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
	exit(0);
}
