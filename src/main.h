#include "common.h"

extern void			cleanup(void);
extern void			init_sdl(void);
extern Cell*		cell_create(void);
extern void			cell_destroy(Cell* stone);
extern SDL_Texture* get_image(const char* file_name, SDL_Renderer* renderer);
extern SDL_Texture* get_text(const char* text, const char* font, int size, SDL_Color color, SDL_Renderer* renderer);

App app;
