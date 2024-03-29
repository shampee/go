typedef struct
{
    SDL_Renderer* renderer;
    SDL_Window*   window;
} App;

App app;
typedef struct
{
    SDL_Rect dims;
    int      cell_value;
    int      has_cell_been_scanned;
    int      has_liberty_been_scanned;
    int      has_border_been_scanned;
    int      territory_value;
    char     position_str[3];
} Cell;

typedef struct
{
    Cell* cell_array[MAXGRIDSIZE][MAXGRIDSIZE];
    Cell* grid_cursor_ghost;
    int   play_size;
} Board;

typedef struct
{
    SDL_Texture* black_sc_texture;
    SDL_Texture* white_sc_texture;
    char         black_sc_str[10];
    char         white_sc_str[10];
    int          current_black_score;
    int          current_white_score;
    int          prev_black_score;
    int          prev_white_score;
    SDL_Color    black;
    SDL_Color    white;
} Score;

typedef struct
{
    int black_stones_next_to_empty_cells;
    int white_stones_next_to_empty_cells;
} EndScore;

typedef enum { DEBUG, REGULAR } GameMode;
typedef enum { MENU, HOST, JOIN, PLAY } State;
typedef struct
{
    int             liberties;
    Cell*           cells_scanned[MAXGRIDSIZE * MAXGRIDSIZE];
    Cell*           stones_captured[MAXGRIDSIZE * MAXGRIDSIZE];
    int             count;
    int             capcount;
    Cell*           ko_rule_black;
    Cell*           ko_rule_white;
    Cell*           to_be_placed;
    int             player_color;
    int             stones_to_capture;
    int             turn;
    Board           board;
    Score           score;
    EndScore        end_score;
    GameMode        game_mode;
    State           state;
    SDL_bool        hosting;
    SDL_bool        joining;
    pthread_mutex_t mutex;
    struct
    {
        IPaddress  ip;
        IPaddress* remote_ip;
        TCPsocket  client;
        TCPsocket  server;
        Uint32     ipaddr;
        Uint16     port;
        int        got_client;

    } net;
} GameState;

typedef struct
{
    char            monitor;
    float           aspect_ratio;
    SDL_DisplayMode display_mode;
    struct
    {
        int w;
        int h;
    } window_size;
    struct
    {
        SDL_DisplayMode* modes;
        int              len;
    } available_modes;

} Settings;
