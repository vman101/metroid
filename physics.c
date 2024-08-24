#include "./metroid.h"

u8	colision_detection_calulate(i32 rect1_x, i32 rect1_y, i32 rect1_width, i32 rect1_height, i32 rect2_x, i32 rect2_y, i32 rect2_width, i32 rect2_height)
{
	u8	result = 0;

	if (rect1_y + rect1_height > rect2_y - 1 && rect1_y + rect1_height < rect2_y + rect2_height - 1)
	{
		if (rect1_x + rect1_width > rect2_x && rect1_x < rect2_x + rect2_width)
		{
			result |= UP;
		}
	}

	else if (rect1_y < rect2_y + rect2_height && rect1_y > rect2_y)
	{
		if (rect1_x + rect1_width > rect2_x && rect1_x < rect2_x + rect2_width)
		{
			result |= DOWN;
		}
	}

	if (rect1_x + rect1_width > rect2_x && rect1_x + rect1_width < rect2_x + rect2_width)
	{
		if (rect1_y + rect1_height < rect2_y && rect1_y  + rect1_height > rect2_y + rect2_height)
		{
			result |= LEFT;
		}
	}

	else if (rect1_x < rect2_x + rect2_width && rect1_x > rect2_x)
	{
		if (rect1_y + rect1_height > rect2_y && rect1_y  + rect1_height < rect2_y + rect2_height)
		{
			result |= RIGHT;
		}
	}
	return (result);
}

void	gravity_apply(t_player *player)
{
	player->y += 5;
}

void	player_jump(t_player *player)
{
	static i32	starting_height = -1;
	static i32	previous_height = WINDOW_HEIGTH_INITIAL;
	i32			jump_height;

	if (starting_height == -1)
		starting_height = player->y;
	jump_height = starting_height - JUMP_HEIGHT;
	if (previous_height < player->y)
	{
		player->is_jumping = false;
		previous_height = WINDOW_HEIGTH_INITIAL;
	}

	if (player->y > jump_height && player->is_jumping)
	{
		player->y -= 5;
		previous_height = player->y;
		if (player->y <= jump_height)
		{
			starting_height = -1;
			player->is_jumping = false;
			previous_height = WINDOW_HEIGTH_INITIAL;
			return ;
		}
	}
}

void	correct_player(t_player *player, t_platform platforms[], u32 rect_count)
{
	u8	collision_points = 0;
	static	t_platform last_standing_point;

	for (u32 i = 0; i < rect_count; i++) {
		collision_points = colision_detection_calulate(	player->x, player->y, player->width, player->height, \
														platforms[i].x, platforms[i].y, platforms[i].width, platforms[i].height);
		if (collision_points == 0 && !player->is_grounded)
		{
			continue ;
		}
		if (collision_points & UP)
		{
			player->y = platforms[i].y - player->height;
			player->is_grounded = true;
			last_standing_point = platforms[i];
		}
		if (!(colision_detection_calulate(	player->x, player->y, player->width, player->height, \
						last_standing_point.x, last_standing_point.y, last_standing_point.width, last_standing_point.height) & UP))
			player->is_grounded = false;
		if (collision_points & DOWN)
		{
			player->y += 1;
		}
		if (collision_points & LEFT)
		{
			player->x = platforms[i].x - player->width;
		}
		if (collision_points & RIGHT)
			player->x = platforms[i].x + platforms[i].width;
	}
	if (player->is_jumping == true)
		player_jump(player);
	else if (player->is_grounded == false)
		gravity_apply(player);
}