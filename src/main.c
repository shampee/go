#include "main.h"

int main(int argc, char* argv[])
{
	memset(&app, 0, sizeof(App));

	init_sdl();

	atexit(cleanup);

	SDL_bool quit		  = SDL_FALSE;
	SDL_bool mouse_active = SDL_FALSE;
	SDL_bool mouse_hover  = SDL_FALSE;

	while (!quit) {
		SDL_Event event;
		Grid	  grid = draw_grid(9, mouse_active, mouse_hover);
		SDL_RenderPresent(app.renderer);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_w:
				case SDLK_UP:
					grid.cursor.y -= grid.cell_size;
					break;
				case SDLK_s:
				case SDLK_DOWN:
					grid.cursor.y += grid.cell_size;
					break;
				case SDLK_a:
				case SDLK_LEFT:
					grid.cursor.x -= grid.cell_size;
					break;
				case SDLK_d:
				case SDLK_RIGHT:
					grid.cursor.x += grid.cell_size;
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				grid.cursor.x =
					(event.motion.x / grid.cell_size) * grid.cell_size;
				grid.cursor.y =
					(event.motion.y / grid.cell_size) * grid.cell_size;
				break;
			case SDL_MOUSEMOTION:
				grid.cursor_ghost.x =
					(event.motion.x / grid.cell_size) * grid.cell_size;
				grid.cursor_ghost.y =
					(event.motion.y / grid.cell_size) * grid.cell_size;

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
	}

	return 0;
}
