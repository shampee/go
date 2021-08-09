#include "common.h"

enum { EMPTY,
	   BLACK,
	   WHITE,
	   OOB };

#define MAXGRIDSIZE 21
Cell* cell_array[MAXGRIDSIZE][MAXGRIDSIZE];
Cell* grid_cursor_ghost;

// variable that tracks whose turn it is
int turn = BLACK;

void init_sdl(void)
{
	int rendererFlags, windowFlags;

	rendererFlags = SDL_RENDERER_ACCELERATED;

	windowFlags = 0;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	app.window =
		SDL_CreateWindow("Go", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

	if (!app.window)
	{
		printf("Failed to open %d x %d window: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_GetError());
		exit(1);
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

	if (!app.renderer)
	{
		printf("Failed to create renderer: %s\n", SDL_GetError());
		exit(1);
	}

	// Initialize support for loading png images - can add different img types as flags
	IMG_Init(IMG_INIT_PNG);

	//Initialize support for fonts
	TTF_Init();
}

void cleanup(void)
{
	SDL_DestroyRenderer(app.renderer);

	SDL_DestroyWindow(app.window);

	IMG_Quit();

	TTF_Quit();

	SDL_Quit();
}

Cell* cell_create(void)
{
	Cell* cell = (Cell*)calloc(1, sizeof(Cell));
	return cell;
}

SDL_Texture* get_image(const char* file_name, SDL_Renderer* renderer)
{
	SDL_Surface* image = IMG_Load(file_name);
	SDL_Texture* image_texture =
		SDL_CreateTextureFromSurface(app.renderer, image);
	SDL_FreeSurface(image);
	return image_texture;
}

SDL_Texture* get_text(const char* text, const char* font_path, int size, SDL_Color color, SDL_Renderer* renderer)
{
	TTF_Font*	 font_and_size = TTF_OpenFont(font_path, size);
	SDL_Surface* surface_message =
		TTF_RenderText_Solid(font_and_size, text, color);
	SDL_Texture* message_texture = SDL_CreateTextureFromSurface(app.renderer, surface_message);
	SDL_FreeSurface(surface_message);
	return message_texture;
}

void init_board(int play_size)
{
	int row			   = 0;
	int col			   = 0;
	int x			   = 0;
	int y			   = 0;
	int grid_size	   = play_size + 1;
	int grid_cell_size = SCREEN_HEIGHT / (grid_size + 1);

	while (row <= grid_size)
	{
		cell_array[row][col] = cell_create();

		if (row == 0 || row == grid_size || col == 0 || col == grid_size)
			cell_array[row][col]->cell_value = OOB;
		else
			cell_array[row][col]->cell_value = EMPTY;

		cell_array[row][col]->dims.w	 = grid_cell_size;
		cell_array[row][col]->dims.h	 = grid_cell_size;
		cell_array[row][col]->dims.x	 = x;
		cell_array[row][col]->dims.y	 = y;
		cell_array[row][col]->scan_count = 0;

		x += grid_cell_size;
		col++;

		if (col > grid_size)
		{
			col = 0;
			x	= 0;
			row++;
			y += grid_cell_size;
		}
	}

	grid_cursor_ghost			  = cell_create();
	grid_cursor_ghost->cell_value = OOB;
}

int	  liberties = 0;
Cell* cells_scanned[MAXGRIDSIZE * MAXGRIDSIZE];
int	  count = 0;

void init_scan(int enemy_color, int row, int col);
void scan_group_for_liberties(int enemy_color, int row, int col);

void left_click_on_board(int play_size, int cursor_x, int cursor_y)
{
	int row			   = 0;
	int col			   = 0;
	int grid_size	   = play_size + 1;
	int grid_cell_size = SCREEN_HEIGHT / (grid_size + 1);

	while (row <= grid_size)
	{
		if (cursor_x > cell_array[row][col]->dims.x && cursor_y > cell_array[row][col]->dims.y && cursor_x < (cell_array[row][col]->dims.x) + grid_cell_size && cursor_y < (cell_array[row][col]->dims.y) + grid_cell_size)
		{
			if (cell_array[row][col]->cell_value == EMPTY)
			{
				if (turn == BLACK)
				{
					cell_array[row][col]->cell_value = BLACK;
					init_scan(WHITE, row - 1, col); // scans the enemy group (if there is one) for liberties directly above the placed black stone
					init_scan(WHITE, row, col + 1); // scans the enemy group directly to the right of the placed black stone
					init_scan(WHITE, row + 1, col); // scans the enemy group directly below the placed black stone
					init_scan(WHITE, row, col - 1); // scans the enemy group directly to the left of the placed black stone
				}

				else if (turn == WHITE)
				{
					cell_array[row][col]->cell_value = WHITE;
					init_scan(BLACK, row - 1, col);
					init_scan(BLACK, row, col + 1);
					init_scan(BLACK, row + 1, col);
					init_scan(BLACK, row, col - 1);
				}
			}
			break;
		}

		col++;
		if (col > grid_size)
		{
			col = 0;
			row++;
		}
	}
}

/*	init_scan: the first stone needs to be scanned before scan_group_for_liberties scans all of the stones of the same color that are connected to the first stone,
	the address of all cells of the stones in the group are stored in the cells_scanned array, which will later be used to remove (if no liberties are found) the
	stones off of the board, by changing the cell value of each cell to EMPTY
*/

void init_scan(int enemy_color, int row, int col)
{
	if (cell_array[row][col]->cell_value == enemy_color)
	{
		cell_array[row][col]->scan_count++;
		cells_scanned[++count] = cell_array[row][col];
		scan_group_for_liberties(enemy_color, row, col);

		if (liberties == 0)
		{
			while (count > 0)
			{
				cells_scanned[count]->cell_value = EMPTY;
				cells_scanned[count]->scan_count = 0;
				--count;
			}
		}
		else if (liberties != 0)
		{
			while (count > 0)
				cells_scanned[count--]->scan_count = 0;
			liberties = 0;
		}
	}
}

/*	scan_group_for_liberties: this rescursively scans all stones of the same color that are connected to the stone that was first scanned in init_scan, it will only store a cell
	in the cells_scanned array if it is the first time scanning the cell, this is kept track of by incrementing the scan_count in the cell every time it is scanned
*/

void scan_group_for_liberties(int enemy_color, int row, int col)
{
	if (cell_array[row - 1][col]->cell_value == enemy_color)
	{
		cell_array[row - 1][col]->scan_count++;
		if (cell_array[row - 1][col]->scan_count == 1)
		{
			cells_scanned[++count] = cell_array[row - 1][col];
			scan_group_for_liberties(enemy_color, row - 1, col);
		}
	}
	else if (cell_array[row - 1][col]->cell_value == EMPTY)
		++liberties;

	if (cell_array[row][col + 1]->cell_value == enemy_color)
	{
		cell_array[row][col + 1]->scan_count++;
		if (cell_array[row][col + 1]->scan_count == 1)
		{
			cells_scanned[++count] = cell_array[row][col + 1];
			scan_group_for_liberties(enemy_color, row, col + 1);
		}
	}
	else if (cell_array[row][col + 1]->cell_value == EMPTY)
		++liberties;

	if (cell_array[row + 1][col]->cell_value == enemy_color)
	{
		cell_array[row + 1][col]->scan_count++;
		if (cell_array[row + 1][col]->scan_count == 1)
		{
			cells_scanned[++count] = cell_array[row + 1][col];
			scan_group_for_liberties(enemy_color, row + 1, col);
		}
	}
	else if (cell_array[row + 1][col]->cell_value == EMPTY)
		++liberties;

	if (cell_array[row][col - 1]->cell_value == enemy_color)
	{
		cell_array[row][col - 1]->scan_count++;
		if (cell_array[row][col - 1]->scan_count == 1)
		{
			cells_scanned[++count] = cell_array[row][col - 1];
			scan_group_for_liberties(enemy_color, row, col - 1);
		}
	}
	else if (cell_array[row][col - 1]->cell_value == EMPTY)
		++liberties;
}

void reset_board(int play_size)
{
	int row		  = 0;
	int col		  = 0;
	int grid_size = play_size + 1;

	while (row <= grid_size)
	{
		if (cell_array[row][col]->cell_value == BLACK || cell_array[row][col]->cell_value == WHITE)
			cell_array[row][col]->cell_value = EMPTY;
		cell_array[row][col]->scan_count = 0;
		col++;
		if (col > grid_size)
		{
			col = 0;
			row++;
		}
	}
}

void mouse_over_board(int play_size, int cursor_x, int cursor_y)
{
	int row			   = 0;
	int col			   = 0;
	int grid_size	   = play_size + 1;
	int grid_cell_size = SCREEN_HEIGHT / (grid_size + 1);

	while (row <= grid_size)
	{
		if (cursor_x > cell_array[row][col]->dims.x && cursor_y > cell_array[row][col]->dims.y && cursor_x < (cell_array[row][col]->dims.x) + grid_cell_size && cursor_y < (cell_array[row][col]->dims.y) + grid_cell_size)
		{
			grid_cursor_ghost = cell_array[row][col];
			break;
		}

		col++;
		if (col > grid_size)
		{
			col = 0;
			row++;
		}
	}
}