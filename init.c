#include "./metroid.h"

void	initialize_sdl(SDL_Renderer **renderer, SDL_Window **window, i32 window_width, i32 window_height)
{
	setup_signal_handlers();
	if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
	{
		SDL_Log("Error initilazing SDL: %s", SDL_GetError());
		exit(1);
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Error initilazing SDL_Image: %s", SDL_GetError());
		SDL_Quit();
		exit(1);
	}

	if ((*window = SDL_CreateWindow(
			"Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			window_width, window_height,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL)) == NULL)
	{
		SDL_Log("Error creating SDL_Window: %s", SDL_GetError());
		exit(1);
	}

	if ((*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)) == NULL)
	{
		SDL_Log("Error creating renderer: %s", SDL_GetError());
		exit(1);
	}
}
