#include "common.h"

extern void cleanup(void);
extern void init_sdl(void);
extern void do_input(void);
extern Grid draw_grid(int grid_size, SDL_bool mouse_active,
					  SDL_bool mouse_hover);

App app;
