#include "main.h"
int main(int argc, char* argv[])
{
    memset(&app, 0, sizeof(App));

    Settings s = {
        .monitor = 0,
    };

    // Should we move these initialisations into some function?
    GameState gs = { .liberties         = 0,
                     .cells_scanned     = { NULL },
                     .stones_captured   = { NULL },
                     .count             = 0,
                     .capcount          = 0,
                     .ko_rule_black     = NULL,
                     .ko_rule_white     = NULL,
                     .stones_to_capture = NO,
                     .turn              = BLACK,
                     .score             = {
                         .black_sc_texture = NULL,
                         .white_sc_texture = NULL,
                         .black_sc_str     = { '\0' },
                         .white_sc_str     = { '\0' },
                         .current_black_score      = 0,
                         .current_white_score      = 0,
                         .prev_black_score = 0,
                         .prev_white_score = 0,
                     },
                     .board = {
                         .cell_array = { { NULL } },
                         .grid_cursor_ghost = NULL,
                         .play_size         = 13,
                    },
                     .hosting = SDL_FALSE,
    };

    init_sdl(&s, &gs, argc, argv);
    SDL_SetWindowTitle(app.window, "Go");
    print_display_mode(&s.display_mode);
    atexit(cleanup);

    int grid_size      = gs.board.play_size + 1;
    int grid_cell_size = s.window_size.h / (grid_size + 1);

    int row = 0;
    int col = 0;

    init_board(&s, &gs.board, gs.board.play_size);

    // Colors
    SDL_Color grid_background = { 222, 184, 135, 255 };
    SDL_Color grid_line_color = { 0, 0, 0, 255 };
    SDL_Color black           = { 0, 0, 0, 255 };
    SDL_Color white           = { 255, 255, 255, 255 };
    SDL_Color black_ter       = { 0, 0, 0, 127 };
    SDL_Color white_ter       = { 255, 255, 255, 127 };

    // load images for stones
    SDL_Texture* black_stone;
    SDL_Texture* white_stone;
    black_stone = get_image("assets/b.png", app.renderer);
    white_stone = get_image("assets/w.png", app.renderer);

    // dimensions for buttons to change players turn
    SDL_Rect blackb;
    SDL_Rect whiteb;

    blackb.w = grid_cell_size;
    blackb.h = grid_cell_size;
    blackb.x = (s.window_size.h + ((s.window_size.w - s.window_size.h) / 3));
    blackb.y = (s.window_size.h / (grid_size + 1)) * 3;

    whiteb.w = grid_cell_size;
    whiteb.h = grid_cell_size;
    whiteb.x = (s.window_size.h + ((s.window_size.w - s.window_size.h) / 3));
    whiteb.y = (s.window_size.h / (grid_size + 1)) * 5;

    // load text for reset board button
    SDL_Texture* reset_button_text;
    reset_button_text = get_text(
        " Reset Board ", "fonts/times-new-roman.ttf", 100, black, app.renderer);

    // load text for calculate territory button
    SDL_Texture* calc_territory_text;
    calc_territory_text = get_text(" Calculate Territory ",
                                   "fonts/times-new-roman.ttf",
                                   100,
                                   black,
                                   app.renderer);

    // cells to show score for black and white
    SDL_Rect black_sc_rect;
    SDL_Rect white_sc_rect;

    black_sc_rect.w = grid_cell_size;
    black_sc_rect.h = grid_cell_size;
    black_sc_rect.x = (blackb.x + grid_cell_size * 2);
    black_sc_rect.y = (blackb.y);

    white_sc_rect.w = grid_cell_size;
    white_sc_rect.h = grid_cell_size;
    white_sc_rect.x = (whiteb.x + grid_cell_size * 2);
    white_sc_rect.y = (whiteb.y);

    // get_score_text_black(&gs);
    sprintf(gs.score.black_sc_str, " %d ", gs.score.current_black_score);
    gs.score.black_sc_texture = get_text(gs.score.black_sc_str,
                                         "fonts/times-new-roman.ttf",
                                         100,
                                         gs.score.black,
                                         app.renderer);
    // get_score_text_white(&gs);
    sprintf(gs.score.white_sc_str, " %d ", gs.score.current_white_score);
    gs.score.white_sc_texture = get_text(gs.score.white_sc_str,
                                         "fonts/times-new-roman.ttf",
                                         100,
                                         gs.score.black,
                                         app.renderer);

    // dimensions for reset board button
    SDL_Rect reset_board_b;
    reset_board_b.w = grid_cell_size * 4;
    reset_board_b.h = grid_cell_size;
    reset_board_b.x =
        (s.window_size.h + ((s.window_size.w - s.window_size.h) / 6));
    reset_board_b.y = (s.window_size.h / (grid_size + 1)) * 7;

    // dimensions for calculate territory button
    SDL_Rect calc_territory_b;
    calc_territory_b.w = grid_cell_size * 4;
    calc_territory_b.h = grid_cell_size;
    calc_territory_b.x =
        (s.window_size.h + ((s.window_size.w - s.window_size.h) / 6));
    calc_territory_b.y = (s.window_size.h / (grid_size + 1)) * 9;

    // create textures for co ordinates on edges of board - alphabetical
    SDL_Texture* texture_array_alpha[grid_size];
    char         alphabet_char = 'A';
    char         alphabet_char_string[5];
    int          i;
    for (i = 0; i < gs.board.play_size; alphabet_char++, i++)
    {
        sprintf(alphabet_char_string, " %c ", alphabet_char);
        texture_array_alpha[i] = get_text(alphabet_char_string,
                                          "fonts/times-new-roman.ttf",
                                          100,
                                          black,
                                          app.renderer);
    }

    // create textures for co ordinates on edges of board - numerical
    SDL_Texture* texture_array_num[grid_size];
    int          num_char = gs.board.play_size;
    char         num_char_str[5];
    for (i = 0; i < gs.board.play_size; num_char--, i++)
    {
        sprintf(num_char_str, " %d ", num_char);
        texture_array_num[i] = get_text(num_char_str,
                                        "fonts/times-new-roman.ttf",
                                        100,
                                        black,
                                        app.renderer);
    }


    // store a position string (eg. B4) in each Cell - used for net stuff later on
    alphabet_char = 'A';
    alphabet_char--;
    for (int i = 0; i < gs.board.play_size + 1; alphabet_char++, i++)
    {
        for (int j = 0; j < gs.board.play_size + 1; num_char--, j++)
        {
            sprintf(gs.board.cell_array[j][i]->position_str,
                    "%c%d",
                    alphabet_char,
                    num_char);
            if (j == gs.board.play_size)
            {
                num_char = gs.board.play_size + 2;
            }
        }
    }


    // load image for dot
    SDL_Texture* dot_image;
    dot_image = get_image("assets/dot.png", app.renderer);

    SDL_bool quit         = SDL_FALSE;
    SDL_bool mouse_active = SDL_FALSE;
    SDL_bool mouse_hover  = SDL_FALSE;
    SDL_bool score_phase  = SDL_FALSE;

    while (!quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                // if in debug mode do this
                if (gs.game_mode == DEBUG)
                {
                    if (is_cursor_within_board(&s, event.motion))
                    {
                        // if game still going process placing stones
                        if (score_phase == SDL_FALSE)
                            process_click_on_board(&s, &gs, event.motion);
                        // if both players pass (not implemented yet), or the determine territory button is pressed, it is now the score phase - you can toggle stones dead or alive
                        else if (score_phase == SDL_TRUE)
                        {
                            toggle_dead_stones(&s, &gs, event.motion);
                            determine_territory(&gs);
                        }
                    }
                }
                // if in regular mode do this
                else if (gs.game_mode == REGULAR)
                {
                    // if game still going process placing stones
                    if (score_phase == SDL_FALSE && gs.player_color == gs.turn)
                        process_click_on_board(&s, &gs, event.motion);
                    // to do: score phase
                   
                }

                // clicking on black stone button makes turn black
                if (is_cursor_within_button(event.motion, blackb))
                    (&gs)->turn = BLACK;
                // clicking on white stone button makes turn white
                else if (is_cursor_within_button(event.motion, whiteb))
                    (&gs)->turn = WHITE;
                // clicking reset board button resets board
                else if (is_cursor_within_button(event.motion, reset_board_b))
                {
                    reset_board(&gs);
                    score_phase = SDL_FALSE;
                }
                // clicking determine territory button turns score phase on, marks black and white territory and dead stones
                else if (is_cursor_within_button(event.motion,
                                                 calc_territory_b))
                {
                    mark_dead_stones(&gs);
                    determine_territory(&gs);
                    score_phase = SDL_TRUE;
                }
                 // to do: pass turn button for each player to determine when placing stones has ended
                break;
            case SDL_MOUSEMOTION:
                // mouse over cells is a black or white square depending on whose turn it is
                if (is_cursor_within_board(&s, event.motion))
                    process_mouse_over_board(&s, &gs.board, event.motion);
                if (!mouse_active)
                    mouse_active = SDL_TRUE;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_ENTER && !mouse_hover)
                    mouse_hover = SDL_TRUE;
                else if (event.window.event == SDL_WINDOWEVENT_LEAVE &&
                         mouse_hover)
                    mouse_hover = SDL_FALSE;
                break;
            case SDL_QUIT:
                quit = SDL_TRUE;
                break;
            }
        }

        get_window_size(&s);
        update_board(&s, &gs.board);

        /*
        switch (gs.state)
        {
        case MENU:
            break;
        case HOST: {
            host_receive_stone(&gs);
            break;
        }
        case JOIN: {
            // pthread_t tid;
            // join(&gs);
            break;
        }
        case PLAY:
            break;
        }
        */

        // changes dimensions of textures and cells depending on size of window
        grid_cell_size = s.window_size.h / (grid_size + 1);
        blackb.w       = grid_cell_size;
        blackb.h       = grid_cell_size;
        blackb.x =
            (s.window_size.h + ((s.window_size.w - s.window_size.h) / 3));
        blackb.y = (s.window_size.h / (grid_size + 1)) * 3;

        whiteb.w = grid_cell_size;
        whiteb.h = grid_cell_size;
        whiteb.x =
            (s.window_size.h + ((s.window_size.w - s.window_size.h) / 3));
        whiteb.y = (s.window_size.h / (grid_size + 1)) * 5;

        black_sc_rect.w = grid_cell_size;
        black_sc_rect.h = grid_cell_size;
        black_sc_rect.x = (blackb.x + grid_cell_size * 2);
        black_sc_rect.y = (blackb.y);

        white_sc_rect.w = grid_cell_size;
        white_sc_rect.h = grid_cell_size;
        white_sc_rect.x = (whiteb.x + grid_cell_size * 2);
        white_sc_rect.y = (whiteb.y);

        reset_board_b.w = grid_cell_size * 4;
        reset_board_b.h = grid_cell_size;
        reset_board_b.x =
            (s.window_size.h + ((s.window_size.w - s.window_size.h) / 6));
        reset_board_b.y = (s.window_size.h / (grid_size + 1)) * 7;

        calc_territory_b.w = grid_cell_size * 4;
        calc_territory_b.h = grid_cell_size;
        calc_territory_b.x =
            (s.window_size.h + ((s.window_size.w - s.window_size.h) / 6));
        calc_territory_b.y = (s.window_size.h / (grid_size + 1)) * 9;

        // Draw grid background.
        SDL_SetRenderDrawColor(app.renderer,
                               grid_background.r,
                               grid_background.g,
                               grid_background.b,
                               grid_background.a);
        SDL_RenderClear(app.renderer);

        // Draw grid lines.
        SDL_SetRenderDrawColor(app.renderer,
                               grid_line_color.r,
                               grid_line_color.g,
                               grid_line_color.b,
                               grid_line_color.a);

        for (int x = grid_cell_size * 1.5;
             x < 1 + grid_cell_size * grid_size + 1;
             x += grid_cell_size)
        {
            SDL_RenderDrawLine(app.renderer,
                               x,
                               grid_cell_size * 1.5,
                               x,
                               (grid_cell_size * grid_size) -
                                   (grid_cell_size / 2));
        }

        for (int y = grid_cell_size * 1.5;
             y < 1 + grid_cell_size * grid_size + 1;
             y += grid_cell_size)
        {
            SDL_RenderDrawLine(app.renderer,
                               grid_cell_size * 1.5,
                               y,
                               (grid_cell_size * grid_size) -
                                   (grid_cell_size / 2),
                               y);
        }

        // Draw dots on board
        if (gs.board.play_size == 9)
        {
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[3][3]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[3][7]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[5][5]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[7][3]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[7][7]->dims);
        }

        else if (gs.board.play_size == 13)
        {
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[4][4]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[4][10]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[7][7]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[10][4]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[10][10]->dims);
        }

        else if (gs.board.play_size == 19)
        {
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[4][4]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[4][10]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[4][16]->dims);

            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[10][4]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[10][10]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[10][16]->dims);

            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[16][4]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[16][10]->dims);
            SDL_RenderCopy(app.renderer,
                           dot_image,
                           NULL,
                           &gs.board.cell_array[16][16]->dims);
        }

        // Draw co ordinates
        for (row = 0, col = 1, i = 0; i < gs.board.play_size; col++, i++)
            SDL_RenderCopy(app.renderer,
                           texture_array_alpha[i],
                           NULL,
                           &gs.board.cell_array[row][col]->dims);

        for (row = grid_size, col = 1, i = 0; i < gs.board.play_size;
             col++, i++)
            SDL_RenderCopy(app.renderer,
                           texture_array_alpha[i],
                           NULL,
                           &gs.board.cell_array[row][col]->dims);

        for (row = 1, col = 0, i = 0; i < gs.board.play_size; row++, i++)
            SDL_RenderCopy(app.renderer,
                           texture_array_num[i],
                           NULL,
                           &gs.board.cell_array[row][col]->dims);

        for (row = 1, col = grid_size, i = 0; i < gs.board.play_size;
             row++, i++)
            SDL_RenderCopy(app.renderer,
                           texture_array_num[i],
                           NULL,
                           &gs.board.cell_array[row][col]->dims);

        // Draw white and black buttons
        SDL_RenderCopy(app.renderer, black_stone, NULL, &blackb);
        SDL_RenderCopy(app.renderer, white_stone, NULL, &whiteb);

        // Draw reset button - it would seem things are rendered on top of what was rendered before, so the order matters
        SDL_SetRenderDrawColor(
            app.renderer, white.r, white.g, white.b, white.a);
        SDL_RenderFillRect(app.renderer, &reset_board_b);
        SDL_RenderCopy(app.renderer, reset_button_text, NULL, &reset_board_b);

        // Draw calculate territory button
        SDL_SetRenderDrawColor(
            app.renderer, white.r, white.g, white.b, white.a);
        SDL_RenderFillRect(app.renderer, &calc_territory_b);
        SDL_RenderCopy(
            app.renderer, calc_territory_text, NULL, &calc_territory_b);

        // Draw grid ghost cursor.
        if (mouse_active && mouse_hover)
        {
            if (gs.board.grid_cursor_ghost->cell_value == EMPTY &&
                score_phase == SDL_FALSE)
            {
                // set render color to black if turn is black
                if (gs.turn == BLACK)
                    SDL_SetRenderDrawColor(
                        app.renderer, black.r, black.g, black.b, black.a);
                // set render color to white if turn is white
                else if (gs.turn == WHITE)
                    SDL_SetRenderDrawColor(
                        app.renderer, white.r, white.g, white.b, white.a);

                // draw ghost cursor only when it is debug mode or if it is the players turn
                if (gs.game_mode == DEBUG || gs.turn == gs.player_color)
                    SDL_RenderFillRect(app.renderer,
                                       &gs.board.grid_cursor_ghost->dims);
            }
        }
        // Draw stones and territory
        for (row = 0, col = 0; row <= grid_size;)
        {
            
            // draw black territory
            if (gs.board.cell_array[row][col]->territory_value == BLACK_T)
            {
                SDL_SetRenderDrawColor(app.renderer,
                                       black_ter.r,
                                       black_ter.g,
                                       black_ter.b,
                                       black_ter.a);
                SDL_RenderFillRect(app.renderer,
                                   &gs.board.cell_array[row][col]->dims);
            }
            // draw white territory
            else if (gs.board.cell_array[row][col]->territory_value == WHITE_T)
            {
                SDL_SetRenderDrawColor(app.renderer,
                                       white_ter.r,
                                       white_ter.g,
                                       white_ter.b,
                                       white_ter.a);
                SDL_RenderFillRect(app.renderer,
                                   &gs.board.cell_array[row][col]->dims);
            }
            // draw black stones
            if (gs.board.cell_array[row][col]->cell_value == BLACK)
                SDL_RenderCopy(app.renderer,
                               black_stone,
                               NULL,
                               &gs.board.cell_array[row][col]->dims);
            // draw white stones
            else if (gs.board.cell_array[row][col]->cell_value == WHITE)
                SDL_RenderCopy(app.renderer,
                               white_stone,
                               NULL,
                               &gs.board.cell_array[row][col]->dims);

            col++;
            if (col > grid_size)
            {
                col = 0;
                row++;
            }
        }

        // SDL_RenderSetScale(app.renderer, 1, 1);
        // SDL_RenderSetLogicalSize(
        //     app.renderer, s.display_mode.w, s.display_mode.h);
        // SDL_Rect r;
        // printRect(r.x, r.y, r.w, r.h);
        // printRect(s.window_size.w, s.window_size.h, 0, 0);


        // if score changes for black or white update textures for the score
        get_score_text_black(&gs);
        get_score_text_white(&gs);
        // renders the score boxes for black and white
        SDL_RenderCopy(
            app.renderer, gs.score.black_sc_texture, NULL, &black_sc_rect);
        SDL_RenderCopy(
            app.renderer, gs.score.white_sc_texture, NULL, &white_sc_rect);

        SDL_RenderPresent(app.renderer);

        // Dirty
        switch (gs.state)
        {
        case MENU:
            break;
        case HOST: {
            if (!gs.net.got_client)
                get_client(&gs);
            if (!gs.hosting)
            {
                SDL_Thread* thread;
                thread =
                    SDL_CreateThread((SDL_ThreadFunction)send_or_receive_stones,
                                     "send_or_receive_stones",
                                     (GameState*)&gs);

                gs.hosting = SDL_TRUE;
            }
            break;
        }
        case JOIN: {
            if (!gs.joining)
            {
                SDL_Thread* thread;
                thread =
                    SDL_CreateThread((SDL_ThreadFunction)send_or_receive_stones,
                                     "send_or_receive_stones",
                                     (GameState*)&gs);
                gs.joining = SDL_TRUE;
            }

            break;
        }
        case PLAY:
            break;
        }
        // delay to stop cpu going 100%
        SDL_Delay(1000 / 60);
    }
    // closes ports when program quits when 'x' in corner is clicked
    SDLNet_TCP_Close(gs.net.server);
    SDLNet_TCP_Close(gs.net.client);

    return 0;
}

