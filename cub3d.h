#ifndef CUB3D_H
# define CUB3D_H

# include <stdio.h>
# include <unistd.h>
# include <fcntl.h>
# include <stdarg.h>
# include <stdlib.h>
# include <limits.h>
# include <string.h>
# include "mlx/mlx.h"
# define BUFFER_SIZE 1
# define HEIGHT 1080
# define WIDTH 1920
# define TEX_SIZE 128

typedef struct s_struct
{
    char    **map;
	char	*NO;
	char	*SO;
	char	*WE;
	char	*EA;
	int     F;
	int     C;
}	t_struct;

typedef struct	s_data {
	void	*img;
	char	*addr;
	int		bits_per_pixel;
	int		line_length;
	int		endian;
	int		x;
	int		y;
	void	*mlx;
	void	*mlx_win;
}				t_data;

typedef struct s_game
{
	void	*mlx;
	void	*win;
	t_data	img;
	int		*texture[4];
	double	pos_x;
	double	pos_y;
	double	dir_x;
	double	dir_y;
	double	plane_x;
	double	plane_y;
}	t_game;

typedef struct s_dda
{
	int		map_x;
	int		map_y;
	int		step_x;
	int		step_y;
	int		hit;
	int		side;
	int		lineheight;
	int		drawstart;
	int		drawend;
	int		texnum;
	int		tex_x;
	int		tex_y;
	int		index_y;
	double	wallx;
	double	step;
	double	texpos;
	double	camera_x;
	double	raydir_x;
	double	raydir_y;
	double	deltadist_x;
	double	deltadist_y;
	double	sidedist_x;
	double	sidedist_y;
	double	perpwalldist;
}	t_dda;

#endif