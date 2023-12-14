#include "primlib.h"
#include <stdlib.h>
#include <unistd.h>
#include "pieces.h"
#include <time.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define BLOCK_SIDE gfx_screenHeight() / BOARD_HEIGHT;
#define SCREEN_HEIGHT BOARD_HEIGHT + 2

#define NUMBER_OF_KINDS 7
#define NUMBER_OF_ROTATIONS 4
#define PIECE_LENGTH 4

int is_game_over = 0;
int is_moving_down = 0;
int is_valid_move = 1;
int is_valid_horizontal_move = 1;
int is_first_block = 1;
int is_valid_rotate;

int gameboard[SCREEN_HEIGHT][BOARD_WIDTH];
float coordinates[SCREEN_HEIGHT][BOARD_WIDTH][2];

int new_block_board[PIECE_LENGTH][PIECE_LENGTH];
float new_block_coordinates[PIECE_LENGTH][PIECE_LENGTH][2];

int shape, rotation;
int new_shape, new_rotation;

int delay;
int waiting_time;

int current_row;
int current_column;

int difference_row, difference_column;

int block_width, block_height;

int previous_rotating_block_row, previous_rotating_block_column;
int current_rotating_block_row, current_rotating_block_column;

void initialize_game()
{
	float x_coordinate = gfx_screenWidth() / 2 - 0.5 * BOARD_WIDTH * BLOCK_SIDE;
	float y_coordinate = 0;

	for (int row = 0; row < SCREEN_HEIGHT; row++)
	{
		for (int column = 0; column < BOARD_WIDTH; column++)
		{
			gameboard[row][column] = 0;
			coordinates[row][column][0] = x_coordinate + column * BLOCK_SIDE;
			coordinates[row][column][1] = y_coordinate + row * BLOCK_SIDE;
		}
	}

	x_coordinate = coordinates[2][BOARD_WIDTH - 1][0] + 3 * BLOCK_SIDE;
	y_coordinate = coordinates[2][0][1];

	for (int row = 0; row < PIECE_LENGTH; row++)
	{
		for (int column = 0; column < PIECE_LENGTH; column++)
		{
			new_block_board[row][column] = 0;

			new_block_coordinates[row][column][0] = x_coordinate + column * BLOCK_SIDE;
			new_block_coordinates[row][column][1] = y_coordinate + row * BLOCK_SIDE;
		}
	}
}

void draw_board()
{
	for (int row = 0; row < SCREEN_HEIGHT; row++)
	{
		for (int column = 0; column < BOARD_WIDTH; column++)
		{
			float x1 = coordinates[row][column][0];
			float y1 = coordinates[row][column][1];

			float x2 = x1 + BLOCK_SIDE;
			float y2 = y1 + BLOCK_SIDE;

			if (gameboard[row][column] == 1)
			{
				gfx_filledRect(x1, y1, x2, y2, GREEN);
			}
			else if (gameboard[row][column] == 2)
			{
				gfx_filledRect(x1, y1, x2, y2, YELLOW);
			}
			else if (gameboard[row][column] == 3)
			{
				gfx_filledRect(x1, y1, x2, y2, RED);
			}
		}
	}

	for (int row = 0; row < PIECE_LENGTH; row++)
	{
		for (int column = 0; column < PIECE_LENGTH; column++)
		{
			float x1 = new_block_coordinates[row][column][0];
			float y1 = new_block_coordinates[row][column][1];

			float x2 = x1 + BLOCK_SIDE;
			float y2 = y1 + BLOCK_SIDE;

			if (new_block_board[row][column] == 1)
			{
				gfx_filledRect(x1, y1, x2, y2, GREEN);
			}
			else if (new_block_board[row][column] == 2)
			{
				gfx_filledRect(x1, y1, x2, y2, YELLOW);
			}
		}
	}

	float upper_left_corner_x = coordinates[2][0][0];
	float upper_left_corner_y = coordinates[2][0][1];

	float lower_right_corner_x = coordinates[BOARD_HEIGHT - 1][BOARD_WIDTH - 1][0] + BLOCK_SIDE;
	float lower_right_corner_y = gfx_screenHeight();

	gfx_rect(upper_left_corner_x, upper_left_corner_y, lower_right_corner_x, lower_right_corner_y, WHITE);

	for (int row = SCREEN_HEIGHT - BOARD_HEIGHT; row < SCREEN_HEIGHT; row++)
	{
		upper_left_corner_x = coordinates[row][0][0];
		upper_left_corner_y = coordinates[row][0][1];
		lower_right_corner_x = coordinates[row][BOARD_WIDTH - 1][0] + BLOCK_SIDE;
		lower_right_corner_y = coordinates[row][BOARD_WIDTH - 1][1];

		gfx_line(upper_left_corner_x, upper_left_corner_y, lower_right_corner_x, lower_right_corner_y, WHITE);
	}

	for (int column = 0; column < BOARD_WIDTH; column++)
	{
		upper_left_corner_x = coordinates[2][column][0];
		upper_left_corner_y = coordinates[2][column][1];
		lower_right_corner_x = coordinates[BOARD_HEIGHT - 1][column][0];
		lower_right_corner_y = coordinates[BOARD_HEIGHT - 1][column][1] + BLOCK_SIDE;

		gfx_line(upper_left_corner_x, upper_left_corner_y, lower_right_corner_x, lower_right_corner_y, WHITE);
	}
}

