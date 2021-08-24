#include "common.h"

enum { EMPTY,
	   BLACK,
	   WHITE,
	   OOB,
	   OK,
	   YES,
	   NO };

Cell* cell_array[MAXGRIDSIZE][MAXGRIDSIZE];
Cell* grid_cursor_ghost;

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
		TTF_RenderText_Blended(font_and_size, text, color);
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

void init_scan_enemy(GameState* gs, int enemy_color, int row, int col);
void capture_stones(GameState* gs);
int	 check_for_suicide(GameState* gs, int own_color, int row, int col);
void scan_group_for_liberties(GameState* gs, int enemy_color, int row, int col);
void get_score_text_black(void);
void get_score_text_white(void);

void left_click_on_board(GameState* gs, int play_size, int cursor_x, int cursor_y)
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
				if (gs->turn == BLACK)
				{
					cell_array[row][col]->cell_value = BLACK;
					gs->stones_to_capture			 = NO;

					init_scan_enemy(gs, WHITE, row - 1, col); // scans the enemy group (if there is one) for liberties directly above the placed black stone
					init_scan_enemy(gs, WHITE, row, col + 1); // scans the enemy group directly to the right of the placed black stone
					init_scan_enemy(gs, WHITE, row + 1, col); // scans the enemy group directly below the placed black stone
					init_scan_enemy(gs, WHITE, row, col - 1); // scans the enemy group directly to the left of the placed black stone

					// Check for Ko
					if (gs->stones_to_capture == YES && gs->ko_rule_black == cell_array[row][col])
					{
						printf("Rule: ko, also known as infinity - you cannot place the stone in the same cell as your previous move\n\n");
						cell_array[row][col]->cell_value = EMPTY;
						while (gs->capcount > 0)
							gs->stones_captured[gs->capcount--]->scan_count = 0;
						return;
					}

					capture_stones(gs);
					get_score_text_black();

					if (check_for_suicide(gs, BLACK, row, col) == OK)
						gs->ko_rule_black = cell_array[row][col];
				}

				else if (gs->turn == WHITE)
				{
					cell_array[row][col]->cell_value = WHITE;
					gs->stones_to_capture			 = NO;

					init_scan_enemy(gs, BLACK, row - 1, col);
					init_scan_enemy(gs, BLACK, row, col + 1);
					init_scan_enemy(gs, BLACK, row + 1, col);
					init_scan_enemy(gs, BLACK, row, col - 1);

					// Check for Ko
					if (gs->stones_to_capture == YES && gs->ko_rule_white == cell_array[row][col])
					{
						printf("Rule: ko, also known as infinity - you cannot place the stone in the same cell as your previous move\n\n");
						cell_array[row][col]->cell_value = EMPTY;
						while (gs->capcount > 0)
							gs->stones_captured[gs->capcount--]->scan_count = 0;
						return;
					}

					capture_stones(gs);
					get_score_text_white();

					if (check_for_suicide(gs, WHITE, row, col) == OK)
						gs->ko_rule_white = cell_array[row][col];
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

/*	init_scan_enemy: the first stone needs to be scanned before scan_group_for_liberties scans all of the stones of the same color that are connected to the first stone,
	the address of all cells of the stones in the group are stored in the cells_scanned array, which will later be used to remove (if no liberties are found) the
	stones off of the board, by changing the cell value of each cell to EMPTY
*/

void init_scan_enemy(GameState* gs, int enemy_color, int row, int col)
{
	if (cell_array[row][col]->cell_value == enemy_color && cell_array[row][col]->scan_count == 0)
	{
		cell_array[row][col]->scan_count++;
		gs->cells_scanned[++gs->count] = cell_array[row][col];

		scan_group_for_liberties(gs, enemy_color, row, col);

		if (gs->liberties == 0)
		{
			while (gs->count > 0)
				gs->stones_captured[++gs->capcount] = gs->cells_scanned[gs->count--];
			gs->stones_to_capture = YES;
		}
		else if (gs->liberties != 0)
		{
			while (gs->count > 0)
				gs->cells_scanned[gs->count--]->scan_count = 0;
			gs->liberties = 0;
		}
	}
}

/*	scan_group_for_liberties: this rescursively scans all stones of the same color that are connected to the stone that was first scanned in init_scan_enemy, it will only store a cell
	in the cells_scanned array if it is the first time scanning the cell, this is kept track of by incrementing the scan_count in the cell every time it is scanned
*/

void scan_group_for_liberties(GameState* gs, int target_color, int row, int col)
{
	if (cell_array[row - 1][col]->cell_value == target_color)
	{
		cell_array[row - 1][col]->scan_count++;
		if (cell_array[row - 1][col]->scan_count == 1)
		{
			gs->cells_scanned[++gs->count] = cell_array[row - 1][col];
			scan_group_for_liberties(gs, target_color, row - 1, col);
		}
	}
	else if (cell_array[row - 1][col]->cell_value == EMPTY)
		++gs->liberties;

	if (cell_array[row][col + 1]->cell_value == target_color)
	{
		cell_array[row][col + 1]->scan_count++;
		if (cell_array[row][col + 1]->scan_count == 1)
		{
			gs->cells_scanned[++gs->count] = cell_array[row][col + 1];
			scan_group_for_liberties(gs, target_color, row, col + 1);
		}
	}
	else if (cell_array[row][col + 1]->cell_value == EMPTY)
		++gs->liberties;

	if (cell_array[row + 1][col]->cell_value == target_color)
	{
		cell_array[row + 1][col]->scan_count++;
		if (cell_array[row + 1][col]->scan_count == 1)
		{
			gs->cells_scanned[++gs->count] = cell_array[row + 1][col];
			scan_group_for_liberties(gs, target_color, row + 1, col);
		}
	}
	else if (cell_array[row + 1][col]->cell_value == EMPTY)
		++gs->liberties;

	if (cell_array[row][col - 1]->cell_value == target_color)
	{
		cell_array[row][col - 1]->scan_count++;
		if (cell_array[row][col - 1]->scan_count == 1)
		{
			gs->cells_scanned[++gs->count] = cell_array[row][col - 1];
			scan_group_for_liberties(gs, target_color, row, col - 1);
		}
	}
	else if (cell_array[row][col - 1]->cell_value == EMPTY)
		++gs->liberties;
}

// score for capturing stones
int black_score = 0;
int white_score = 0;

void capture_stones(GameState* gs)
{
	if (gs->turn == BLACK)
		black_score += gs->capcount;
	else if (gs->turn == WHITE)
		white_score += gs->capcount;
	while (gs->capcount > 0)
	{
		gs->stones_captured[gs->capcount]->cell_value = EMPTY;
		gs->stones_captured[gs->capcount]->scan_count = 0;
		--gs->capcount;
	}
}

// Variables for keeping score
SDL_Texture* black_sc_texture;
SDL_Texture* white_sc_texture;
char		 black_sc_str[10];
char		 white_sc_str[10];
SDL_Color	 black = { 0, 0, 0, 255 };
SDL_Color	 white = { 255, 255, 255, 0 };

void get_score_text_black(void)
{
	sprintf(black_sc_str, " %d ", black_score);
	black_sc_texture = get_text(black_sc_str, "times-new-roman.ttf", 100, black, app.renderer);
}

void get_score_text_white(void)
{
	sprintf(white_sc_str, " %d ", white_score);
	white_sc_texture = get_text(white_sc_str, "times-new-roman.ttf", 100, black, app.renderer);
}

int check_for_suicide(GameState* gs, int own_color, int row, int col)
{
	scan_group_for_liberties(gs, own_color, row, col);

	if (gs->liberties == 0)
	{
		cell_array[row][col]->cell_value = EMPTY;
		printf("Rule: You can not reduce your own group of stones liberties to 0, or otherwise place a stone where there are no liberties for it\n\n");
		while (gs->count > 0)
			gs->cells_scanned[gs->count--]->scan_count = 0;
		return !OK;
	}
	else
	{
		while (gs->count > 0)
			gs->cells_scanned[gs->count--]->scan_count = 0;
		gs->liberties = 0;
		return OK;
	}
}

void reset_board(GameState* gs, int play_size)
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
	gs->ko_rule_black = NULL;
	gs->ko_rule_white = NULL;
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
