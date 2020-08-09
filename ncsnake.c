#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "ncsnake.h"
#include "game.h"

// Implementations
void timestamp(char *str)
{
    time_t lt;
    struct tm *t;
    lt = time(NULL);
    t = localtime(&lt);
    sprintf(str, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
}

void die(char *msg, char *err)
{
    cleanup();
    char stamp[10];
    timestamp(stamp);
    if (*err)
        fprintf(stderr, "[%s %llu] [err] ncsnake: %s: %s\n", stamp, tick, msg, err);
    else
        fprintf(stderr, "[%s %llu] [err] ncsnake: %s\n", stamp, tick, msg);
    exit(EXIT_FAILURE);
}

void warn(char *msg, char *err)
{
    def_prog_mode();
    endwin();
    char stamp[10];
    timestamp(stamp);
    if (*err)
        fprintf(stderr, "[%s %llu] [warn] ncsnake: %s: %s\n", stamp, tick, msg, err);
    else
        fprintf(stderr, "[%s %llu] [warn] ncsnake: %s\n", stamp, tick, msg);
    reset_prog_mode();
    refresh();
}

void debug(char *msg, char *err)
{
#ifdef DEBUG
    def_prog_mode();
    endwin();
    char stamp[10];
    timestamp(stamp);
    if (*err)
        fprintf(stderr, "[%s %llu] [debug] ncsnake: %s: %s\n", stamp, tick, msg, err);
    else
        fprintf(stderr, "[%s %llu] [debug] ncsnake: %s\n", stamp, tick, msg);
    reset_prog_mode();
    refresh();
#endif
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
    paused = FALSE;
    memset(&keybuf, 0, KEYBUF_SIZE * sizeof(keybuf[0]));
    gameover = FALSE;
}

void listen()
{
    int ch;
    while ((ch = getch()) != ERR) {
        switch(ch) {
            case 'q':
                running = FALSE;
                break;
            case 'p':
                if (!gameover) {
                    if (paused) {
                        windowsPop(windows, 1);
                    } else {
                        paused = TRUE;
                        showMsg("PAUSE", msghookUnpause);
                        draw();
                    }
                }
                break;
            case '\n':
                windowsPop(windows, 1);
                break;
            default:
                /* Allow input buffering, but only for different consecutive presses!
                 * KEYBUF_SIZE determines how many keys can be queued up simultaneously,
                 * meaning if you press a combination of snake movements very quickly,
                 * up to KEYBUF_SIZE keypresses will be remembered and executed one
                 * by one in the following game steps. If buffer fills completely, your
                 * key presses will not be registered until some space has been cleared.
                 * keybuf shrinks by 1 keypress every game step (unless the buffer is empty).
                 */
                keybufPush(ch);
                break;
        }
    }
}

void step()
{
    if (!paused && !windowsExist(windows)) {
        // Turn the snakes if appropriate
        switch(keybufPop()) {
            case KEYBUF_KEY_UP:
                if (!paused)
                    snakesTurn(snakes, SNAKE_DIR_UP);
                break;
            case KEYBUF_KEY_LEFT:
                if (!paused)
                    snakesTurn(snakes, SNAKE_DIR_LEFT);
                break;
            case KEYBUF_KEY_DOWN:
                if (!paused)
                    snakesTurn(snakes, SNAKE_DIR_DOWN);
                break;
            case KEYBUF_KEY_RIGHT:
                if (!paused)
                    snakesTurn(snakes, SNAKE_DIR_RIGHT);
                break;
            case KEYBUF_KEY_NONE:
                break;
        }

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

        // If any snakes are dead, it's game over
        Snake *s;
        for (s = snakes; s && s->state != SNAKE_STATE_DEAD; s = s->next);
        if (s) {
            gameover = TRUE;
            showMsg("GAME OVER", msghookGameOver);
        }
    }
}

void draw()
{
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
}

void cleanup()
{
    windowsFree(windows);
    snakesFree(snakes);
    endwin();
}

void showMsg(char *msg, void (*hook)())
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

    if (!windowsPush(windows, msg, h, w, y, x, hook)) {
        die("showMsg error", "windowsPush returned non-zero");
    }
}

int isGameStep()
{
    return (speedstep <= 0);
}

void keybufPush(int key)
{
    int i;
    for (i = 0; i < KEYBUF_SIZE && keybuf[i] != KEYBUF_KEY_NONE; i++);
    if (i < KEYBUF_SIZE) {
        // Convert key code to KeybufKey
        KeybufKey k;
        switch (key) {
            case KEY_UP: case 'k': case 'w': 
                k = (paused ? KEYBUF_KEY_NONE : KEYBUF_KEY_UP);
                break;
            case KEY_LEFT: case 'h': case 'a': 
                k = (paused ? KEYBUF_KEY_NONE : KEYBUF_KEY_LEFT);
                break;
            case KEY_DOWN: case 'j': case 's': 
                k = (paused ? KEYBUF_KEY_NONE : KEYBUF_KEY_DOWN);
                break;
            case KEY_RIGHT: case 'l': case 'd':
                k = (paused ? KEYBUF_KEY_NONE : KEYBUF_KEY_RIGHT);
                break;
            default:
                k = KEYBUF_KEY_NONE;
                break;
        }

        // Filter out superfluous keys
        int flags;
        flags = (k == KEYBUF_KEY_NONE) |
                (i && keybuf[i - 1] == k) |
                (i && keybuf[i - 1] == KEYBUF_KEY_UP    && k == KEYBUF_KEY_DOWN ) |
                (i && keybuf[i - 1] == KEYBUF_KEY_LEFT  && k == KEYBUF_KEY_RIGHT) |
                (i && keybuf[i - 1] == KEYBUF_KEY_DOWN  && k == KEYBUF_KEY_UP   ) |
                (i && keybuf[i - 1] == KEYBUF_KEY_RIGHT && k == KEYBUF_KEY_LEFT );
        if (flags)
            return;

        keybuf[i] = k;
    } else {
        debug("keybufPush", "input buffer is full");
    }
}

KeybufKey keybufPop()
{
    KeybufKey ret = keybuf[0];
    memcpy(keybuf, keybuf + 1, (KEYBUF_SIZE - 1) * sizeof(keybuf[0]));
    keybuf[KEYBUF_SIZE - 1] = KEYBUF_KEY_NONE;
    return ret;
}

int main(int argc, char **argv)
{
    tick = 1;
    init();

    // Countdown
    draw();
    while (speedstep--) {
        usleep(1000000 / FPS);
        tick++;
    }

    // Enter game loop
    running = TRUE;
    while(running) {
        speedstep -= (paused == FALSE);

        listen();
        if (isGameStep() && !paused) {
            step();
            draw();
        }

        speedstep = (speedstep <= 0 ? FPS - speed + 1: speedstep);
        usleep(1000000 / FPS);
        tick++;
    }

    // Cleanup
    cleanup();

    return EXIT_SUCCESS;
}

void msghookUnpause()
{
    paused = FALSE;
    draw();
}

void msghookGameOver()
{
    running = FALSE;
}
