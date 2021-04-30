#include "main.h"

enum { EMPTY, BLACK, WHITE, OOB };

int main(int argc, char* argv[])
{
	memset(&app, 0, sizeof(App));

	init_sdl();
	SDL_SetWindowTitle(app.window, "Go");

	atexit(cleanup);

	int play_size	   = 9;
	int grid_size	   = play_size + 1;
	int grid_cell_size = SCREEN_HEIGHT / grid_size;

	Cell* cell_array[grid_size][grid_size];

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
		cell_array[row][col]->dims.x = x - (grid_cell_size / 2);
		cell_array[row][col]->dims.y = y - (grid_cell_size / 2);

		x += grid_cell_size;
		col++;

		if (col > grid_size)
		{
			col = 0;
			row++;
			y += grid_cell_size;
			x = 0;
		}
	}

	// The cursor ghost is a cursor that always shows in the cell below the mouse cursor.
	SDL_Rect grid_cursor_ghost = { 0, 0, grid_cell_size / 2,
								   grid_cell_size / 2 };

	// Light theme.
	SDL_Color grid_background		  = { 129, 162, 190, 255 };
	SDL_Color grid_line_color		  = { 255, 255, 255, 255 };
	SDL_Color grid_cursor_ghost_color = { 240, 198, 116, 255 };
	SDL_Color grid_cursor_color		  = { 38, 50, 56, 255 };

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
					break;
				case SDL_MOUSEMOTION:
					grid_cursor_ghost.x =
						(event.motion.x / grid_cell_size) * grid_cell_size -
						grid_cell_size / 4;
					grid_cursor_ghost.y =
						(event.motion.y / grid_cell_size) * grid_cell_size -
						grid_cell_size / 4;

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
		SDL_SetRenderDrawColor(app.renderer, grid_background.r,
							   grid_background.g, grid_background.b,
							   grid_background.a);
		SDL_RenderClear(app.renderer);

		// Draw grid lines.
		SDL_SetRenderDrawColor(app.renderer, grid_line_color.r,
							   grid_line_color.g, grid_line_color.b,
							   grid_line_color.a);

		for (int x = 0; x < 1 + grid_cell_size * play_size + 1;
			 x += grid_cell_size)
		{
			SDL_RenderDrawLine(app.renderer, x, grid_cell_size, x,
							   (grid_cell_size * play_size));
		}

		for (int y = 0; y < 1 + grid_cell_size * play_size + 1;
			 y += grid_cell_size)
		{
			SDL_RenderDrawLine(app.renderer, grid_cell_size, y,
							   (grid_cell_size * play_size), y);
		}

		// Draw grid ghost cursor.
		if (mouse_active && mouse_hover)
		{
			SDL_SetRenderDrawColor(app.renderer, grid_cursor_ghost_color.r,
								   grid_cursor_ghost_color.g,
								   grid_cursor_ghost_color.b,
								   grid_cursor_ghost_color.a);
			SDL_RenderFillRect(app.renderer, &grid_cursor_ghost);
		}

		// Draw grid cursor.
		SDL_SetRenderDrawColor(app.renderer, grid_cursor_color.r,
							   grid_cursor_color.g, grid_cursor_color.b,
							   grid_cursor_color.a);

		SDL_RenderFillRect(app.renderer, &cell_array[1][3]->dims);
		SDL_RenderFillRect(app.renderer, &cell_array[2][5]->dims);
		SDL_RenderFillRect(app.renderer, &cell_array[4][6]->dims);

		SDL_RenderPresent(app.renderer);
	}

	return 0;
}