void clear_the_grid()
{
	for (int row = 0; row < SCREEN_HEIGHT; row++)
	{
		for (int column = 0; column < BOARD_WIDTH; column++)
		{
			if (gameboard[row][column] != 3)
			{
				gameboard[row][column] = 0;
			}
		}
	}

	for (int row = 0; row < PIECE_LENGTH; row++)
	{
		for (int column = 0; column < PIECE_LENGTH; column++)
		{
			new_block_board[row][column] = 0;
		}
	}
}

void draw_gamestate()
{
	gfx_filledRect(0, 0, gfx_screenWidth(), gfx_screenHeight(), BLACK);
	draw_board();
}

void generate_block()
{
	srand(time(NULL));

	if (is_first_block)
	{
		shape = rand() % NUMBER_OF_KINDS;
		rotation = rand() % NUMBER_OF_ROTATIONS;
	}

	new_shape = rand() % NUMBER_OF_KINDS;
	new_rotation = rand() % NUMBER_OF_ROTATIONS;

	is_first_block = 0;
}

void calculate_block_width_and_height()
{
	block_height = 0;
	block_width = 0;

	for (int column = 0; column < PIECE_LENGTH; column++)
	{
		for (int row = 0; row < PIECE_LENGTH; row++)
		{
			if (pieces[shape][rotation][row][column] == 1 || pieces[shape][rotation][row][column] == 2)
			{
				block_width++;
				break;
			}
		}
	}

	for (int row = 0; row < PIECE_LENGTH; row++)
	{
		for (int column = 0; column < PIECE_LENGTH; column++)
		{
			if (pieces[shape][rotation][row][column] == 1 || pieces[shape][rotation][row][column] == 2)
			{
				block_height++;
				break;
			}
		}
	}
}

void check_horizontal_move(char path)
{
	if (path == 'L')
	{
		for (int row = 0; row < SCREEN_HEIGHT; row++)
		{
			for (int column = 0; column < BOARD_WIDTH; column++)
			{
				if (gameboard[row][column] == 1 || gameboard[row][column] == 2)
				{
					if (gameboard[row][column - 1] == 3)
					{
						is_valid_horizontal_move = 0;
					}
				}
			}
		}
	}

	if (path == 'R')
	{
		for (int row = 0; row < SCREEN_HEIGHT; row++)
		{
			for (int column = 0; column < BOARD_WIDTH - 1; column++)
			{
				if (gameboard[row][column] == 1 || gameboard[row][column] == 2)
				{
					if (gameboard[row][column + 1] == 3)
					{
						is_valid_horizontal_move = 0;
					}
				}
			}
		}
	}
}

