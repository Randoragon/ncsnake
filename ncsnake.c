#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ncsnake.h"
#include "game.h"

#define FPS 60
#define KEYBUF_SIZE 21 /* see the listen() function implementation */

// Implementations
void die(char *msg, char *err)
{
    if (*err)
        fprintf(stderr, "[err] ncsnake: %s: %s\n", msg, err);
    else
        fprintf(stderr, "[err] ncsnake: %s\n", msg);
    cleanup();
    exit(EXIT_FAILURE);
}

void warn(char *msg, char *err)
{
    if (*err)
        fprintf(stderr, "[warn] ncsnake: %s: %s\n", msg, err);
    else
        fprintf(stderr, "[warn] ncsnake: %s\n", msg);
}

void init()
{
    // Ncurses
    initscr();
    if (!has_colors()) {
        die("failed to initialize colors", "no terminal support");
    }
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    ESCDELAY = 0;

    // Initialize game stage grid and set it to empty
    for (int i = 0; i < LAYER_COUNT; i++) {
        if (gameStageCreate(&layers[i], LINES, COLS / 2)) {
            die("failed to create stage", "malloc failed");
        }
        gameStageFill(&layers[i], LINES, COLS / 2, 0, 0, GAME_TILE_EMPTY);
    }
    gameStageSetDefault(&layers[LAYER_WALL]);

    // Initialize colors that represent various tiles
    init_pair(1 + GAME_TILE_WALL      , COLOR_BLACK, COLOR_WHITE   );
    init_pair(1 + GAME_TILE_SNAKE_HEAD, COLOR_BLACK, COLOR_YELLOW  );
    init_pair(1 + GAME_TILE_SNAKE_BODY, COLOR_BLACK, COLOR_GREEN   );
    init_pair(1 + GAME_TILE_FOOD      , COLOR_BLACK, COLOR_MAGENTA );
    init_pair(1 + GAME_TILE_UNKNOWN   , COLOR_BLACK, COLOR_RED     );

    // Initialize windows list
    if (!(windows = windowsInit())) {
        die("failed to initialize windows", "malloc failed");
    }

    // Initialize snakes list
    if (!(snakes = snakesInit())) {
        die("failed to initialize snakes", "malloc failed");
    }

    // Add default player
    SnakeCoords coords[] = {
        { .y = 5, .x = 7 },
        { .y = 5, .x = 6 },
        { .y = 5, .x = 5 },
    };
    if (snakeCreate(snakes, coords, sizeof coords / sizeof coords[0], SNAKE_DIR_RIGHT)) {
        die("failed to create snake", "snakeCreate returned non-zero");
    }

    // Set game variables
    speed = 45;
    speedstep = FPS * 1; /* wait 1 second before starting the game */
    foodcount = 1;
    paused = 0;
}

void listen()
{
    /* Allow input buffering, but only for different consecutive presses!
     * The size-1 of the keyboard buffer determines how many keys can be pressed
     * in one game frame (FPS) before input delay kicks in.
     */
    int keybuf[KEYBUF_SIZE] = {0};
    int ch, i = 1;
    while ((ch = getch()) != ERR) {
        if (isKeyValid(ch) && keybuf[i-1] != ch) {
            keybuf[i++] = ch;
        }
        if (i >= KEYBUF_SIZE) {
            break;
        }
    }

    // Re-enqueue the captured keys
    for (i = KEYBUF_SIZE - 1; i-- > 0;) {
        if (keybuf[i])
            ungetch(keybuf[i]);
    }

    switch(getch()) {
        case KEY_UP: case 'k': case 'w':
            if (!paused)
                snakesTurn(snakes, SNAKE_DIR_UP);
            break;
        case KEY_LEFT: case 'h': case 'a':
            if (!paused)
                snakesTurn(snakes, SNAKE_DIR_LEFT);
            break;
        case KEY_DOWN: case 'j': case 's':
            if (!paused)
                snakesTurn(snakes, SNAKE_DIR_DOWN);
            break;
        case KEY_RIGHT: case 'l': case 'd':
            if (!paused)
                snakesTurn(snakes, SNAKE_DIR_RIGHT);
            break;
        case 'p':
            paused = !paused;
            break;
        case 'q':
            running = FALSE;
            break;
    }
}

void step()
{
    if (!paused && isGameStep()) {
        // Calculate new snake positions
        if (snakesAdvance(snakes)) {
            warn("snakesAdvance failed", "");
        }

        // Check for collisions
        if (snakesCollision(snakes, layers, LAYER_COUNT)) {
            printf("collision detected\n");
        }

        // Update snake positions
        if (snakesUpdate(snakes)) {
            die("snakesUpdate failed", "snakeSegCopy returned non-zero");
        }
    }
}

void draw()
{
    if (isGameStep) {
        clear();
        if (snakesDraw(snakes, &layers[LAYER_SNAKE])) {
            warn("snakesDraw failed", "out-of-bounds snake");
        }
        for (int i = 0; i < LAYER_COUNT; i++) {
            for (int j = 0; j < layers[i].h; j++) {
                for (int k = 0; k < layers[i].w; k++) {
                    if (layers[i].tile[j][k] != GAME_TILE_EMPTY) {
                        attron(COLOR_PAIR(1 + layers[i].tile[j][k]));
                        mvprintw(j, k * 2, "  ");
                        attroff(COLOR_PAIR(1 + layers[i].tile[j][k]));
                    }
                }
            }
        }
        refresh();
        if (windowsDraw(windows)) {
            die("windowsDraw error", "returned non-zero");
        }
        refresh();
    }
}

void clean()
{
    if (snakesUndraw(snakes, &layers[LAYER_SNAKE])) {
        warn("snakesUndraw failed", "out-of-bounds snake");
    }
}

void cleanup()
{
    windowsFree(windows);
    snakesFree(snakes);
    endwin();
}

void showMsg(char *msg)
{
    int y, x, w = 0, h, linec = 0;
    char token[] = "\n", *line, *lines;

    // Calculate window dimensions from text msg
    if (!(lines = (char *)malloc(sizeof(char) * (strlen(msg) + 1)))) {
        die("showMsg error", "malloc");
    }
    strcpy(lines, msg);
    line = strtok(lines, token);
    while (line) {
        w = MAX(w, strlen(line));
        line = strtok(NULL, token);
        linec++;
    }
    free(lines);
    w = MIN(COLS - 2, w + 8);
    h = MIN(LINES - 2, linec + 4);
    y = (LINES - h) / 2;
    x = (COLS - w) / 2;

    if (!windowsLink(windows, msg, h, w, y, x)) {
        die("showMsg error", "windowsLink returned non-zero");
    }
}

int isKeyValid(int ch)
{
    switch(ch) {
        case KEY_UP: case KEY_LEFT: case KEY_DOWN: case KEY_RIGHT:
        case 'k'   : case 'h'     : case 'j'     : case 'l'      :
        case 'w'   : case 'a'     : case 's'     : case 'd'      :
            return 1;
        case 'q': case 'p':
            return 2;
        default:
            return 0;
    }
}

int isGameStep()
{
    return (speedstep <= 0);
}

int main(int argc, char **argv)
{
    init();

    // Enter game loop
    running = 1;
    while(running) {
        speedstep = (paused ? 0 : speedstep - 1);

        listen();
        step();
        draw();
        clean();

        speedstep = (speedstep <= 0 ? FPS - speed + 1: speedstep);
        usleep(1000000 / FPS);
    }

    // Cleanup
    cleanup();

    return EXIT_SUCCESS;
}