void init_sdl(Settings* s, GameState* gs, char argc, char* argv[])
{
    int rendererFlags, windowFlags;

    rendererFlags = SDL_RENDERER_ACCELERATED;

    windowFlags = SDL_WINDOW_RESIZABLE;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
    // We need functionality so we can choose, like a menu
    get_display_modes(s);
    s->display_mode = s->available_modes.modes[0];

    // Initialize window size
    s->window_size.w = s->display_mode.w;
    s->window_size.h = s->display_mode.h;
    // Initialize ratio
    s->aspect_ratio = (float)s->display_mode.w / (float)s->display_mode.h;

    app.window = SDL_CreateWindow(
        "Go", 0, 0, s->display_mode.w, s->display_mode.h, windowFlags);

    if (!app.window)
    {
        printf("Failed to open %d x %d window: %s\n",
               s->display_mode.w,
               s->display_mode.h,
               SDL_GetError());
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

    if (!app.renderer)
    {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        exit(1);
    }

    // Initialize support for loading png images - can add different img types as flags
    IMG_Init(IMG_INIT_PNG);

    //Initialize support for fonts
    TTF_Init();

    //Initialize net
    if (SDLNet_Init() < 0)
    {
        printf("Failed to initialize SDLNet: %s\n", SDLNet_GetError());
        exit(1);
    }
    if (argc > 1)
    {
        gs->game_mode = REGULAR;
        if (strcmp(argv[1], "host") == 0)
        {
            gs->state = HOST;
            printf("Host mode\n");
        }
        else if (strcmp(argv[1], "join") == 0)
        {
            gs->state = JOIN;
            printf("Join mode\n");
        }
        // Get port from argv
        if (argc > 2)
            gs->net.port = (Uint16)strtol(argv[2], NULL, 0);
        else
            // Default port
            gs->net.port = (Uint16)7777;

        if (gs->state == HOST)
        {
            if (SDLNet_ResolveHost(&gs->net.ip, NULL, gs->net.port) < 0)
            {
                fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                exit(1);
            }
            if (!(gs->net.server = SDLNet_TCP_Open(&gs->net.ip)))
            {
                fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
                exit(1);
            }
            gs->net.got_client = 0;
            gs->player_color   = BLACK;
        }

        else if (gs->state == JOIN)
        {
            if (SDLNet_ResolveHost(&gs->net.ip, "127.0.0.1", gs->net.port) < 0)
            {
                fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                exit(1);
            }
            if (!(gs->net.client = SDLNet_TCP_Open(&gs->net.ip)))
            {
                fprintf(stderr, "SDLNet_TCP_Open: %s\n", SDLNet_GetError());
                exit(EXIT_FAILURE);
            }
            gs->player_color = WHITE;
        }
    }
    else
    {
        gs->game_mode = DEBUG;
        printf("Debug mode\n");
    }

    // For having semi-transparent things rendered
    SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
}

void cleanup(void)
{
    SDL_DestroyRenderer(app.renderer);

    SDL_DestroyWindow(app.window);

    IMG_Quit();

    TTF_Quit();

    SDLNet_Quit();

    // SDLNet_TCP_Close(gs->net.server);
    // SDLNet_TCP_Close(gs->net.client);

    SDL_Quit();
}

Cell* cell_create(void)
{
    Cell* cell = (Cell*)calloc(1, sizeof(Cell));
    return cell;
}

SDL_Texture* get_image(const char* file_name, SDL_Renderer* renderer)
{
    SDL_Surface* image = IMG_Load(file_name);
    SDL_Texture* image_texture =
        SDL_CreateTextureFromSurface(app.renderer, image);
    SDL_FreeSurface(image);
    return image_texture;
}

SDL_Texture* get_text(const char* text, const char* font_path, int size,
                      SDL_Color color, SDL_Renderer* renderer)
{
    TTF_Font*    font_and_size = TTF_OpenFont(font_path, size);
    SDL_Surface* surface_message =
        TTF_RenderText_Blended(font_and_size, text, color);
    SDL_Texture* message_texture =
        SDL_CreateTextureFromSurface(app.renderer, surface_message);
    SDL_FreeSurface(surface_message);
    return message_texture;
}
void init_board(Settings* s, Board* board, int play_size)
{
    int row            = 0;
    int col            = 0;
    int x              = 0;
    int y              = 0;
    int grid_size      = board->play_size + 1;
    int grid_cell_size = s->window_size.h / (grid_size + 1);

    while (row <= grid_size)
    {
        board->cell_array[row][col] = cell_create();

        if (row == 0 || row == grid_size || col == 0 || col == grid_size)
            board->cell_array[row][col]->cell_value = OOB;
        else
            board->cell_array[row][col]->cell_value = EMPTY;

        board->cell_array[row][col]->territory_value = NO_T;

        board->cell_array[row][col]->dims.w                   = grid_cell_size;
        board->cell_array[row][col]->dims.h                   = grid_cell_size;
        board->cell_array[row][col]->dims.x                   = x;
        board->cell_array[row][col]->dims.y                   = y;
        board->cell_array[row][col]->has_cell_been_scanned    = NO;
        board->cell_array[row][col]->has_liberty_been_scanned = NO;
        board->cell_array[row][col]->has_border_been_scanned  = NO;

        x += grid_cell_size;
        col++;

        if (col > grid_size)
        {
            col = 0;
            x   = 0;
            row++;
            y += grid_cell_size;
        }
    }

    board->grid_cursor_ghost             = cell_create();
    board->grid_cursor_ghost->cell_value = OOB;
    board->play_size                     = play_size;
}

void update_board(Settings* s, Board* board)
{
    int row            = 0;
    int col            = 0;
    int x              = 0;
    int y              = 0;
    int grid_size      = board->play_size + 1;
    int grid_cell_size = s->window_size.h / (grid_size + 1);

    while (row <= grid_size)
    {
        board->cell_array[row][col]->dims.w = grid_cell_size;
        board->cell_array[row][col]->dims.h = grid_cell_size;
        board->cell_array[row][col]->dims.x = x;
        board->cell_array[row][col]->dims.y = y;

        x += grid_cell_size;
        col++;

        if (col > grid_size)
        {
            col = 0;
            x   = 0;
            row++;
            y += grid_cell_size;
        }
    }
}

void handle_turn(GameState* gs, int row, int col, int colour)
{
    int enemy_colour;

    if (colour == BLACK)
    {
        enemy_colour = WHITE;
    }

    else
    {
        enemy_colour = BLACK;
    }
        
    
    gs->board.cell_array[row][col]->cell_value = colour;
    gs->to_be_placed                           = gs->board.cell_array[row][col];
    gs->stones_to_capture                      = NO;
    
    // scans the enemy group (if there is one)
    // for liberties directly above the placed black stone
    init_scan_enemy(gs, enemy_colour, row - 1, col);
    // scans the enemy group directly to the right of the placed black stone
    init_scan_enemy(gs, enemy_colour, row, col + 1);
    // scans the enemy group directly below the placed black stone
    init_scan_enemy(gs, enemy_colour, row + 1, col);
    // scans the enemy group directly to the left of the placed black stone
    init_scan_enemy(gs, enemy_colour, row, col - 1);

    // Check for Ko
    if (is_there_ko(gs, colour, row, col) == YES)
    {
        printf("Rule: ko, also known as infinity - you cannot "
               "place the stone in the same cell as your "
               "previous move\n\n");
        gs->board.cell_array[row][col]->cell_value = EMPTY;
        while (gs->capcount > 0)
            gs->stones_captured[gs->capcount--]->has_cell_been_scanned = NO;
        return;
    }

    capture_stones(gs);
    get_score_text_black(gs);

    if (check_for_suicide(gs, colour, row, col) == OK)
    {
        if (colour == BLACK)
            gs->ko_rule_black = gs->board.cell_array[row][col];
        else
            gs->ko_rule_white = gs->board.cell_array[row][col];
    }
}

int is_there_ko(GameState* gs, int own_color, int row, int col)
{
    if (own_color == BLACK)
    {
        if(gs->stones_to_capture == YES &&
        gs->ko_rule_black == gs->board.cell_array[row][col])
            return YES;
    }
        
    else
    {
        if(gs->stones_to_capture == YES &&
        gs->ko_rule_white == gs->board.cell_array[row][col])
            return YES;
    }
        
    return NO;

}
void process_click_on_board(Settings* s, GameState* gs, SDL_MouseMotionEvent m)
{
    int row            = 0;
    int col            = 0;
    int grid_size      = gs->board.play_size + 1;
    int grid_cell_size = s->window_size.h / (grid_size + 1);

    while (row <= grid_size)
    {
        if (m.x > gs->board.cell_array[row][col]->dims.x &&
            m.y > gs->board.cell_array[row][col]->dims.y &&
            m.x < (gs->board.cell_array[row][col]->dims.x) + grid_cell_size &&
            m.y < (gs->board.cell_array[row][col]->dims.y) + grid_cell_size)
        {
            if (gs->board.cell_array[row][col]->cell_value == EMPTY)
            {
                gs->to_be_placed = gs->board.cell_array[row][col];
                if (gs->turn == BLACK)
                    handle_turn(gs, row, col, BLACK);
                else if (gs->turn == WHITE)
                    handle_turn(gs, row, col, WHITE);
            }
            else
                gs->to_be_placed = NULL;
            return;
        }

        col++;
        if (col > grid_size)
        {
            col = 0;
            row++;
        }
    }
    gs->to_be_placed = NULL;
}

// The first stone needs to be scanned before scan_group_for_liberties
// scans all of the stones of the same color that are connected to the first stone,
// the address of all cells of the stones in the group are stored in
// the cells_scanned array, which will later be used to remove the
// stones off of the board (if no liberties are found),
// by changing the cell value of each cell to EMPTY
void init_scan_enemy(GameState* gs, int enemy_color, int row, int col)
{
    if (gs->board.cell_array[row][col]->cell_value == enemy_color &&
        gs->board.cell_array[row][col]->has_cell_been_scanned == NO)
    {
        gs->board.cell_array[row][col]->has_cell_been_scanned = YES;
        gs->cells_scanned[++gs->count] = gs->board.cell_array[row][col];

        scan_group_for_liberties(gs, enemy_color, row, col);

        if (gs->liberties == 0)
        {
            while (gs->count > 0)
                gs->stones_captured[++gs->capcount] =
                    gs->cells_scanned[gs->count--];
            gs->stones_to_capture = YES;
        }
        else if (gs->liberties != 0)
        {
            while (gs->count > 0)
                gs->cells_scanned[gs->count--]->has_cell_been_scanned = NO;
            gs->liberties = 0;
        }
        reset_liberty_scan_count_for_all_cells(gs);
    }
}

void scan_group_for_liberties(GameState* gs, int target_color, int row, int col)
{
    if (gs->board.cell_array[row - 1][col]->cell_value == target_color &&
        gs->board.cell_array[row - 1][col]->has_cell_been_scanned == NO)
    {
        gs->board.cell_array[row - 1][col]->has_cell_been_scanned = YES;
        gs->cells_scanned[++gs->count] = gs->board.cell_array[row - 1][col];
        scan_group_for_liberties(gs, target_color, row - 1, col);
    }
    else if (gs->board.cell_array[row - 1][col]->cell_value == EMPTY &&
             gs->board.cell_array[row - 1][col]->has_liberty_been_scanned == NO)
    {
        gs->board.cell_array[row - 1][col]->has_liberty_been_scanned = YES;
        ++gs->liberties;
    }

    if (gs->board.cell_array[row][col + 1]->cell_value == target_color &&
        gs->board.cell_array[row][col + 1]->has_cell_been_scanned == NO)
    {
        gs->board.cell_array[row][col + 1]->has_cell_been_scanned = YES;
        gs->cells_scanned[++gs->count] = gs->board.cell_array[row][col + 1];
        scan_group_for_liberties(gs, target_color, row, col + 1);
    }
    else if (gs->board.cell_array[row][col + 1]->cell_value == EMPTY &&
             gs->board.cell_array[row][col + 1]->has_liberty_been_scanned == NO)
    {
        gs->board.cell_array[row][col + 1]->has_liberty_been_scanned = YES;
        ++gs->liberties;
    }

    if (gs->board.cell_array[row + 1][col]->cell_value == target_color &&
        gs->board.cell_array[row + 1][col]->has_cell_been_scanned == NO)
    {
        gs->board.cell_array[row + 1][col]->has_cell_been_scanned = YES;
        gs->cells_scanned[++gs->count] = gs->board.cell_array[row + 1][col];
        scan_group_for_liberties(gs, target_color, row + 1, col);
    }
    else if (gs->board.cell_array[row + 1][col]->cell_value == EMPTY &&
             gs->board.cell_array[row + 1][col]->has_liberty_been_scanned == NO)
    {
        gs->board.cell_array[row + 1][col]->has_liberty_been_scanned = YES;
        ++gs->liberties;
    }

    if (gs->board.cell_array[row][col - 1]->cell_value == target_color &&
        gs->board.cell_array[row][col - 1]->has_cell_been_scanned == NO)
    {
        gs->board.cell_array[row][col - 1]->has_cell_been_scanned = YES;
        gs->cells_scanned[++gs->count] = gs->board.cell_array[row][col - 1];
        scan_group_for_liberties(gs, target_color, row, col - 1);
    }
    else if (gs->board.cell_array[row][col - 1]->cell_value == EMPTY &&
             gs->board.cell_array[row][col - 1]->has_liberty_been_scanned == NO)
    {
        gs->board.cell_array[row][col - 1]->has_liberty_been_scanned = YES;
        ++gs->liberties;
    }
}

void capture_stones(GameState* gs)
{
    if (gs->turn == BLACK)
        gs->score.current_black_score += gs->capcount;
    else if (gs->turn == WHITE)
        gs->score.current_white_score += gs->capcount;
    while (gs->capcount > 0)
    {
        gs->stones_captured[gs->capcount]->cell_value            = EMPTY;
        gs->stones_captured[gs->capcount]->has_cell_been_scanned = NO;
        --gs->capcount;
    }
}

void get_score_text_black(GameState* gs)
{
    if (gs->score.current_black_score != gs->score.prev_black_score)
    {
        sprintf(gs->score.black_sc_str, " %d ", gs->score.current_black_score);
        gs->score.black_sc_texture = get_text(gs->score.black_sc_str,
                                              "fonts/times-new-roman.ttf",
                                              100,
                                              gs->score.black,
                                              app.renderer);
        gs->score.prev_black_score = gs->score.current_black_score;
    }
}

void get_score_text_white(GameState* gs)
{
    if (gs->score.current_white_score != gs->score.prev_white_score)
    {
        sprintf(gs->score.white_sc_str, " %d ", gs->score.current_white_score);
        gs->score.white_sc_texture = get_text(gs->score.white_sc_str,
                                              "fonts/times-new-roman.ttf",
                                              100,
                                              gs->score.black,
                                              app.renderer);
        gs->score.prev_white_score = gs->score.current_white_score;
    }
}

int check_for_suicide(GameState* gs, int own_color, int row, int col)
{
    scan_group_for_liberties(gs, own_color, row, col);

    if (gs->liberties == 0)
    {
        gs->board.cell_array[row][col]->cell_value = EMPTY;
        printf("Rule: You can not reduce your own group of stones liberties to "
               "0, or otherwise place a stone where there are no liberties for "
               "it\n\n");
        while (gs->count > 0)
            gs->cells_scanned[gs->count--]->has_cell_been_scanned = NO;
        gs->to_be_placed = NULL;
        return !OK;
    }
    else
    {
        while (gs->count > 0)
            gs->cells_scanned[gs->count--]->has_cell_been_scanned = NO;
        gs->liberties = 0;
        reset_liberty_scan_count_for_all_cells(gs);
        return OK;
    }
}

void reset_board(GameState* gs)
{
    int row       = 0;
    int col       = 0;
    int grid_size = gs->board.play_size + 1;

    while (row <= grid_size)
    {
        if (gs->board.cell_array[row][col]->cell_value == BLACK ||
            gs->board.cell_array[row][col]->cell_value == WHITE)
            gs->board.cell_array[row][col]->cell_value = EMPTY;
        gs->board.cell_array[row][col]->territory_value          = NO_T;
        gs->board.cell_array[row][col]->has_cell_been_scanned    = NO;
        gs->board.cell_array[row][col]->has_liberty_been_scanned = NO;
        gs->board.cell_array[row][col]->has_border_been_scanned  = NO;
        col++;
        if (col > grid_size)
        {
            col = 0;
            row++;
        }
    }
    gs->ko_rule_black = NULL;
    gs->ko_rule_white = NULL;
}

void process_mouse_over_board(Settings* s, Board* board, SDL_MouseMotionEvent m)
{
    int row            = 0;
    int col            = 0;
    int grid_size      = board->play_size + 1;
    int grid_cell_size = s->window_size.h / (grid_size + 1);

    while (row <= grid_size)
    {
        if (m.x > board->cell_array[row][col]->dims.x &&
            m.y > board->cell_array[row][col]->dims.y &&
            m.x < (board->cell_array[row][col]->dims.x) + grid_cell_size &&
            m.y < (board->cell_array[row][col]->dims.y) + grid_cell_size)
        {
            board->grid_cursor_ghost = board->cell_array[row][col];
            break;
        }

        col++;
        if (col > grid_size)
        {
            col = 0;
            row++;
        }
    }
}

int is_cursor_within_board(Settings* s, const SDL_MouseMotionEvent m)
{
    return (m.x > 0 && m.y > 0 && m.x < s->window_size.h &&
            m.y < s->window_size.h);
}

int is_cursor_within_button(const SDL_MouseMotionEvent m, const SDL_Rect b)
{
    return (m.x > b.x && m.y > b.y && m.x < (b.x + b.w) && m.y < (b.y + b.h));
}

void print_display_mode(const SDL_DisplayMode* mode)
{
    printf("{bpp: %i, format: %s, w: %i, h: %i, hz: %i}\n",
           SDL_BITSPERPIXEL(mode->format),
           SDL_GetPixelFormatName(mode->format),
           mode->w,
           mode->h,
           mode->refresh_rate);
}

// Fills up the s.available_modes.modes with all modes
// available to the monitor that s.monitor is set to.
void get_display_modes(Settings* s)
{
    int n = SDL_GetNumDisplayModes(s->monitor);

    SDL_DisplayMode mode;
    s->available_modes.modes = calloc(n, sizeof(SDL_DisplayMode));
    s->available_modes.len   = n;

    // The more obvious way didn't seem to work
    // so we're going with the hacky way now
    for (int i = 0; i < n; i++)
    {
        if (SDL_GetDisplayMode(s->monitor, i, &mode) != 0)
            SDL_Log("SDL_GetDisplayMode failed: %s", SDL_GetError());
        s->available_modes.modes[i] = mode;
    }
}
void print_rect(int x, int y, int w, int h)
{
    if (x < 0 && y < 0)
        printf("{w: %i, h: %i}\n", w, h);
    else
        printf("{x: %i, y: %i, w: %i, h: %i}\n", x, y, w, h);
}

void get_window_size(Settings* s)
{
    // SDL_GL_GetDrawableSize(app.window, &s->window_size.w, &s->window_size.h);
    SDL_GetWindowSize(app.window, &s->window_size.w, &s->window_size.h);
}

void mark_dead_stones(GameState* gs)
{
    int row       = 0;
    int col       = 0;
    int grid_size = gs->board.play_size + 1;
    int mark_color;
    int territory_color;

    int loop_counter = 2;
    int stone_color  = BLACK;

    while (loop_counter > 0)
    {
        if (stone_color == BLACK)
            mark_color = WHITE_T;
        if (stone_color == WHITE)
            mark_color = BLACK_T;
        row = 0;
        col = 0;

        while (row <= grid_size)
        {
            if (gs->board.cell_array[row][col]->cell_value == stone_color &&
                gs->board.cell_array[row][col]->has_cell_been_scanned == NO)
            {
                gs->board.cell_array[row][col]->has_cell_been_scanned = YES;
                gs->cells_scanned[++gs->count] = gs->board.cell_array[row][col];

                scan_group_for_liberties(gs, stone_color, row, col);

                if (gs->liberties == 1)
                {
                    while (gs->count > 0)
                        gs->cells_scanned[gs->count--]->territory_value =
                            mark_color;
                }
                gs->liberties                                  = 0;
                gs->count                                      = 0;
                gs->end_score.black_stones_next_to_empty_cells = 0;
                gs->end_score.white_stones_next_to_empty_cells = 0;
                reset_liberty_scan_count_for_all_cells(gs);
            }

            col++;

            if (col > grid_size)
            {
                col = 0;
                row++;
            }
        }
        reset_scan_count_for_all_cells(gs);
        stone_color = WHITE;
        --loop_counter;
    }

    loop_counter = 2;
    stone_color  = BLACK;
    int   count_storage;
    Cell* cell_storage[5];
    int   i = 0;

    while (loop_counter > 0)
    {
        if (stone_color == BLACK)
            mark_color = WHITE_T;
        if (stone_color == WHITE)
            mark_color = BLACK_T;

        row = 0;
        col = 0;
        i   = 0;

        while (row <= grid_size)
        {
            if (gs->board.cell_array[row][col]->cell_value == stone_color &&
                gs->board.cell_array[row][col]->has_cell_been_scanned == NO)
            {
                gs->board.cell_array[row][col]->has_cell_been_scanned = YES;
                gs->cells_scanned[++gs->count] = gs->board.cell_array[row][col];

                scan_group_for_liberties(gs, stone_color, row, col);

                if (gs->count == 2)
                {
                    count_storage = gs->count;
                    while (gs->count > 0)
                    {
                        cell_storage[++i] = gs->cells_scanned[gs->count];
                        gs->cells_scanned[gs->count]->cell_value = EMPTY;
                        --gs->count;
                    }

                    scan_empty_cells_for_ownership(gs, row, col);

                    if (gs->end_score.black_stones_next_to_empty_cells >
                        gs->end_score.white_stones_next_to_empty_cells)
                        territory_color = BLACK;
                    else if (gs->end_score.black_stones_next_to_empty_cells <
                             gs->end_score.white_stones_next_to_empty_cells)
                        territory_color = WHITE;

                    if (stone_color != territory_color)
                    {
                        while (i > 0)
                            cell_storage[i--]->territory_value = mark_color;
                        i = count_storage;
                    }

                    while (i > 0)
                        cell_storage[i--]->cell_value = stone_color;
                }

                gs->liberties                                  = 0;
                gs->count                                      = 0;
                gs->end_score.black_stones_next_to_empty_cells = 0;
                gs->end_score.white_stones_next_to_empty_cells = 0;
                reset_liberty_scan_count_for_all_cells(gs);
                reset_border_scan_count_for_all_cells(gs);
            }

            col++;

            if (col > grid_size)
            {
                col = 0;
                row++;
            }
        }
        reset_scan_count_for_all_cells(gs);
        stone_color = WHITE;
        --loop_counter;
    }

    loop_counter = 2;
    stone_color  = BLACK;
    i            = 0;

    while (loop_counter > 0)
    {
        if (stone_color == BLACK)
            mark_color = WHITE_T;
        if (stone_color == WHITE)
            mark_color = BLACK_T;

        row = 0;
        col = 0;
        i   = 0;

        while (row <= grid_size)
        {
            if (gs->board.cell_array[row][col]->cell_value == stone_color &&
                gs->board.cell_array[row][col]->has_cell_been_scanned == NO)
            {
                gs->board.cell_array[row][col]->has_cell_been_scanned = YES;
                gs->cells_scanned[++gs->count] = gs->board.cell_array[row][col];

                scan_group_for_liberties(gs, stone_color, row, col);

                if (gs->count == 1)
                {
                    count_storage = gs->count;
                    while (gs->count > 0)
                    {
                        cell_storage[++i] = gs->cells_scanned[gs->count];
                        gs->cells_scanned[gs->count]->cell_value = EMPTY;
                        --gs->count;
                    }

                    scan_empty_cells_for_ownership(gs, row, col);

                    if (gs->end_score.black_stones_next_to_empty_cells >
                        gs->end_score.white_stones_next_to_empty_cells)
                        territory_color = BLACK;
                    else if (gs->end_score.black_stones_next_to_empty_cells <
                             gs->end_score.white_stones_next_to_empty_cells)
                        territory_color = WHITE;

                    if (stone_color != territory_color)
                    {
                        while (i > 0)
                            cell_storage[i--]->territory_value = mark_color;
                        i = count_storage;
                    }

                    while (i > 0)
                        cell_storage[i--]->cell_value = stone_color;
                }

                gs->liberties                                  = 0;
                gs->count                                      = 0;
                gs->end_score.black_stones_next_to_empty_cells = 0;
                gs->end_score.white_stones_next_to_empty_cells = 0;
                reset_liberty_scan_count_for_all_cells(gs);
                reset_border_scan_count_for_all_cells(gs);
            }

            col++;

            if (col > grid_size)
            {
                col = 0;
                row++;
            }
        }
        reset_scan_count_for_all_cells(gs);
        stone_color = WHITE;
        --loop_counter;
    }
}

void reset_scan_count_for_all_cells(GameState* gs)
{
    int row       = 0;
    int col       = 0;
    int grid_size = gs->board.play_size + 1;

    while (row <= grid_size)
    {
        gs->board.cell_array[row][col]->has_cell_been_scanned = NO;
        col++;
        if (col > grid_size)
        {
            col = 0;
            row++;
        }
    }
}

void reset_liberty_scan_count_for_all_cells(GameState* gs)
{
    int row       = 0;
    int col       = 0;
    int grid_size = gs->board.play_size + 1;

    while (row <= grid_size)
    {
        gs->board.cell_array[row][col]->has_liberty_been_scanned = NO;
        col++;
        if (col > grid_size)
        {
            col = 0;
            row++;
        }
    }
}

void reset_border_scan_count_for_all_cells(GameState* gs)
{
    int row       = 0;
    int col       = 0;
    int grid_size = gs->board.play_size + 1;

    while (row <= grid_size)
    {
        gs->board.cell_array[row][col]->has_border_been_scanned = NO;
        col++;
        if (col > grid_size)
        {
            col = 0;
            row++;
        }
    }
}

void determine_territory(GameState* gs)
{
    int row       = 0;
    int col       = 0;
    int grid_size = gs->board.play_size + 1;

    while (row <= grid_size)
    {
        if (gs->board.cell_array[row][col]->cell_value == EMPTY &&
            gs->board.cell_array[row][col]->has_cell_been_scanned == NO)
        {
            gs->board.cell_array[row][col]->has_cell_been_scanned = YES;
            gs->cells_scanned[++gs->count] = gs->board.cell_array[row][col];

            scan_empty_cells_for_ownership(gs, row, col);

            if (gs->count <= 2 &&
                gs->cells_scanned[gs->count]->cell_value == EMPTY)
            {
                if (gs->end_score.black_stones_next_to_empty_cells >= 1 &&
                    gs->end_score.white_stones_next_to_empty_cells >= 1)
                    while (gs->count > 0)
                        gs->cells_scanned[gs->count--]->territory_value = NO_T;

                else if (gs->end_score.black_stones_next_to_empty_cells >
                         gs->end_score.white_stones_next_to_empty_cells)
                    while (gs->count > 0)
                        gs->cells_scanned[gs->count--]->territory_value =
                            BLACK_T;

                else if (gs->end_score.black_stones_next_to_empty_cells <
                         gs->end_score.white_stones_next_to_empty_cells)
                    while (gs->count > 0)
                        gs->cells_scanned[gs->count--]->territory_value =
                            WHITE_T;
            }

            else if (gs->end_score.black_stones_next_to_empty_cells >
                     gs->end_score.white_stones_next_to_empty_cells)
            {
                while (gs->count > 0)
                {
                    if (gs->cells_scanned[gs->count]->cell_value == EMPTY)
                        gs->cells_scanned[gs->count]->territory_value = BLACK_T;
                    --gs->count;
                }
            }
            else if (gs->end_score.black_stones_next_to_empty_cells <
                     gs->end_score.white_stones_next_to_empty_cells)
            {
                while (gs->count > 0)
                {
                    if (gs->cells_scanned[gs->count]->cell_value == EMPTY)
                        gs->cells_scanned[gs->count]->territory_value = WHITE_T;
                    --gs->count;
                }
            }
            else
            {
                while (gs->count > 0)
                {
                    if (gs->cells_scanned[gs->count]->cell_value == EMPTY)
                        gs->cells_scanned[gs->count]->territory_value = NO_T;
                    --gs->count;
                }
            }
            gs->count                                      = 0;
            gs->end_score.black_stones_next_to_empty_cells = 0;
            gs->end_score.white_stones_next_to_empty_cells = 0;
            reset_border_scan_count_for_all_cells(gs);
        }

        col++;

        if (col > grid_size)
        {
            col = 0;
            row++;
        }
    }
    reset_scan_count_for_all_cells(gs);
}

void toggle_dead_stones(Settings* s, GameState* gs, SDL_MouseMotionEvent m)
{
    int row            = 0;
    int col            = 0;
    int grid_size      = gs->board.play_size + 1;
    int grid_cell_size = s->window_size.h / (grid_size + 1);

    while (row <= grid_size)
    {
        if (m.x > gs->board.cell_array[row][col]->dims.x &&
            m.y > gs->board.cell_array[row][col]->dims.y &&
            m.x < (gs->board.cell_array[row][col]->dims.x) + grid_cell_size &&
            m.y < (gs->board.cell_array[row][col]->dims.y) + grid_cell_size)
        {
            if (gs->board.cell_array[row][col]->cell_value == BLACK)
            {
                gs->cells_scanned[++gs->count] = gs->board.cell_array[row][col];
                scan_group_for_liberties(gs, BLACK, row, col);

                if (gs->board.cell_array[row][col]->territory_value == NO_T)
                {
                    while (gs->count > 0)
                        gs->cells_scanned[gs->count--]->territory_value =
                            WHITE_T;
                }
                else if (gs->board.cell_array[row][col]->territory_value ==
                         WHITE_T)
                {
                    while (gs->count > 0)
                        gs->cells_scanned[gs->count--]->territory_value = NO_T;
                }
            }

            else if (gs->board.cell_array[row][col]->cell_value == WHITE)
            {
                gs->cells_scanned[++gs->count] = gs->board.cell_array[row][col];
                scan_group_for_liberties(gs, WHITE, row, col);

                if (gs->board.cell_array[row][col]->territory_value == NO_T)
                {
                    while (gs->count > 0)
                        gs->cells_scanned[gs->count--]->territory_value =
                            BLACK_T;
                }
                else if (gs->board.cell_array[row][col]->territory_value ==
                         BLACK_T)
                {
                    while (gs->count > 0)
                        gs->cells_scanned[gs->count--]->territory_value = NO_T;
                }
            }
            reset_scan_count_for_all_cells(gs);
            break;
        }

        col++;
        if (col > grid_size)
        {
            col = 0;
            row++;
        }
    }
}
void scan_empty_cells_for_ownership(GameState* gs, int row, int col)
{
    if (gs->board.cell_array[row - 1][col]->cell_value == EMPTY ||
        gs->board.cell_array[row - 1][col]->territory_value == BLACK_T ||
        gs->board.cell_array[row - 1][col]->territory_value == WHITE_T)
    {
        if (gs->board.cell_array[row - 1][col]->has_cell_been_scanned == NO)
        {
            gs->board.cell_array[row - 1][col]->has_cell_been_scanned = YES;
            gs->cells_scanned[++gs->count] = gs->board.cell_array[row - 1][col];
            scan_empty_cells_for_ownership(gs, row - 1, col);
        }
    }
    else if (gs->board.cell_array[row - 1][col]->cell_value == BLACK &&
             gs->board.cell_array[row - 1][col]->has_border_been_scanned == NO)
    {
        gs->board.cell_array[row - 1][col]->has_border_been_scanned = YES;
        ++gs->end_score.black_stones_next_to_empty_cells;
    }
    else if (gs->board.cell_array[row - 1][col]->cell_value == WHITE &&
             gs->board.cell_array[row - 1][col]->has_border_been_scanned == NO)
    {
        gs->board.cell_array[row - 1][col]->has_border_been_scanned = YES;
        ++gs->end_score.white_stones_next_to_empty_cells;
    }

    if (gs->board.cell_array[row][col + 1]->cell_value == EMPTY ||
        gs->board.cell_array[row][col + 1]->territory_value == BLACK_T ||
        gs->board.cell_array[row][col + 1]->territory_value == WHITE_T)

    {
        if (gs->board.cell_array[row][col + 1]->has_cell_been_scanned == NO)
        {
            gs->board.cell_array[row][col + 1]->has_cell_been_scanned = YES;
            gs->cells_scanned[++gs->count] = gs->board.cell_array[row][col + 1];
            scan_empty_cells_for_ownership(gs, row, col + 1);
        }
    }
    else if (gs->board.cell_array[row][col + 1]->cell_value == BLACK &&
             gs->board.cell_array[row][col + 1]->has_border_been_scanned == NO)
    {
        gs->board.cell_array[row][col + 1]->has_border_been_scanned = YES;
        ++gs->end_score.black_stones_next_to_empty_cells;
    }

    else if (gs->board.cell_array[row][col + 1]->cell_value == WHITE &&
             gs->board.cell_array[row][col + 1]->has_border_been_scanned == NO)
    {
        gs->board.cell_array[row][col + 1]->has_border_been_scanned = YES;
        ++gs->end_score.white_stones_next_to_empty_cells;
    }

    if (gs->board.cell_array[row + 1][col]->cell_value == EMPTY ||
        gs->board.cell_array[row + 1][col]->territory_value == BLACK_T ||
        gs->board.cell_array[row + 1][col]->territory_value == WHITE_T)

    {
        if (gs->board.cell_array[row + 1][col]->has_cell_been_scanned == NO)
        {
            gs->board.cell_array[row + 1][col]->has_cell_been_scanned = YES;
            gs->cells_scanned[++gs->count] = gs->board.cell_array[row + 1][col];
            scan_empty_cells_for_ownership(gs, row + 1, col);
        }
    }
    else if (gs->board.cell_array[row + 1][col]->cell_value == BLACK &&
             gs->board.cell_array[row + 1][col]->has_border_been_scanned == NO)
    {
        gs->board.cell_array[row + 1][col]->has_border_been_scanned = YES;
        ++gs->end_score.black_stones_next_to_empty_cells;
    }

    else if (gs->board.cell_array[row + 1][col]->cell_value == WHITE &&
             gs->board.cell_array[row + 1][col]->has_border_been_scanned == NO)
    {
        gs->board.cell_array[row + 1][col]->has_border_been_scanned = YES;
        ++gs->end_score.white_stones_next_to_empty_cells;
    }

    if (gs->board.cell_array[row][col - 1]->cell_value == EMPTY ||
        gs->board.cell_array[row][col - 1]->territory_value == BLACK_T ||
        gs->board.cell_array[row][col - 1]->territory_value == WHITE_T)

    {
        if (gs->board.cell_array[row][col - 1]->has_cell_been_scanned == NO)
        {
            gs->board.cell_array[row][col - 1]->has_cell_been_scanned = YES;
            gs->cells_scanned[++gs->count] = gs->board.cell_array[row][col - 1];
            scan_empty_cells_for_ownership(gs, row, col - 1);
        }
    }
    else if (gs->board.cell_array[row][col - 1]->cell_value == BLACK &&
             gs->board.cell_array[row][col - 1]->has_border_been_scanned == NO)
    {
        gs->board.cell_array[row][col - 1]->has_border_been_scanned = YES;
        ++gs->end_score.black_stones_next_to_empty_cells;
    }
    else if (gs->board.cell_array[row][col - 1]->cell_value == WHITE &&
             gs->board.cell_array[row][col - 1]->has_border_been_scanned == NO)
    {
        gs->board.cell_array[row][col - 1]->has_border_been_scanned = YES;
        ++gs->end_score.white_stones_next_to_empty_cells;
    }
}

void get_client(GameState* gs)
{
    while (1)
    {
        if ((gs->net.client = SDLNet_TCP_Accept(gs->net.server)))
        {
            if ((gs->net.remote_ip = SDLNet_TCP_GetPeerAddress(gs->net.client)))
            {
                gs->net.ipaddr = SDL_SwapBE32(gs->net.remote_ip->host);
                printf("Accepted a connection from %d.%d.%d.%d port %hu\n",
                       gs->net.ipaddr >> 24,
                       gs->net.ipaddr >> 16 & 0xff,
                       gs->net.ipaddr >> 8 & 0xff,
                       gs->net.ipaddr & 0xff,
                       gs->net.remote_ip->port);
                gs->net.got_client = 1;
                return;
            }
            else
                fprintf(stderr,
                        "SDLNet_TCP_GetPeerAddress: %s\n",
                        SDLNet_GetError());
        }
        SDL_Delay(100);
    }
}

/*
void host_receive_stone(GameState* gs)
{
    // if (!gs->net.got_client)
    //     wait_for_join(gs);
    int  len;
    char message[3];
    while (1)
    {
        if ((len = SDLNet_TCP_Recv(gs->net.client, message, 3)) > 0)
        {
            printf("Received: %s\n", message);
            place_on_pos(gs, message);

            if (strcmp(message, "exit") == 0)
            {
                SDLNet_TCP_Close(gs->net.client);
                SDLNet_TCP_Close(gs->net.server);
                printf("Terminate connection\n");
                exit(0);
            }
        }
        SDL_Delay(100);
    }
    return;
}

void join_send_stone(GameState* gs)
{
    while (1)
    {
        if (gs->to_be_placed != NULL)
        {
            int len = strlen(gs->to_be_placed->position_str);
            if (SDLNet_TCP_Send(gs->net.client,
                                (void*)gs->to_be_placed->position_str,
                                len) < len)
            {
                fprintf(stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
                exit(EXIT_FAILURE);
            }
            gs->to_be_placed = NULL;
        }
        SDL_Delay(100);
    }
    return;
}
*/

void place_on_pos(GameState* gs, const char* pos)
{
    int row       = 0;
    int col       = 0;
    int grid_size = gs->board.play_size + 1;

    while (row <= grid_size)
    {
        if (strcmp(gs->board.cell_array[row][col]->position_str, pos) == 0)
        {
            if (gs->board.cell_array[row][col]->cell_value == EMPTY)
            {
                if (gs->turn == BLACK)
                {
                    gs->board.cell_array[row][col]->cell_value = BLACK;
                    gs->stones_to_capture                      = NO;
                    // scans the enemy group (if there is one)
                    // for liberties directly above the placed black stone
                    init_scan_enemy(gs, WHITE, row - 1, col);
                    // scans the enemy group directly to the right of the placed black stone
                    init_scan_enemy(gs, WHITE, row, col + 1);
                    // scans the enemy group directly below the placed black stone
                    init_scan_enemy(gs, WHITE, row + 1, col);
                    // scans the enemy group directly to the left of the placed black stone
                    init_scan_enemy(gs, WHITE, row, col - 1);

                    // Check for Ko
                    if (gs->stones_to_capture == YES &&
                        gs->ko_rule_black == gs->board.cell_array[row][col])
                    {
                        printf("Rule: ko, also known as infinity - you cannot "
                               "place the stone in the same cell as your "
                               "previous move\n\n");
                        gs->board.cell_array[row][col]->cell_value = EMPTY;
                        while (gs->capcount > 0)
                            gs->stones_captured[gs->capcount--]
                                ->has_cell_been_scanned = NO;
                        return;
                    }

                    capture_stones(gs);

                    if (check_for_suicide(gs, BLACK, row, col) == OK)
                        gs->ko_rule_black = gs->board.cell_array[row][col];
                }

                else if (gs->turn == WHITE)
                {
                    gs->board.cell_array[row][col]->cell_value = WHITE;
                    gs->stones_to_capture                      = NO;

                    init_scan_enemy(gs, BLACK, row - 1, col);
                    init_scan_enemy(gs, BLACK, row, col + 1);
                    init_scan_enemy(gs, BLACK, row + 1, col);
                    init_scan_enemy(gs, BLACK, row, col - 1);

                    // Check for Ko
                    if (gs->stones_to_capture == YES &&
                        gs->ko_rule_white == gs->board.cell_array[row][col])
                    {
                        printf("Rule: ko, also known as infinity - you cannot "
                               "place the stone in the same cell as your "
                               "previous move\n\n");
                        gs->board.cell_array[row][col]->cell_value = EMPTY;
                        while (gs->capcount > 0)
                            gs->stones_captured[gs->capcount--]
                                ->has_cell_been_scanned = NO;
                        return;
                    }

                    capture_stones(gs);

                    if (check_for_suicide(gs, WHITE, row, col) == OK)
                        gs->ko_rule_white = gs->board.cell_array[row][col];
                }
            }
            printf("Placed stone on %s.\n", pos);
            break;
        }

        col++;
        if (col > grid_size)
        {
            col = 0;
            row++;
        }
    }
}

void send_or_receive_stones(GameState* gs)
{
    char message[3];
    while (1)
    {
        // receive stones
        if (gs->turn != gs->player_color)
            while (1)
            {
                if (SDLNet_TCP_Recv(gs->net.client, message, 3) > 0)
                {
                    printf("Received: %s\n", message);
                    place_on_pos(gs, message);
                    change_turn(gs);
                    // when message coordinates go to double digits (e.g A19), the next received stone (e.g A3) would be changed to A39, the next statement fixes this
                    message[2] = '\0';
                    break;
                }
                SDL_Delay(100);
            }
        // send stones
        else if (gs->turn == gs->player_color)
            while (1)
            {
                if (gs->to_be_placed != NULL)
                {
                    int len = strlen(gs->to_be_placed->position_str);
                    if (SDLNet_TCP_Send(gs->net.client,
                                        (void*)gs->to_be_placed->position_str,
                                        len) < len)
                    {
                        fprintf(
                            stderr, "SDLNet_TCP_Send: %s\n", SDLNet_GetError());
                        exit(EXIT_FAILURE);
                    }
                    gs->to_be_placed = NULL;
                    change_turn(gs);
                    break;
                }
                SDL_Delay(100);
            }
    }
    return;
}

void change_turn(GameState* gs)
{
    if (gs->turn == BLACK)
        gs->turn = WHITE;
    else if (gs->turn == WHITE)
        gs->turn = BLACK;
}