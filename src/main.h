#include "common.h"

enum { EMPTY, BLACK, WHITE, OOB, OK, YES, NO, BLACK_T, WHITE_T, NO_T };

void init_sdl(Settings* s, GameState* gs, char argc, char* argv[]);
void cleanup(void);

Cell* cell_create(void);

SDL_Texture* get_image(const char* file_name, SDL_Renderer* renderer);
SDL_Texture* get_text(const char* text, const char* font_path, int size,
                      SDL_Color color, SDL_Renderer* renderer);

void init_board(Settings* s, Board* board, int play_size);
void init_scan_enemy(Board* board, GameState* gs, int enemy_color, int row,
                     int col);
void update_board(Settings* s, Board* board);
void capture_stones(GameState* gs);
int  check_for_suicide(Board* board, GameState* gs, int own_color, int row,
                       int col);
void scan_group_for_liberties(Board* board, GameState* gs, int enemy_color,
                              int row, int col);
void reset_liberty_scan_count_for_all_cells(Board* board, GameState* gs);

void process_click_on_board(Settings* s, Board* board, GameState* gs,
                            SDL_MouseMotionEvent m);
void get_score_text_black(GameState* gs);
void get_score_text_white(GameState* gs);
void reset_board(Board* board, GameState* gs);
void process_mouse_over_board(Settings* s, Board* board,
                              SDL_MouseMotionEvent m);

int is_cursor_within_board(Settings* s, SDL_MouseMotionEvent m);
int is_cursor_within_button(SDL_MouseMotionEvent m, const SDL_Rect b);

void get_display_modes(Settings* s);

void print_display_mode(const SDL_DisplayMode* mode);
void print_rect(int x, int y, int w, int h);
void get_window_size(Settings* s);

void mark_dead_stones(Board* board, GameState* gs, EndScore* es);

void reset_scan_count_for_all_cells(Board* board, GameState* gs);

void reset_border_scan_count_for_all_cells(Board* board, GameState* gs);

void determine_territory(Board* board, GameState* gs, EndScore* es);

void scan_empty_cells_for_ownership(Board* board, GameState* gs, EndScore* es,
                                    int row, int col);

void  place_on_pos(GameState* s, Board* board, const char* pos);
void* host(void* vargp);
void* join(void* vargp);
