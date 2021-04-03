#include "draw.h"

Grid draw_grid(int grid_size, SDL_bool mouse_active, SDL_bool mouse_hover)
{
	int border	   = SCREEN_HEIGHT / grid_size;
	int board_size = SCREEN_HEIGHT - border;
	int cell_size  = board_size / grid_size;

	// Place the grid cursor in the middle of the screen.
	SDL_Rect grid_cursor = {
		.x = (grid_size - 1) / 2 * cell_size,
		.y = (grid_size - 1) / 2 * cell_size,
		.w = cell_size,
		.h = cell_size,
	};
	SDL_Rect grid_cursor_ghost = { grid_cursor.x, grid_cursor.y, cell_size,
								   cell_size };

	// Draw grid background
	SDL_SetRenderDrawColor(app.renderer, grid_background.r, grid_background.g,
						   grid_background.b, grid_background.a);
	SDL_RenderClear(app.renderer);

	// Draw grid lines
	SDL_SetRenderDrawColor(app.renderer, grid_line_color.r, grid_line_color.g,
						   grid_line_color.b, grid_line_color.a);

	for (int x = cell_size; x < cell_size + cell_size * grid_size;
		 x += cell_size)
	{
		SDL_RenderDrawLine(app.renderer, x, cell_size, x, board_size);
	}

	for (int y = cell_size; y < cell_size + cell_size * grid_size;
		 y += cell_size)
	{
		SDL_RenderDrawLine(app.renderer, border, y, board_size, y);
	}

	// Draw grid ghost cursor
	if (mouse_active && mouse_hover)
	{
		SDL_SetRenderDrawColor(app.renderer, grid_cursor_ghost_color.r,
							   grid_cursor_ghost_color.g,
							   grid_cursor_ghost_color.b,
							   grid_cursor_ghost_color.a);
		SDL_RenderFillRect(app.renderer, &grid_cursor_ghost);
		SDL_RenderPresent(app.renderer);
	}
	SDL_SetRenderDrawColor(app.renderer, grid_cursor_color.r,
						   grid_cursor_color.g, grid_cursor_color.b,
						   grid_cursor_color.a);
	SDL_RenderFillRect(app.renderer, &grid_cursor);
	SDL_RenderPresent(app.renderer);

	Grid grid = { .size			= grid_size,
				  .cell_size	= cell_size,
				  .cursor_ghost = grid_cursor_ghost };
	return grid;
}
