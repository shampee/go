typedef struct {
	SDL_Renderer* renderer;
	SDL_Window*	  window;
} App;

typedef struct {
	int		 size;
	int		 cell_size;
	SDL_Rect cursor;
	SDL_Rect cursor_ghost;
} Grid;
