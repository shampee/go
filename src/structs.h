typedef struct
{
	SDL_Renderer* renderer;
	SDL_Window*	  window;
} App;

typedef struct
{
	SDL_Rect dims;
	int		 cell_value;
} Cell;
