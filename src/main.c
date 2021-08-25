#include "main.h"
int main(int argc, char* argv[])
{
	memset(&app, 0, sizeof(App));

	init_sdl();
	SDL_SetWindowTitle(app.window, "Go");

	// Should we move these initialisations into some function?
	Board board = {
		.cell_array		   = { { NULL } },
		.grid_cursor_ghost = NULL,
		.play_size		   = 9,
	};

	GameState gs = { .liberties			= 0,
					 .cells_scanned		= { NULL },
					 .stones_captured	= { NULL },
					 .count				= 0,
					 .capcount			= 0,
					 .ko_rule_black		= NULL,
					 .ko_rule_white		= NULL,
					 .stones_to_capture = NO,
					 .turn				= BLACK,
					 .score				= {
						 .black_sc_texture = NULL,
						 .white_sc_texture = NULL,
						 .black_sc_str	   = { '\0' },
						 .white_sc_str	   = { '\0' },
						 .black_score	   = 0,
						 .white_score	   = 0,
					 } };

	atexit(cleanup);

	int grid_size	   = board.play_size + 1;
	int grid_cell_size = SCREEN_HEIGHT / (grid_size + 1);

	int row = 0;
	int col = 0;

	init_board(&board, board.play_size);

	// Colors
	SDL_Color grid_background		  = { 222, 184, 135, 255 };
	SDL_Color grid_line_color		  = { 0, 0, 0, 255 };
	SDL_Color grid_cursor_ghost_color = { 240, 198, 116, 255 };
	SDL_Color black					  = { 0, 0, 0, 255 };
	SDL_Color white					  = { 255, 255, 255, 0 };

	// load images for stones
	SDL_Texture* black_stone;
	SDL_Texture* white_stone;
	black_stone = get_image("assets/b.png", app.renderer);
	white_stone = get_image("assets/w.png", app.renderer);

	// dimensions for buttons to change players turn
	SDL_Rect blackb;
	SDL_Rect whiteb;

	blackb.w = grid_cell_size;
	blackb.h = grid_cell_size;
	blackb.x = (SCREEN_HEIGHT + ((SCREEN_WIDTH - SCREEN_HEIGHT) / 3));
	blackb.y = (SCREEN_HEIGHT / (grid_size + 1)) * 3;

	whiteb.w = grid_cell_size;
	whiteb.h = grid_cell_size;
	whiteb.x = (SCREEN_HEIGHT + ((SCREEN_WIDTH - SCREEN_HEIGHT) / 3));
	whiteb.y = (SCREEN_HEIGHT / (grid_size + 1)) * 5;

	// load text for reset board button
	SDL_Texture* reset_button_text;
	reset_button_text = get_text(" Reset Board ", "fonts/times-new-roman.ttf", 100, black, app.renderer);

	// cells to show score for black and white
	SDL_Rect black_sc_rect;
	SDL_Rect white_sc_rect;

	black_sc_rect.w = grid_cell_size;
	black_sc_rect.h = grid_cell_size;
	black_sc_rect.x = (blackb.x + grid_cell_size * 2);
	black_sc_rect.y = (blackb.y);

	white_sc_rect.w = grid_cell_size;
	white_sc_rect.h = grid_cell_size;
	white_sc_rect.x = (whiteb.x + grid_cell_size * 2);
	white_sc_rect.y = (whiteb.y);

	get_score_text_black(&gs);
	get_score_text_white(&gs);

	// dimensions for reset board button
	SDL_Rect reset_board_b;
	reset_board_b.w = grid_cell_size * 4;
	reset_board_b.h = grid_cell_size;
	reset_board_b.x = (SCREEN_HEIGHT + ((SCREEN_WIDTH - SCREEN_HEIGHT) / 6));
	reset_board_b.y = (SCREEN_HEIGHT / (grid_size + 1)) * 7;

	// create textures for co ordinates on edges of board - alphabetical
	SDL_Texture* texture_array_alpha[grid_size];
	char		 alphabet_char = 'A';
	char		 alphabet_char_string[5];
	int			 i;
	for (i = 0; i < board.play_size; alphabet_char++, i++)
	{
		sprintf(alphabet_char_string, " %c ", alphabet_char);
		texture_array_alpha[i] = get_text(alphabet_char_string, "fonts/times-new-roman.ttf", 100, black, app.renderer);
	}

	// create textures for co ordinates on edges of board - numerical
	SDL_Texture* texture_array_num[grid_size];
	int			 num_char = board.play_size;
	char		 num_char_str[5];
	for (i = 0; i < board.play_size; num_char--, i++)
	{
		sprintf(num_char_str, " %d ", num_char);
		texture_array_num[i] = get_text(
			num_char_str, "fonts/times-new-roman.ttf", 100, black, app.renderer);
	}

	// load image for dot
	SDL_Texture* dot_image;
	dot_image = get_image("dot.png", app.renderer);

	SDL_bool quit		  = SDL_FALSE;
	SDL_bool mouse_active = SDL_FALSE;
	SDL_bool mouse_hover  = SDL_FALSE;

	while (!quit)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.motion.x > 0 && event.motion.y > 0 &&
						event.motion.x < SCREEN_HEIGHT &&
						event.motion.y < SCREEN_HEIGHT)
						left_click_on_board(&board,
											&gs,
											event.motion.x,
											event.motion.y);
					else if (event.motion.x > blackb.x &&
							 event.motion.y > blackb.y &&
							 event.motion.x < (blackb.x + blackb.w) &&
							 event.motion.y < (blackb.y + blackb.h))
						(&gs)->turn = BLACK;
					else if (event.motion.x > whiteb.x &&
							 event.motion.y > whiteb.y &&
							 event.motion.x < (whiteb.x + whiteb.w) &&
							 event.motion.y < (whiteb.y + whiteb.h))
						(&gs)->turn = WHITE;
					else if (event.motion.x > reset_board_b.x &&
							 event.motion.y > reset_board_b.y &&
							 event.motion.x <
								 (reset_board_b.x + reset_board_b.w) &&
							 event.motion.y <
								 (reset_board_b.y + reset_board_b.h))
						reset_board(&board, &gs);
					break;
				case SDL_MOUSEMOTION:
					if (event.motion.x > 0 && event.motion.y > 0 &&
						event.motion.x < SCREEN_HEIGHT &&
						event.motion.y < SCREEN_HEIGHT)
						mouse_over_board(
							&board, event.motion.x, event.motion.y);
					if (!mouse_active)
						mouse_active = SDL_TRUE;
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_ENTER &&
						!mouse_hover)
						mouse_hover = SDL_TRUE;
					else if (event.window.event == SDL_WINDOWEVENT_LEAVE &&
							 mouse_hover)
						mouse_hover = SDL_FALSE;
					break;
				case SDL_QUIT:
					quit = SDL_TRUE;
					break;
			}
		}

		// Draw grid background.
		SDL_SetRenderDrawColor(app.renderer,
							   grid_background.r,
							   grid_background.g,
							   grid_background.b,
							   grid_background.a);
		SDL_RenderClear(app.renderer);

		// Draw grid lines.
		SDL_SetRenderDrawColor(app.renderer,
							   grid_line_color.r,
							   grid_line_color.g,
							   grid_line_color.b,
							   grid_line_color.a);

		for (int x = grid_cell_size * 1.5;
			 x < 1 + grid_cell_size * grid_size + 1;
			 x += grid_cell_size)
		{
			SDL_RenderDrawLine(app.renderer,
							   x,
							   grid_cell_size * 1.5,
							   x,
							   (grid_cell_size * grid_size) -
								   (grid_cell_size / 2));
		}

		for (int y = grid_cell_size * 1.5;
			 y < 1 + grid_cell_size * grid_size + 1;
			 y += grid_cell_size)
		{
			SDL_RenderDrawLine(app.renderer,
							   grid_cell_size * 1.5,
							   y,
							   (grid_cell_size * grid_size) -
								   (grid_cell_size / 2),
							   y);
		}

		// Draw dots on board
		if (board.play_size == 9)
		{
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[3][3]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[3][7]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[5][5]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[7][3]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[7][7]->dims);
		}

		else if (board.play_size == 13)
		{
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[4][4]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[4][10]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[7][7]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[10][4]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[10][10]->dims);
		}

		else if (board.play_size == 19)
		{
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[4][4]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[4][10]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[4][16]->dims);

			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[10][4]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[10][10]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[10][16]->dims);

			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[16][4]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[16][10]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &board.cell_array[16][16]->dims);
		}

		// Draw co ordinates
		for (row = 0, col = 1, i = 0; i < board.play_size; col++, i++)
			SDL_RenderCopy(app.renderer, texture_array_alpha[i], NULL, &board.cell_array[row][col]->dims);

		for (row = grid_size, col = 1, i = 0; i < board.play_size; col++, i++)
			SDL_RenderCopy(app.renderer, texture_array_alpha[i], NULL, &board.cell_array[row][col]->dims);

		for (row = 1, col = 0, i = 0; i < board.play_size; row++, i++)
			SDL_RenderCopy(app.renderer, texture_array_num[i], NULL, &board.cell_array[row][col]->dims);

		for (row = 1, col = grid_size, i = 0; i < board.play_size; row++, i++)
			SDL_RenderCopy(app.renderer, texture_array_num[i], NULL, &board.cell_array[row][col]->dims);

		// Draw white and black buttons
		SDL_RenderCopy(app.renderer, black_stone, NULL, &blackb);
		SDL_RenderCopy(app.renderer, white_stone, NULL, &whiteb);

		// Draw reset button - it would seem things are rendered on top of what was rendered before, so the order matters
		SDL_SetRenderDrawColor(
			app.renderer, white.r, white.g, white.b, white.a);
		SDL_RenderFillRect(app.renderer, &reset_board_b);
		SDL_RenderCopy(app.renderer, reset_button_text, NULL, &reset_board_b);

		// Draw grid ghost cursor.
		if (mouse_active && mouse_hover)
		{
			SDL_SetRenderDrawColor(app.renderer, grid_cursor_ghost_color.r, grid_cursor_ghost_color.g, grid_cursor_ghost_color.b, grid_cursor_ghost_color.a);
			if (board.grid_cursor_ghost->cell_value == EMPTY)
				SDL_RenderFillRect(app.renderer, &board.grid_cursor_ghost->dims);
		}

		// Draw stones.
		for (row = 0, col = 0; row <= grid_size;)
		{
			if (board.cell_array[row][col]->cell_value == BLACK)
				SDL_RenderCopy(app.renderer, black_stone, NULL, &board.cell_array[row][col]->dims);
			else if (board.cell_array[row][col]->cell_value == WHITE)
				SDL_RenderCopy(app.renderer, white_stone, NULL, &board.cell_array[row][col]->dims);
			col++;
			if (col > grid_size)
			{
				col = 0;
				row++;
			}
		}

		// draw score for black and white

		SDL_RenderCopy(app.renderer, gs.score.black_sc_texture, NULL, &black_sc_rect);
		SDL_RenderCopy(app.renderer, gs.score.white_sc_texture, NULL, &white_sc_rect);

		SDL_RenderPresent(app.renderer);
	}

	return 0;
}
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

	app.window = SDL_CreateWindow("Go", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

	if (!app.window)
	{
		printf("Failed to open %d x %d window: %s\n",
			   SCREEN_WIDTH,
			   SCREEN_HEIGHT,
			   SDL_GetError());
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
	TTF_Font*	 font_and_size	 = TTF_OpenFont(font_path, size);
	SDL_Surface* surface_message = TTF_RenderText_Blended(font_and_size, text, color);
	SDL_Texture* message_texture = SDL_CreateTextureFromSurface(app.renderer, surface_message);
	SDL_FreeSurface(surface_message);
	return message_texture;
}
void init_board(Board* board, int play_size)
{
	int row			   = 0;
	int col			   = 0;
	int x			   = 0;
	int y			   = 0;
	int grid_size	   = board->play_size + 1;
	int grid_cell_size = SCREEN_HEIGHT / (grid_size + 1);

	while (row <= grid_size)
	{
		board->cell_array[row][col] = cell_create();

		if (row == 0 || row == grid_size || col == 0 || col == grid_size)
			board->cell_array[row][col]->cell_value = OOB;
		else
			board->cell_array[row][col]->cell_value = EMPTY;

		board->cell_array[row][col]->dims.w		= grid_cell_size;
		board->cell_array[row][col]->dims.h		= grid_cell_size;
		board->cell_array[row][col]->dims.x		= x;
		board->cell_array[row][col]->dims.y		= y;
		board->cell_array[row][col]->scan_count = 0;

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

	board->grid_cursor_ghost			 = cell_create();
	board->grid_cursor_ghost->cell_value = OOB;
	board->play_size					 = play_size;
}
void left_click_on_board(Board* board, GameState* gs, int cursor_x, int cursor_y)
{
	int row			   = 0;
	int col			   = 0;
	int grid_size	   = board->play_size + 1;
	int grid_cell_size = SCREEN_HEIGHT / (grid_size + 1);

	while (row <= grid_size)
	{
		if (cursor_x > board->cell_array[row][col]->dims.x && cursor_y > board->cell_array[row][col]->dims.y && cursor_x < (board->cell_array[row][col]->dims.x) + grid_cell_size && cursor_y < (board->cell_array[row][col]->dims.y) + grid_cell_size)
		{
			if (board->cell_array[row][col]->cell_value == EMPTY)
			{
				if (gs->turn == BLACK)
				{
					board->cell_array[row][col]->cell_value = BLACK;
					gs->stones_to_capture					= NO;
					// scans the enemy group (if there is one) for liberties directly above the placed black stone
					init_scan_enemy(board, gs, WHITE, row - 1, col);
					// scans the enemy group directly to the right of the placed black stone
					init_scan_enemy(board, gs, WHITE, row, col + 1);
					// scans the enemy group directly below the placed black stone
					init_scan_enemy(board, gs, WHITE, row + 1, col);
					// scans the enemy group directly to the left of the placed black stone
					init_scan_enemy(board, gs, WHITE, row, col - 1);

					// Check for Ko
					if (gs->stones_to_capture == YES &&
						gs->ko_rule_black == board->cell_array[row][col])
					{
						printf("Rule: ko, also known as infinity - you cannot place the stone in the same cell as your previous move\n\n");
						board->cell_array[row][col]->cell_value = EMPTY;
						while (gs->capcount > 0)
							gs->stones_captured[gs->capcount--]->scan_count = 0;
						return;
					}

					capture_stones(gs);
					get_score_text_black(gs);

					if (check_for_suicide(board, gs, BLACK, row, col) == OK)
						gs->ko_rule_black = board->cell_array[row][col];
				}

				else if (gs->turn == WHITE)
				{
					board->cell_array[row][col]->cell_value = WHITE;
					gs->stones_to_capture					= NO;

					init_scan_enemy(board, gs, BLACK, row - 1, col);
					init_scan_enemy(board, gs, BLACK, row, col + 1);
					init_scan_enemy(board, gs, BLACK, row + 1, col);
					init_scan_enemy(board, gs, BLACK, row, col - 1);

					// Check for Ko
					if (gs->stones_to_capture == YES &&
						gs->ko_rule_white == board->cell_array[row][col])
					{
						printf("Rule: ko, also known as infinity - you cannot place the stone in the same cell as your previous move\n\n");
						board->cell_array[row][col]->cell_value = EMPTY;
						while (gs->capcount > 0)
							gs->stones_captured[gs->capcount--]->scan_count = 0;
						return;
					}

					capture_stones(gs);
					get_score_text_white(gs);

					if (check_for_suicide(board, gs, WHITE, row, col) == OK)
						gs->ko_rule_white = board->cell_array[row][col];
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
void init_scan_enemy(Board* board, GameState* gs, int enemy_color, int row, int col)
{
	if (board->cell_array[row][col]->cell_value == enemy_color &&
		board->cell_array[row][col]->scan_count == 0)
	{
		board->cell_array[row][col]->scan_count++;
		gs->cells_scanned[++gs->count] = board->cell_array[row][col];

		scan_group_for_liberties(board, gs, enemy_color, row, col);

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

void scan_group_for_liberties(Board* board, GameState* gs, int target_color, int row, int col)
{
	if (board->cell_array[row - 1][col]->cell_value == target_color)
	{
		board->cell_array[row - 1][col]->scan_count++;
		if (board->cell_array[row - 1][col]->scan_count == 1)
		{
			gs->cells_scanned[++gs->count] = board->cell_array[row - 1][col];
			scan_group_for_liberties(board, gs, target_color, row - 1, col);
		}
	}
	else if (board->cell_array[row - 1][col]->cell_value == EMPTY)
		++gs->liberties;

	if (board->cell_array[row][col + 1]->cell_value == target_color)
	{
		board->cell_array[row][col + 1]->scan_count++;
		if (board->cell_array[row][col + 1]->scan_count == 1)
		{
			gs->cells_scanned[++gs->count] = board->cell_array[row][col + 1];
			scan_group_for_liberties(board, gs, target_color, row, col + 1);
		}
	}
	else if (board->cell_array[row][col + 1]->cell_value == EMPTY)
		++gs->liberties;

	if (board->cell_array[row + 1][col]->cell_value == target_color)
	{
		board->cell_array[row + 1][col]->scan_count++;
		if (board->cell_array[row + 1][col]->scan_count == 1)
		{
			gs->cells_scanned[++gs->count] = board->cell_array[row + 1][col];
			scan_group_for_liberties(board, gs, target_color, row + 1, col);
		}
	}
	else if (board->cell_array[row + 1][col]->cell_value == EMPTY)
		++gs->liberties;

	if (board->cell_array[row][col - 1]->cell_value == target_color)
	{
		board->cell_array[row][col - 1]->scan_count++;
		if (board->cell_array[row][col - 1]->scan_count == 1)
		{
			gs->cells_scanned[++gs->count] = board->cell_array[row][col - 1];
			scan_group_for_liberties(board, gs, target_color, row, col - 1);
		}
	}
	else if (board->cell_array[row][col - 1]->cell_value == EMPTY)
		++gs->liberties;
}

void capture_stones(GameState* gs)
{
	if (gs->turn == BLACK)
		gs->score.black_score += gs->capcount;
	else if (gs->turn == WHITE)
		gs->score.white_score += gs->capcount;
	while (gs->capcount > 0)
	{
		gs->stones_captured[gs->capcount]->cell_value = EMPTY;
		gs->stones_captured[gs->capcount]->scan_count = 0;
		--gs->capcount;
	}
}

void get_score_text_black(GameState* gs)
{
	sprintf(gs->score.black_sc_str, " %d ", gs->score.black_score);
	gs->score.black_sc_texture = get_text(gs->score.black_sc_str, "fonts/times-new-roman.ttf", 100, gs->score.black, app.renderer);
}

void get_score_text_white(GameState* gs)
{
	sprintf(gs->score.white_sc_str, " %d ", gs->score.white_score);
	gs->score.white_sc_texture = get_text(gs->score.white_sc_str, "fonts/times-new-roman.ttf", 100, gs->score.black, app.renderer);
}

int check_for_suicide(Board* board, GameState* gs, int own_color, int row, int col)
{
	scan_group_for_liberties(board, gs, own_color, row, col);

	if (gs->liberties == 0)
	{
		board->cell_array[row][col]->cell_value = EMPTY;
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

void reset_board(Board* board, GameState* gs)
{
	int row		  = 0;
	int col		  = 0;
	int grid_size = board->play_size + 1;

	while (row <= grid_size)
	{
		if (board->cell_array[row][col]->cell_value == BLACK ||
			board->cell_array[row][col]->cell_value == WHITE)
			board->cell_array[row][col]->cell_value = EMPTY;
		board->cell_array[row][col]->scan_count = 0;
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

void mouse_over_board(Board* board, int cursor_x, int cursor_y)
{
	int row			   = 0;
	int col			   = 0;
	int grid_size	   = board->play_size + 1;
	int grid_cell_size = SCREEN_HEIGHT / (grid_size + 1);

	while (row <= grid_size)
	{
		if (cursor_x > board->cell_array[row][col]->dims.x &&
			cursor_y > board->cell_array[row][col]->dims.y &&
			cursor_x < (board->cell_array[row][col]->dims.x) + grid_cell_size &&
			cursor_y < (board->cell_array[row][col]->dims.y) + grid_cell_size)
		{
			board->grid_cursor_ghost = board->cell_array[row][col];
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
