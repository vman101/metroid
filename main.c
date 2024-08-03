#include <SDL2/SDL_error.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <signal.h>
#include <SDL2/SDL.h>

int32_t	g_signal_flag = 0;

void	handle_sigint(int sig)
{
	if (sig == SIGINT)
	{
		g_signal_flag = 1;
	}
}

void	setup_signal_handlers(void)
{
	struct sigaction	sa;

	bzero(&sa, sizeof(sa));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handle_sigint;
	sigaction(SIGINT, &sa, NULL);
	sa.sa_handler = SIG_IGN;
	sigaction(SIGQUIT, &sa, NULL);
	sigaction(SIGTSTP, &sa, NULL);
}

void	handle_keypress(SDL_Keycode key_code_pressed)
{
	switch(key_code_pressed)
	{
	}
}

int main()
{
	SDL_Window		*window = NULL;
	SDL_Renderer	*renderer = NULL;

	setup_signal_handlers();
	if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
	{
		SDL_Log("Error initilazing SDL: %s", SDL_GetError());
		exit(1);
	}
	if ((window = SDL_CreateWindow(	"Window", \
									SDL_WINDOWPOS_UNDEFINED, \
									SDL_WINDOWPOS_UNDEFINED, 640, 480, \
									SDL_WINDOW_MAXIMIZED)) == NULL)
	{
		SDL_Log("Error creating SDL_Window: %s", SDL_GetError());
		exit(1);
	}
	if ((renderer = SDL_CreateRenderer(window, -1, 0)) == NULL)
	{
		SDL_Log("Error creating renderer: %s", SDL_GetError());
		exit(1);
	}

	SDL_Rect	rectangle;

	rectangle.x = 0;
    rectangle.y = 0;
    rectangle.w = 20;
    rectangle.h = 20;

	int program_is_running = 1;
	while (program_is_running)
	{
		SDL_Event	event;
		SDL_Keycode key_last_pressed = 0;
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case (SDL_KEYDOWN):
					switch (event.key.keysym.sym)
					{
						case (SDLK_ESCAPE):
							program_is_running = 0;
							break ;
						case (SDLK_a):
							if (rectangle.x > 0)
							{
								if (key_last_pressed == event.key.keysym.sym)
									rectangle.x -= rectangle.x;
								else
									rectangle.x -= 2;
							}
							break;
						case (SDLK_d):
							if (rectangle.x < 640 - rectangle.x)
							{
								if (key_last_pressed == event.key.keysym.sym)
									rectangle.x += rectangle.x;
								else
									rectangle.x += 2;
							}
							break;
						case (SDLK_w):
							if (rectangle.y > 0)
							{
								if (key_last_pressed == event.key.keysym.sym)
									rectangle.y -= rectangle.y;
								else
									rectangle.y -= 2;
							}
							break;
						case (SDLK_s):
							if (rectangle.y < 480 - rectangle.h)
							{
								if (key_last_pressed == event.key.keysym.sym)
									rectangle.y += rectangle.y;
								else
									rectangle.y += 2;
							}
							break;
					}
					key_last_pressed = event.key.keysym.sym;
					break;
			}
			SDL_SetRenderDrawColor(renderer, 0xde, 0xf2, 0x0, 0xff);
			SDL_RenderClear(renderer);
			SDL_SetRenderDrawColor(renderer, 0xff, 0x0, 0x0, 0xff);
			SDL_RenderFillRect(renderer, &rectangle);
			SDL_RenderDrawRect(renderer, &rectangle);
			SDL_RenderPresent(renderer);
		}
	}
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	exit(0);
}
