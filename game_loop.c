/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   game_loop.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kbeceren <kbeceren@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/30 10:59:21 by kbeceren          #+#    #+#             */
/*   Updated: 2023/05/30 11:24:54 by kbeceren         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d"
// calculates the color of the pixel based on the texture and lighting.
static void	cal_color(t_dda *dda, t_game *game, int x)
{
	//calculates the exact wallx coordinate based on the hit side. 
	//If the side is 0 (east) or 1 (west), it calculates game->pos_y + dda->perpwalldist * dda->raydir_y. 
	//Otherwise, for sides 2 (north) and 3 (south), it calculates game->pos_x + dda->perpwalldist * dda->raydir_x.
	if (dda->side == 0 || dda->side == 1)
		dda->wallx = game->pos_y + dda->perpwalldist * dda->raydir_y;
	else
		dda->wallx = game->pos_x + dda->perpwalldist * dda->raydir_x;
	dda->wallx -= floor(dda->wallx);
	//determines the x-coordinate in the texture that corresponds to the current wall pixel.
	dda->tex_x = (int)(dda->wallx * (double)TEX_SIZE);
	//If the side is 1 (west) or 2 (north), the code adjusts dda->tex_x to properly handle the texture mirroring or flipping.
	if (dda->side == 1)
		dda->tex_x = TEX_SIZE - dda->tex_x - 1;
	if (dda->side == 2)
		dda->tex_x = TEX_SIZE - dda->tex_x - 1;
	//calculates the step size (step) for incrementing the texture coordinate based on the line height. 
	dda->step = TEX_SIZE * 1.0 / dda->lineheight;
	// calculates the initial texture position (texpos) based on the draw start position and the line height.
	dda->texpos = (dda->drawstart
			- (HEIGHT / 2 - dda->lineheight / 2)) * dda->step;
	// sets dda->index_y to the draw start position
	dda->index_y = dda->drawstart;
	while (dda->index_y < dda->drawend)
	{
		dda->tex_y = (int)dda->texpos & (TEX_SIZE - 1);
		dda->texpos += dda->step;
		game->buf[dda->index_y][x] = \
				game->texture[dda->texnum][TEX_SIZE * dda->tex_x + dda->tex_y];
		++dda->index_y;
	}
}
//sets the texture input (tex_x) based on the hit side and the coordinates of the wall hit.
//calculates the perpendicular wall distance (perpwalldist) based on the hit side.
// If the side is 0 (east) or 1 (west), it calculates the distance.
// Otherwise, for sides 2 (north) and 3 (south), it calculates the distance using 
// another formula

static void	tex_input(t_dda *dda, t_game *game)
{
	if (dda->side == 0 || dda->side == 1)
		dda->perpwalldist = (dda->map_x - game->pos_x
				+ ((1 - dda->step_x) / 2)) / dda->raydir_x;
	else
		dda->perpwalldist = (dda->map_y - game->pos_y
				+ ((1 - dda->step_y) / 2)) / dda->raydir_y;
	
	// calculates the line height (lineheight) based on the perpendicular wall distance
	dda->lineheight = (int)((HEIGHT) / dda->perpwalldist);
	
	//sets the drawstart variable as the vertical starting point for drawing the wall on the screen. 
	//It calculates HEIGHT / 2 - dda->lineheight / 2 and assigns it to dda->drawstart. 
	//If dda->drawstart is less than 0, it is set to 0 to prevent drawing outside the screen boundaries.
	dda->drawstart = HEIGHT / 2 - dda->lineheight / 2;
	if (dda->drawstart < 0)
		dda->drawstart = 0;
	
	//calculates the drawend variable as the vertical ending point for drawing the wall. 
	//It calculates HEIGHT / 2 + dda->lineheight / 2 and assigns it to dda->drawend. If dda->drawend exceeds
	dda->drawend = HEIGHT / 2 + dda->lineheight / 2;
	if (dda->drawend >= HEIGHT)
		dda->drawend = HEIGHT - 1;
	if (game->map[dda->map_y][dda->map_x] == '1' && dda->side == 0)
		dda->texnum = WE;
	else if (game->map[dda->map_y][dda->map_x] == '1' && dda->side == 1)
		dda->texnum = EA;
	else if (game->map[dda->map_y][dda->map_x] == '1' && dda->side == 2)
		dda->texnum = NO;
	else if (game->map[dda->map_y][dda->map_x] == '1' && dda->side == 3)
		dda->texnum = SO;
}

//checks for a wall hit by iterating along the ray until a wall is encountered. It updates the dda structure to keep track of the hit state and the coordinates of the wall hit. It also updates the game structure with information about the wall hit.
static void	hit_check(t_dda *dda, t_game *game)
{
	while (dda->hit == 0)
	{
		//it checks if sidedist_x is smaller than sidedist_y. If so, 
		//it means the next intersection with a vertical wall (map_x) is 
		//closer than the next intersection with a horizontal wall (map_y). 
		//In this case, it updates sidedist_x, increments map_x by step_x, 
		//and sets the side variable accordingly (0 for the east side, 1 for the west side).
		//If sidedist_y is smaller than or equal to sidedist_x, it means the next intersection 
		//with a horizontal wall (map_y) is closer or at the same distance as the next intersection 
		//with a vertical wall (map_x). In this case, it updates sidedist_y, increments map_y by step_y, 
		//and sets the side variable accordingly (2 for the south side, 3 for the north side).
		if (dda->sidedist_x < dda->sidedist_y)
		{
			dda->sidedist_x += dda->deltadist_x;
			dda->map_x += dda->step_x;
			if (dda->step_x == 1)
				dda->side = 0;
			else if (dda->step_x == -1)
				dda->side = 1;
		}
		else
		{
			dda->sidedist_y += dda->deltadist_y;
			dda->map_y += dda->step_y;
			if (dda->step_y == 1)
				dda->side = 2;
			else if (dda->step_y == -1)
				dda->side = 3;
		}
		//checks if the map cell at the current map_y and map_x coordinates contains a wall ('1'). 
		//If it does, it sets dda->hit to 1, indicating that a wall has been hit, and the while loop terminates.
		if (game->map[dda->map_y][dda->map_x] == '1')
			dda->hit = 1;
	}
}

// calculates the side distances (sidedist_x and sidedist_y) for the ray using the DDA algorithm.
// checks the sign of raydir_x to determine whether the ray is facing left or right.
//If raydir_x is negative, it means the ray is facing left. In that case, it sets step_x to -1 
//and calculates sidedist_x by multiplying the difference between the player's position (game->pos_x) 
//and the integer value of map_x (rounded down) by deltadist_x.
//If raydir_x is positive or zero, it means the ray is facing right. In that case, it sets step_x to 1 
//and calculates sidedist_x by multiplying the difference between the next integer value of map_x (rounded up) 
//and the player's position by deltadist_x.


static void	cal_sidedist(t_dda *dda, t_game *game)
{
	if (dda->raydir_x < 0)
	{
		dda->step_x = -1;
		dda->sidedist_x = (game->pos_x - dda->map_x) * dda->deltadist_x;
	}
	else
	{
		dda->step_x = 1;
		dda->sidedist_x = (dda->map_x + 1.0 - game->pos_x) * dda->deltadist_x;
	}
	if (dda->raydir_y < 0)
	{
		dda->step_y = -1;
		dda->sidedist_y = (game->pos_y - dda->map_y) * dda->deltadist_y;
	}
	else
	{
		dda->step_y = 1;
		dda->sidedist_y = (dda->map_y + 1.0 - game->pos_y) * dda->deltadist_y;
	}
}

void	raycasting(t_game *game)
{
	int		x;
	t_dda	dda;

	x = -1;
	while (++x < WIDTH)
	{
		//Cast rays for each column of pixels
		dda.camera_x = 2 * x / (double)WIDTH - 1;
		dda.raydir_x = game->dir_x + game->plane_x * dda.camera_x;
		dda.raydir_y = game->dir_y + game->plane_y * dda.camera_x;
		dda.map_x = (int)game->pos_x;
		dda.map_y = (int)game->pos_y;
		dda.deltadist_x = fabs(1 / dda.raydir_x);
		dda.deltadist_y = fabs(1 / dda.raydir_y);
		dda.hit = 0;
		cal_sidedist(&dda, game);
		hit_check(&dda, game);
		tex_input(&dda, game);
		cal_color(&dda, game, x);
	}
}

int	game_loop(t_game *game)
{
	raycasting(game);
	// display(game);
	// update_game(game);
	return (0);
}
