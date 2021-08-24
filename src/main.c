#include "main.h"

int main(int argc, char* argv[])
{
	memset(&app, 0, sizeof(App));

	init_sdl();
	SDL_SetWindowTitle(app.window, "Go");

	GameState gs = {
		.liberties		   = 0,
		.cells_scanned	   = { NULL },
		.stones_captured   = { NULL },
		.count			   = 0,
		.capcount		   = 0,
		.ko_rule_black	   = NULL,
		.ko_rule_white	   = NULL,
		.stones_to_capture = NO,
		.turn			   = BLACK,
	};

	atexit(cleanup);

	int play_size	   = 9;
	int grid_size	   = play_size + 1;
	int grid_cell_size = SCREEN_HEIGHT / (grid_size + 1);

	int row = 0;
	int col = 0;

	init_board(play_size);

	// Colors
	SDL_Color grid_background		  = { 222, 184, 135, 255 };
	SDL_Color grid_line_color		  = { 0, 0, 0, 255 };
	SDL_Color grid_cursor_ghost_color = { 240, 198, 116, 255 };
	SDL_Color black					  = { 0, 0, 0, 255 };
	SDL_Color white					  = { 255, 255, 255, 0 };

	// load images for stones
	SDL_Texture* black_stone;
	SDL_Texture* white_stone;
	black_stone = get_image("b.png", app.renderer);
	white_stone = get_image("w.png", app.renderer);

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
	reset_button_text = get_text(" Reset Board ", "times-new-roman.ttf", 100, black, app.renderer);

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

	get_score_text_black();
	get_score_text_white();

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
	for (i = 0; i < play_size; alphabet_char++, i++)
	{
		sprintf(alphabet_char_string, " %c ", alphabet_char);
		texture_array_alpha[i] = get_text(alphabet_char_string, "times-new-roman.ttf", 100, black, app.renderer);
	}

	// create textures for co ordinates on edges of board - numerical
	SDL_Texture* texture_array_num[grid_size];
	int			 num_char = play_size;
	char		 num_char_str[5];
	for (i = 0; i < play_size; num_char--, i++)
	{
		sprintf(num_char_str, " %d ", num_char);
		texture_array_num[i] = get_text(num_char_str, "times-new-roman.ttf", 100, black, app.renderer);
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
					if (event.motion.x > 0 && event.motion.y > 0 && event.motion.x < SCREEN_HEIGHT && event.motion.y < SCREEN_HEIGHT)
						left_click_on_board(&gs, play_size, event.motion.x, event.motion.y);
					else if (event.motion.x > blackb.x && event.motion.y > blackb.y && event.motion.x < (blackb.x + blackb.w) && event.motion.y < (blackb.y + blackb.h))
						(&gs)->turn = BLACK;
					else if (event.motion.x > whiteb.x && event.motion.y > whiteb.y && event.motion.x < (whiteb.x + whiteb.w) && event.motion.y < (whiteb.y + whiteb.h))
						(&gs)->turn = WHITE;
					else if (event.motion.x > reset_board_b.x && event.motion.y > reset_board_b.y && event.motion.x < (reset_board_b.x + reset_board_b.w) && event.motion.y < (reset_board_b.y + reset_board_b.h))
						reset_board(&gs, play_size);
					break;
				case SDL_MOUSEMOTION:
					if (event.motion.x > 0 && event.motion.y > 0 && event.motion.x < SCREEN_HEIGHT && event.motion.y < SCREEN_HEIGHT)
						mouse_over_board(play_size, event.motion.x, event.motion.y);
					if (!mouse_active)
						mouse_active = SDL_TRUE;
					break;
				case SDL_WINDOWEVENT:
					if (event.window.event == SDL_WINDOWEVENT_ENTER && !mouse_hover)
						mouse_hover = SDL_TRUE;
					else if (event.window.event == SDL_WINDOWEVENT_LEAVE && mouse_hover)
						mouse_hover = SDL_FALSE;
					break;
				case SDL_QUIT:
					quit = SDL_TRUE;
					break;
			}
		}

		// Draw grid background.
		SDL_SetRenderDrawColor(app.renderer, grid_background.r, grid_background.g, grid_background.b, grid_background.a);
		SDL_RenderClear(app.renderer);

		// Draw grid lines.
		SDL_SetRenderDrawColor(app.renderer, grid_line_color.r, grid_line_color.g, grid_line_color.b, grid_line_color.a);

		for (int x = grid_cell_size * 1.5; x < 1 + grid_cell_size * grid_size + 1; x += grid_cell_size)
		{
			SDL_RenderDrawLine(app.renderer, x, grid_cell_size * 1.5, x, (grid_cell_size * grid_size) - (grid_cell_size / 2));
		}

		for (int y = grid_cell_size * 1.5; y < 1 + grid_cell_size * grid_size + 1; y += grid_cell_size)
		{
			SDL_RenderDrawLine(app.renderer, grid_cell_size * 1.5, y, (grid_cell_size * grid_size) - (grid_cell_size / 2), y);
		}

		// Draw dots on board
		if (play_size == 9)
		{
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[3][3]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[3][7]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[5][5]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[7][3]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[7][7]->dims);
		}

		else if (play_size == 13)
		{
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[4][4]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[4][10]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[7][7]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[10][4]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[10][10]->dims);
		}

		else if (play_size == 19)
		{
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[4][4]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[4][10]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[4][16]->dims);

			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[10][4]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[10][10]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[10][16]->dims);

			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[16][4]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[16][10]->dims);
			SDL_RenderCopy(app.renderer, dot_image, NULL, &cell_array[16][16]->dims);
		}

		// Draw co ordinates
		for (row = 0, col = 1, i = 0; i < play_size; col++, i++)
			SDL_RenderCopy(app.renderer, texture_array_alpha[i], NULL, &cell_array[row][col]->dims);

		for (row = grid_size, col = 1, i = 0; i < play_size; col++, i++)
			SDL_RenderCopy(app.renderer, texture_array_alpha[i], NULL, &cell_array[row][col]->dims);

		for (row = 1, col = 0, i = 0; i < play_size; row++, i++)
			SDL_RenderCopy(app.renderer, texture_array_num[i], NULL, &cell_array[row][col]->dims);

		for (row = 1, col = grid_size, i = 0; i < play_size; row++, i++)
			SDL_RenderCopy(app.renderer, texture_array_num[i], NULL, &cell_array[row][col]->dims);

		// Draw white and black buttons
		SDL_RenderCopy(app.renderer, black_stone, NULL, &blackb);
		SDL_RenderCopy(app.renderer, white_stone, NULL, &whiteb);

		// Draw reset button - it would seem things are rendered on top of what was rendered before, so the order matters
		SDL_SetRenderDrawColor(app.renderer, white.r, white.g, white.b, white.a);
		SDL_RenderFillRect(app.renderer, &reset_board_b);
		SDL_RenderCopy(app.renderer, reset_button_text, NULL, &reset_board_b);

		// Draw grid ghost cursor.
		if (mouse_active && mouse_hover)
		{
			SDL_SetRenderDrawColor(app.renderer, grid_cursor_ghost_color.r, grid_cursor_ghost_color.g, grid_cursor_ghost_color.b, grid_cursor_ghost_color.a);
			if (grid_cursor_ghost->cell_value == EMPTY)
				SDL_RenderFillRect(app.renderer, &grid_cursor_ghost->dims);
		}

		// Draw stones.
		for (row = 0, col = 0; row <= grid_size;)
		{
			if (cell_array[row][col]->cell_value == BLACK)
				SDL_RenderCopy(app.renderer, black_stone, NULL, &cell_array[row][col]->dims);
			else if (cell_array[row][col]->cell_value == WHITE)
				SDL_RenderCopy(app.renderer, white_stone, NULL, &cell_array[row][col]->dims);
			col++;
			if (col > grid_size)
			{
				col = 0;
				row++;
			}
		}

		// draw score for black and white

		SDL_RenderCopy(app.renderer, black_sc_texture, NULL, &black_sc_rect);
		SDL_RenderCopy(app.renderer, white_sc_texture, NULL, &white_sc_rect);

		SDL_RenderPresent(app.renderer);
	}

	return 0;
}