void draw_falling_block()
{
	clear_the_grid();

	for (int row = 0; row < PIECE_LENGTH; row++)
	{
		for (int column = 0; column < PIECE_LENGTH; column++)
		{
			new_block_board[row][column] = pieces[new_shape][new_rotation][row][column];

			if (pieces[shape][rotation][row][column] != 0)
			{
				gameboard[row + current_row][column + current_column] = pieces[shape][rotation][row][column];
			}
		}
	}

	calculate_block_width_and_height();
}

void clear_rows()
{
	int cleared_rows = 0;
	int red_blocks = 0;

	for (int row = BOARD_HEIGHT - 1; row >= cleared_rows; row--)
	{
		for (int column = 0; column < BOARD_WIDTH; column++)
		{
			if (gameboard[row][column] == 3)
			{
				red_blocks++;
			}

			if (red_blocks == BOARD_WIDTH)
			{
				cleared_rows++;
			}

			for (int column = 0; column < BOARD_WIDTH; column++)
			{
				gameboard[row][column] = gameboard[row - cleared_rows][column];
			}
		}
		red_blocks = 0;
	}
}

void find_rotating_block(int block)
{
	for (int row = 0; row < PIECE_LENGTH; row++)
	{
		for (int column = 0; column < PIECE_LENGTH; column++)
		{
			if (pieces[shape][rotation][row][column] == 2)
			{
				if (block == 0)
				{
					previous_rotating_block_row = row;
					previous_rotating_block_column = column;
				}
				else if (block == 1)
				{
					current_rotating_block_row = row;
					current_rotating_block_column = column;
				}
			}
		}
	}
}

void check_valid_rotating()
{
	is_valid_rotate = 1;

	difference_row = current_rotating_block_row - previous_rotating_block_row;
	difference_column = current_rotating_block_column - previous_rotating_block_column;

	calculate_block_width_and_height();

	for (int row = 0; row < SCREEN_HEIGHT; row++)
	{
		for (int column = 0; column < BOARD_WIDTH; column++)
		{
			if (current_column + block_width - 1 - difference_column >= BOARD_WIDTH)
			{
				is_valid_rotate = 0;
			}

			if (current_column - difference_column < 0)
			{
				is_valid_rotate = 0;
			}

			if (current_row - difference_row < 0)
			{
				is_valid_rotate = 0;
			}

			if (current_row + block_height > BOARD_HEIGHT)
			{
				is_valid_rotate = 0;
			}
		}
	}

	if (is_valid_rotate)
	{
		current_row -= difference_row;
		current_column -= difference_column;

		for (int row = current_row; row < current_row + PIECE_LENGTH; row++)
		{
			for (int column = current_column; column < current_column + PIECE_LENGTH; column++)
			{
				if (pieces[shape][rotation][row - current_row][column - current_column] != 0)
				{
					if (gameboard[row][column] == 3)
					{
						is_valid_rotate = 0;
						current_row += difference_row;
						current_column += difference_column;
						break;
					}
				}
			}
		}
	}
}

void get_key()
{
	int keycode = gfx_pollkey();
	if (keycode == SDLK_ESCAPE)
	{
		is_game_over = 1;
	}
	if (keycode == SDLK_RIGHT)
	{
		if (current_column < BOARD_WIDTH - block_width)
		{
			check_horizontal_move('R');
			if (is_valid_horizontal_move)
			{
				current_column++;
			}
			is_valid_horizontal_move = 1;
		}
	}
	if (keycode == SDLK_LEFT)
	{
		if (current_column > 0)
		{
			check_horizontal_move('L');
			if (is_valid_horizontal_move)
			{
				current_column--;
			}
			is_valid_horizontal_move = 1;
		}
	}
	if (keycode == SDLK_DOWN)
	{
		waiting_time = 5;
	}
	if (keycode == SDLK_SPACE)
	{
		find_rotating_block(0);

		if (rotation == NUMBER_OF_ROTATIONS - 1)
		{
			rotation = 0;
			find_rotating_block(1);
			check_valid_rotating();

			if (!is_valid_rotate)
			{
				rotation = NUMBER_OF_ROTATIONS - 1;
			}
		}
		else
		{
			rotation++;
			find_rotating_block(1);
			check_valid_rotating();

			if (!is_valid_rotate)
			{
				rotation--;
			}
		}
	}
}

