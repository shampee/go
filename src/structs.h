typedef struct
{
	SDL_Renderer* renderer;
	SDL_Window*	  window;
} App;

typedef struct
{
	int x;
	int y;
} Coordinates;

typedef struct
{
	SDL_Rect dims;
	int		 cell_value;
} Cell;
