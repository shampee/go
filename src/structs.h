typedef struct
{
	SDL_Renderer* renderer;
	SDL_Window*	  window;
} App;

App app;
typedef struct
{
	SDL_Rect dims;
	int		 cell_value;
	int		 scan_count;
} Cell;
