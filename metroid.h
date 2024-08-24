#include <SDL2/SDL.h>
#include <SDL2/SDL_mutex.h>
#include <SDL2/SDL_error.h>
#include "SDL_image.h"
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH_INITIAL 1600
#define WINDOW_HEIGTH_INITIAL 900
#define TARGET_FPS 120
#define FRAME_TIME (1000 / TARGET_FPS)

#define JUMP_HEIGHT 200

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef enum	metroid_directions
{
	NONE = 0,
	UP = 1 << 0,
	DOWN = 1 << 1,
	LEFT = 1 << 2,
	RIGHT = 1 << 3
}	e_direction;

typedef struct
{
	bool	is_jumping;
	bool	is_grounded;
	i32		x;
	i32		y;
	i32		width;
	i32		height;
} t_player;

typedef	struct
{
	i32	x;
	i32	y;
	i32	width;
	i32	height;
}	t_platform;

typedef struct s_window
{
	SDL_Window *window;
	u32 width;
	u32 height;
} t_window;

typedef enum
{
	PLAYER,
	RECT,
}	t_data_type;

typedef struct
{
	void		*transfered_data;
	SDL_mutex	*mutex;
	bool		was_read;
	bool		was_set;
	i32			connection_status;
	t_data_type	data_type;
}	t_server_data;

extern int32_t g_signal_flag;

/* Setup */
void	initialize_sdl(SDL_Renderer **renderer, SDL_Window **window, i32 window_width, i32 window_height);

/* Physics */
void	gravity_apply(t_player *player);
u8	colision_detection_calulate(i32 rect1_x, i32 rect1_y, i32 rect1_width, i32 rect1_height, \
								i32 rect2_x, i32 rect2_y, i32 rect2_width, i32 rect2_height);
void	correct_player(t_player *player, t_platform platforms[], u32 rect_count);

/* Misc */
void setup_signal_handlers(void);
void handle_sigint(int sig);

/* Server */
int	client(void *data);
