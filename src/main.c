#include "main.h"

enum { EMPTY,
	   BLACK,
	   WHITE,
	   OOB };

int main(int argc, char* argv[])
{
	memset(&app, 0, sizeof(App));

	init_sdl();
	SDL_SetWindowTitle(app.window, "Go");

	atexit(cleanup);

	int play_size	   = 9;
	int grid_size	   = play_size + 1;
	int grid_cell_size = SCREEN_HEIGHT / (grid_size + 1);

	Cell* cell_array[grid_size + 1][grid_size + 1];

	int row = 0;
	int col = 0;
	int x	= 0;
	int y	= 0;

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

	// The cursor ghost is a cursor that always shows in the cell below the mouse cursor.
	Cell* grid_cursor_ghost;
	grid_cursor_ghost			  = cell_create();
	grid_cursor_ghost->cell_value = OOB;

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
	reset_button_text = get_text(" Reset Board ", "times-new-roman.ttf", 40, black, app.renderer);

	// dimensions for button and its background for resetting board
	SDL_Rect reset_board_b;
	reset_board_b.w = grid_cell_size * 4;
	reset_board_b.h = grid_cell_size;
	reset_board_b.x = (SCREEN_HEIGHT + ((SCREEN_WIDTH - SCREEN_HEIGHT) / 6));
	reset_board_b.y = (SCREEN_HEIGHT / (grid_size + 1)) * 7;

	// variable that tracks whose turn it is
	int turn = BLACK;

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
						for (row = 0, col = 0; row <= grid_size;)
						{
							if (event.motion.x > cell_array[row][col]->dims.x && event.motion.y > cell_array[row][col]->dims.y && event.motion.x < (cell_array[row][col]->dims.x) + grid_cell_size && event.motion.y < (cell_array[row][col]->dims.y) + grid_cell_size)
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
					else if (event.motion.x > blackb.x && event.motion.y > blackb.y && event.motion.x < (blackb.x + blackb.w) && event.motion.y < (blackb.y + blackb.h))
						turn = BLACK;
					else if (event.motion.x > whiteb.x && event.motion.y > whiteb.y && event.motion.x < (whiteb.x + whiteb.w) && event.motion.y < (whiteb.y + whiteb.h))
						turn = WHITE;
					else if (event.motion.x > reset_board_b.x && event.motion.y > reset_board_b.y && event.motion.x < (reset_board_b.x + reset_board_b.w) && event.motion.y < (reset_board_b.y + reset_board_b.h))
					{
						for (row = 0, col = 0; row <= grid_size;)
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
					break;
				case SDL_MOUSEMOTION:
					if (event.motion.x > 0 && event.motion.y > 0 && event.motion.x < SCREEN_HEIGHT && event.motion.y < SCREEN_HEIGHT)
						for (row = 0, col = 0; row <= grid_size;)
						{
							if (event.motion.x > cell_array[row][col]->dims.x && event.motion.y > cell_array[row][col]->dims.y && event.motion.x < (cell_array[row][col]->dims.x) + grid_cell_size && event.motion.y < (cell_array[row][col]->dims.y) + grid_cell_size)
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

		SDL_RenderPresent(app.renderer);
	}

	return 0;
}
