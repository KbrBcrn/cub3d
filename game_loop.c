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
	if (dda->side == 0 || dda->side == 1)
		dda->wallx = game->pos_y + dda->perpwalldist * dda->raydir_y;
	else
		dda->wallx = game->pos_x + dda->perpwalldist * dda->raydir_x;
	dda->wallx -= floor(dda->wallx);
	dda->tex_x = (int)(dda->wallx * (double)TEX_SIZE);
	if (dda->side == 1)
		dda->tex_x = TEX_SIZE - dda->tex_x - 1;
	if (dda->side == 2)
		dda->tex_x = TEX_SIZE - dda->tex_x - 1;
	dda->step = TEX_SIZE * 1.0 / dda->lineheight;
	dda->texpos = (dda->drawstart
			- (HEIGHT / 2 - dda->lineheight / 2)) * dda->step;
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
//sets the texture input (tex_x) based on the hit side and the coordinates of the wall hit
static void	tex_input(t_dda *dda, t_game *game)
{
	if (dda->side == 0 || dda->side == 1)
		dda->perpwalldist = (dda->map_x - game->pos_x
				+ ((1 - dda->step_x) / 2)) / dda->raydir_x;
	else
		dda->perpwalldist = (dda->map_y - game->pos_y
				+ ((1 - dda->step_y) / 2)) / dda->raydir_y;
	dda->lineheight = (int)((HEIGHT) / dda->perpwalldist);
	dda->drawstart = HEIGHT / 2 - dda->lineheight / 2;
	if (dda->drawstart < 0)
		dda->drawstart = 0;
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
		if (game->map[dda->map_y][dda->map_x] == '1')
			dda->hit = 1;
	}
}

// calculates the side distances (sidedist_x and sidedist_y) for the ray using the DDA algorithm
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
