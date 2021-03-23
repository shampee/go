#include "main.h"

int main(int argc, char* argv[])
{
	memset(&app, 0, sizeof(App));

	init_sdl();
	SDL_SetWindowTitle(app.window, "Go");

	atexit(cleanup);

	int grid_size	   = 9;
	int grid_cell_size = SCREEN_HEIGHT / grid_size;
	int grid_width	   = SCREEN_HEIGHT;
	int grid_height	   = SCREEN_HEIGHT;
	int border		   = grid_cell_size;

	// + 1 so that the last grid lines fit in the screen.
	int window_width  = (grid_width * grid_cell_size) + 1;
	int window_height = (grid_height * grid_cell_size) + 1;

	// Place the grid cursor in the middle of the screen.
	SDL_Rect grid_cursor = {
		.x = (grid_width - 1) / 2 * grid_cell_size,
		.y = (grid_height - 1) / 2 * grid_cell_size,
		.w = grid_cell_size,
		.h = grid_cell_size,
	};

	// The cursor ghost is a cursor that always shows in the cell below the
	// mouse cursor.
	SDL_Rect grid_cursor_ghost = { grid_cursor.x, grid_cursor.y, grid_cell_size,
								   grid_cell_size };

	// Dark theme.
	SDL_Color grid_background		  = { 22, 22, 22, 255 }; // Barely Black
	SDL_Color grid_line_color		  = { 44, 44, 44, 255 }; // Dark grey
	SDL_Color grid_cursor_ghost_color = { 44, 44, 44, 255 };
	SDL_Color grid_cursor_color		  = { 255, 255, 255, 255 }; // White

	SDL_bool quit		  = SDL_FALSE;
	SDL_bool mouse_active = SDL_FALSE;
	SDL_bool mouse_hover  = SDL_FALSE;

	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_w:
				case SDLK_UP:
					grid_cursor.y -= grid_cell_size;
					break;
				case SDLK_s:
				case SDLK_DOWN:
					grid_cursor.y += grid_cell_size;
					break;
				case SDLK_a:
				case SDLK_LEFT:
					grid_cursor.x -= grid_cell_size;
					break;
				case SDLK_d:
				case SDLK_RIGHT:
					grid_cursor.x += grid_cell_size;
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				grid_cursor.x =
					(event.motion.x / grid_cell_size) * grid_cell_size;
				grid_cursor.y =
					(event.motion.y / grid_cell_size) * grid_cell_size;
				break;
			case SDL_MOUSEMOTION:
				grid_cursor_ghost.x =
					(event.motion.x / grid_cell_size) * grid_cell_size;
				grid_cursor_ghost.y =
					(event.motion.y / grid_cell_size) * grid_cell_size;

				if (!mouse_active)
					mouse_active = SDL_TRUE;
				break;
			case SDL_WINDOWEVENT:
				if (event.window.event == SDL_WINDOWEVENT_ENTER && !mouse_hover)
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

		for (int x = 0; x < 1 + grid_cell_size * grid_size;
			 x += grid_cell_size) {
			SDL_RenderDrawLine(app.renderer, x, border, x,
							   (grid_cell_size * grid_size) - border);
		}

		for (int y = 0; y < 1 + grid_cell_size * (grid_size + 1);
			 y += grid_cell_size) {
			SDL_RenderDrawLine(app.renderer, border, y,
							   (grid_cell_size * grid_size) - border, y);
		}

		// Draw grid ghost cursor.
		if (mouse_active && mouse_hover) {
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
		SDL_RenderFillRect(app.renderer, &grid_cursor);

		SDL_RenderPresent(app.renderer);
	}

	return 0;
}
