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

typedef enum {
	MSG_RECT,
	MSG_PLAYER,
	MSG_STATE_CHANGE,
	MSG_UNKNOWN,
	MSG_CONNECT,
	MSG_DISCONNECT,
} MessageType;

typedef struct t_client t_client ;
typedef struct t_player t_player ;
typedef struct t_window t_window ;
typedef struct t_platform t_platform ;
typedef struct t_message t_message ;
typedef struct t_server t_server ;
typedef struct t_ring_buffer t_ring_buffer ;

struct t_player
{
	u32		id;
	bool	is_jumping;
	bool	is_grounded;
	i32		x;
	i32		y;
	i32		width;
	i32		height;
};

struct t_platform
{
	u32			id;
	SDL_Rect 	rect;
};

struct t_window
{
	SDL_Window 	*window;
	u32 		width;
	u32 		height;
};

#define RING_BUFFER_SIZE 16

struct 	t_server
{
	t_ring_buffer	*buffer;
	SDL_mutex		*mutex;
	SDL_cond		*cond;
	bool			was_read;
	i32				connection_status;
};

struct t_client
{
	bool		is_valid;
	u32			id;
	bool		is_connected;
	SDL_mutex	*mutex;
	SDL_cond	*cond;
	t_server	*server;
};

struct t_message
{
	MessageType type;
	union {

		t_player	player;
		t_platform 	platform;
		t_client	client;

		struct {
			int state; // Example state change
		} state_change;

		struct {
			int player_id; // Player ID for disconnect
		} disconnect;
		u8	*unknown;
	} data;
};

struct	t_ring_buffer
{
	t_message	buffer[RING_BUFFER_SIZE];
	u8			head;
	u8			tail;
	SDL_mutex	*mutex;
	SDL_cond	*not_empty;
	SDL_cond	*not_full;
};

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

/* data transmission */
t_message	server_data_get(t_ring_buffer *buffer);
void		server_data_set(t_ring_buffer *buffer, t_message *message);