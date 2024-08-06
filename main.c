#include <SDL2/SDL_error.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
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

#define WINDOW_WIDTH_INITIAL 1600
#define WINDOW_HEIGTH_INITIAL 900
#define TARGET_FPS 120
#define FRAME_TIME (1000 / TARGET_FPS)

typedef uint8_t	 uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef int8_t	int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

void	handle_key_movement(SDL_Rect *rectangle_texture_destination, SDL_Keycode key_code_pressed, int32 window_width, int32 window_height)
{
	int	step = 10;

	switch(key_code_pressed)
	{
		case (SDLK_a):
			rectangle_texture_destination->x = (rectangle_texture_destination->x > step) ? rectangle_texture_destination->x - step : 0;
			break;
		case (SDLK_d):
			rectangle_texture_destination->x = (rectangle_texture_destination->x < window_width - rectangle_texture_destination->w - step) ? rectangle_texture_destination->x + step : window_width - rectangle_texture_destination->w;
			break;
		case (SDLK_w):
			rectangle_texture_destination->y = (rectangle_texture_destination->y > step) ? rectangle_texture_destination->y - step : 0;
			break;
		case (SDLK_s):
			rectangle_texture_destination->y = (rectangle_texture_destination->y < window_height - rectangle_texture_destination->h - step) ? rectangle_texture_destination->y + step : window_height - rectangle_texture_destination->h;
			break;
	}
}

typedef struct	s_window
{
	SDL_Window	*window;
	uint32		width;
	uint32		height;
}				t_window;

int main(int argc, char **argv)
{
	SDL_Window		*window = NULL;
	SDL_Renderer	*renderer = NULL;
	uint32_t		window_width = WINDOW_WIDTH_INITIAL;
	uint32_t		window_height = WINDOW_HEIGTH_INITIAL;

	setup_signal_handlers();
	if (SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
	{
		SDL_Log("Error initilazing SDL: %s", SDL_GetError());
		exit(1);
	}

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		SDL_Log("Error initilazing SDL_Image: %s", SDL_GetError());
		exit(1);
	}

	if ((window = SDL_CreateWindow(	"Window", \
									SDL_WINDOWPOS_UNDEFINED, \
									SDL_WINDOWPOS_UNDEFINED, window_width, window_height, \
									SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL)) == NULL)
	{
		SDL_Log("Error creating SDL_Window: %s", SDL_GetError());
		exit(1);
	}

	if ((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)) == NULL)
	{
		SDL_Log("Error creating renderer: %s", SDL_GetError());
		exit(1);
	}

	SDL_Rect	rectangle_texture_sprite_source = 
	{
		.w = 32,
		.h = 32,
		.x = 0,
		.y = 0,
	};

	SDL_Rect	rectangle_texture_ground =
	{
		.w = window_width,
		.h = 150,
		.x = 0,
		.y = window_height - 150,
	};

	SDL_Rect	rectangle_texture_sprite_destination =
	{
		.x = 0,
		.y = 0,
		.w = 64,
		.h = 64,
	};
	
	SDL_Texture *sprite = NULL;
	SDL_Texture *background = NULL;
	char		*sprite_path = "./assets/sprites/sprite.png";
	char		*background_path = "./assets/oak_woods/background/background_layer_1.png";

	if ((sprite = IMG_LoadTexture(renderer, sprite_path)) == NULL)
	{
		SDL_Log("Error Loading Texture from file: %s: %s", sprite_path, SDL_GetError());
		exit (1);
	}

	if ((background = IMG_LoadTexture(renderer, background_path)) == NULL)
	{
		SDL_Log("Error Loading Texture from file: %s: %s", background_path, SDL_GetError());
		exit (1);
	}

	int program_is_running = 1;
	int32_t start_time, frame_time, delay_time;
	rectangle_texture_sprite_destination.x = 20;
	rectangle_texture_sprite_destination.y = window_height - rectangle_texture_ground.h - rectangle_texture_sprite_destination.h;
	while (program_is_running && g_signal_flag != 1)
	{
		start_time = SDL_GetTicks64();
		SDL_Event	event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				program_is_running = 0;
			else if (event.type == SDL_KEYDOWN)
			{
				if (event.key.keysym.sym == SDLK_ESCAPE)
					program_is_running = 0;
				else
					handle_key_movement(&rectangle_texture_sprite_destination, event.key.keysym.sym, window_width, window_height);
			}
		}
		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, background, NULL, NULL);
		SDL_RenderCopy(renderer, sprite, &rectangle_texture_sprite_source, &rectangle_texture_sprite_destination);
		SDL_RenderPresent(renderer);
		frame_time = SDL_GetTicks64() - start_time;
		delay_time = (frame_time - start_time) / 1000;
		if (delay_time > 0)
			SDL_Delay(delay_time);
	}
	SDL_DestroyTexture(sprite);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
	exit(0);
}
