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

		cell_array[row][col]->dims.w = grid_cell_size;
		cell_array[row][col]->dims.h = grid_cell_size;
		cell_array[row][col]->dims.x = x;
		cell_array[row][col]->dims.y = y;

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
					cell_array[row][col]->cell_value = BLACK;
				else if (turn == WHITE)
					cell_array[row][col]->cell_value = WHITE;
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

void reset_board(int play_size)
{
	int row		  = 0;
	int col		  = 0;
	int grid_size = play_size + 1;

	while (row <= grid_size)
	{
		if (cell_array[row][col]->cell_value == BLACK || cell_array[row][col]->cell_value == WHITE)
			cell_array[row][col]->cell_value = EMPTY;
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