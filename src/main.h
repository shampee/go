#include "common.h"

enum { EMPTY, BLACK, WHITE, OOB, OK, YES, NO };

void init_sdl(void);
void cleanup(void);

Cell* cell_create(void);

SDL_Texture* get_image(const char* file_name, SDL_Renderer* renderer);
SDL_Texture* get_text(const char* text, const char* font_path, int size,
                      SDL_Color color, SDL_Renderer* renderer);

void init_board(Board* board, int play_size);
void init_scan_enemy(Board* board, GameState* gs, int enemy_color, int row,
                     int col);
void capture_stones(GameState* gs);
int  check_for_suicide(Board* board, GameState* gs, int own_color, int row,
                       int col);
void scan_group_for_liberties(Board* board, GameState* gs, int enemy_color,
                              int row, int col);
void process_click_on_board(Board* board, GameState* gs,
                            SDL_MouseMotionEvent m);
void get_score_text_black(GameState* gs);
void get_score_text_white(GameState* gs);
void reset_board(Board* board, GameState* gs);
void process_mouse_over_board(Board* board, SDL_MouseMotionEvent m);

int is_cursor_within_board(SDL_MouseMotionEvent m);
int is_cursor_within_button(SDL_MouseMotionEvent m, const SDL_Rect b);

void getDisplayModes(Settings* s);

void printDisplayMode(const SDL_DisplayMode* mode);
