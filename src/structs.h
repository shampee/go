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

typedef struct
{
	int	  liberties;
	Cell* cells_scanned[MAXGRIDSIZE * MAXGRIDSIZE];
	Cell* stones_captured[MAXGRIDSIZE * MAXGRIDSIZE];
	int	  count;
	int	  capcount;
	Cell* ko_rule_black;
	Cell* ko_rule_white;
	int	  stones_to_capture;
	int	  turn;
} GameState;
