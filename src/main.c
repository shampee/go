#include "main.h"

int main(int argc, char* argv[])
{
	memset(&app, 0, sizeof(App));

	init_sdl();
	SDL_SetWindowTitle(app.window, "Go");

	atexit(cleanup);

	int grid_size	   = 13;
	int difference	   = SCREEN_HEIGHT - SCREEN_HEIGHT / grid_size * grid_size;
	int grid_cell_size = SCREEN_HEIGHT / grid_size;
	int grid_width	   = SCREEN_HEIGHT - difference;
	int grid_height	   = SCREEN_HEIGHT - difference;

	printf("difference: %d\n", difference);

	Stone* stone_array[grid_size * grid_size];
	int	   stone_cursor = 0;

	for (int i = 0; i < grid_size * grid_size; i++)
	{
		stone_array[i]	  = stone_create();
		stone_array[i]->w = grid_cell_size / 2;
		stone_array[i]->h = grid_cell_size / 2;
	}

	// The cursor ghost is a cursor that always shows in the cell below the mouse cursor.
	SDL_Rect grid_cursor_ghost = { (grid_width - 2) / 2 * grid_cell_size,
								   (grid_height - 2) / 2 * grid_cell_size,
								   grid_cell_size / 2, grid_cell_size / 2 };

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
					if (stone_cursor <= grid_size * grid_size)
					{
						stone_cursor++;
						stone_array[stone_cursor]->x =
							(event.motion.x / grid_cell_size) * grid_cell_size -
							grid_cell_size / 4;
						stone_array[stone_cursor]->y =
							(event.motion.y / grid_cell_size) * grid_cell_size -
							grid_cell_size / 4;

						printf("%d (x: %d, y: %d), (w: %d, h: %d)\n",
							   stone_cursor, stone_array[stone_cursor]->x,
							   stone_array[stone_cursor]->y,
							   stone_array[stone_cursor]->w,
							   stone_array[stone_cursor]->h);
					}
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

		for (int x = 0; x < 1 + grid_cell_size * grid_size + 1;
			 x += grid_cell_size)
		{
			SDL_RenderDrawLine(app.renderer, x, 0, x,
							   (grid_cell_size * grid_size));
		}

		for (int y = 0; y < 1 + grid_cell_size * grid_size + 1;
			 y += grid_cell_size)
		{
			SDL_RenderDrawLine(app.renderer, 0, y, (grid_cell_size * grid_size),
							   y);
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
		for (int i = 1; i <= stone_cursor; i++)
			SDL_RenderFillRect(app.renderer, stone_array[i]);

		SDL_RenderPresent(app.renderer);
	}

	return 0;
}
