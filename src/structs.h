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
	Cell* cell_array[MAXGRIDSIZE][MAXGRIDSIZE];
	Cell* grid_cursor_ghost;
	int	  play_size;
} Board;

typedef struct
{
	SDL_Texture* black_sc_texture;
	SDL_Texture* white_sc_texture;
	char		 black_sc_str[10];
	char		 white_sc_str[10];
	int			 black_score;
	int			 white_score;
	SDL_Color	 black;
	SDL_Color	 white;
} Score;

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
	Score score;
} GameState;