void wait_during_collision()
{
	delay = 1;
	waiting_time = 300;
	while (delay % waiting_time != 0)
	{
		draw_gamestate();
		draw_falling_block();
		clear_rows();
		get_key();
		gfx_updateScreen();
		delay++;
	}
}

void check_touching_the_ground()
{
	for (int column = 0; column < BOARD_WIDTH; column++)
	{
		if (gameboard[BOARD_HEIGHT - 1][column] == 1 || gameboard[BOARD_HEIGHT - 1][column] == 2)
		{
			wait_during_collision();

			for (int row = BOARD_HEIGHT - 1; row >= BOARD_HEIGHT - block_height; row--)
			{
				for (int column = 0; column < BOARD_WIDTH; column++)
				{
					if (gameboard[row][column] == 1 || gameboard[row][column] == 2)
					{
						gameboard[row][column] = 3;
					}
				}
			}
			is_moving_down = 0;
		}
	}
}

void check_colliding()
{
	for (int row = BOARD_HEIGHT - 1; row >= 0; row--)
	{
		for (int column = 0; column < BOARD_WIDTH; column++)
		{
			if (gameboard[row][column] == 1 || gameboard[row][column] == 2)
			{
				if (gameboard[row + 1][column] == 3)
				{
					is_valid_move = 0;
				}
			}
		}
	}

	if (!is_valid_move)
	{
		// wait_during_collision();

		for (int row = 0; row < SCREEN_HEIGHT; row++)
		{
			for (int column = 0; column < BOARD_WIDTH; column++)
			{
				if (gameboard[row][column] == 1 || gameboard[row][column] == 2)
				{
					gameboard[row][column] = 3;
				}
			}
		}
		is_moving_down = 0;
	}
}

void free_fall()
{
	for (int row = BOARD_HEIGHT - 2; row >= 0; row--)
	{
		for (int column = 0; column < BOARD_WIDTH; column++)
		{
			if (gameboard[row][column] == 1)
			{
				gameboard[row + 1][column] = 1;
				gameboard[row][column] = 0;
			}
			else if (gameboard[row][column] == 2)
			{
				gameboard[row + 1][column] = 2;
				gameboard[row][column] = 0;
			}
		}
	}
	current_row++;
}

void fall_down()
{
	if (is_moving_down && !is_game_over)
	{
		generate_block();
		while (is_moving_down && !is_game_over)
		{
			draw_gamestate();
			draw_falling_block();
			clear_rows();
			get_key();
			gfx_updateScreen();

			if (delay % waiting_time == 0)
			{
				if (is_moving_down)
				{
					check_touching_the_ground();
				}
				if (is_moving_down)
				{
					check_colliding();
				}
				if (is_moving_down && is_valid_move)
				{
					free_fall();
				}
				delay = 0;
			}
			delay++;
		}
	}

	else
	{
		waiting_time = 300;
		is_moving_down = 1;
		is_valid_move = 1;
		current_row = 0;
		current_column = BOARD_WIDTH / 2 - 1;
		delay = 1;
		shape = new_shape;
		rotation = new_rotation;
	}
}

void show_game_over_screen()
{
	gfx_filledRect(0, 0, gfx_screenWidth(), gfx_screenHeight(), BLACK);
	gfx_textout(gfx_screenWidth() / 2, gfx_screenHeight() / 2, "YOU LOSE", RED);
}

void check_loosing_game()
{
	for (int column = 0; column < BOARD_WIDTH; column++)
	{
		if (gameboard[0][column] == 3 || gameboard[1][column] == 3)
		{
			show_game_over_screen();
			gfx_updateScreen();
			int keycode = gfx_getkey();

			if (keycode == SDLK_ESCAPE)
			{
				is_game_over = 1;
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if (gfx_init())
	{
		exit(3);
	}

	initialize_game();

	while (!is_game_over)
	{
		draw_gamestate();
		fall_down();
		check_loosing_game();
	}

	return 0;
}