#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "ncsnake.h"
#include "game.h"

#define FPS 8

// Implementations
void die(char *msg, char *err)
{
    if (*err)
        fprintf(stderr, "ncsnake: %s: %s\n", msg, err);
    else
        fprintf(stderr, "ncsnake: %s\n", msg);
    cleanup();
    exit(EXIT_FAILURE);
}

void init()
{
    // Ncurses
    initscr();
    if (!has_colors()) {
        die("failed to initialize colors", "no terminal support");
    }
    start_color();
    noecho();
    keypad(stdscr, TRUE);

    // Initialize game stage grid and set it to empty
    if (gameStageCreate(&stage, LINES, COLS / 2)) {
        die("failed to create stage", "malloc failed");
    }
    gameStageSetDefault(&stage);

    // Initialize colors that represent various tiles
    init_pair(1 + GAME_TILE_EMPTY     , COLOR_BLACK, COLOR_BLACK   );
    init_pair(1 + GAME_TILE_WALL      , COLOR_BLACK, COLOR_WHITE   );
    init_pair(1 + GAME_TILE_SNAKE_HEAD, COLOR_BLACK, COLOR_YELLOW  );
    init_pair(1 + GAME_TILE_SNAKE_BODY, COLOR_BLACK, COLOR_GREEN   );
    init_pair(1 + GAME_TILE_FOOD      , COLOR_BLACK, COLOR_MAGENTA );
    init_pair(1 + GAME_TILE_UNKNOWN   , COLOR_BLACK, COLOR_RED     );

    // Initialize windows list
    windows = windowsInit();
}

void draw()
{
    for (int i = 0; i < stage.h; i++) {
        for (int j = 0; j < stage.w; j++) {
            attron(COLOR_PAIR(1 + stage.tile[i][j]));
            mvprintw(i, j * 2, "  ");
            attroff(COLOR_PAIR(1 + stage.tile[i][j]));
        }
    }
    refresh();
    if (windowsDraw(windows)) {
        die("windowsDraw error", "returned non-zero");
    }
    refresh();
}

void cleanup()
{
    windowsFree(windows);
    endwin();
}

void showMsg(char *msg)
{
    int y, x, w = 0, h, linec = 0;
    char token[] = "\n", *line, *lines;

    // Calculate window dimensions from text msg
    if (!(lines = (char *)malloc(sizeof(char) * strlen(msg)))) {
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

int main(int argc, char **argv)
{
    init();

    // Enter game loop
    running = 1;
    while(running) {
        refresh();
        draw();
        usleep(1000000 / FPS);
    }

    // Cleanup
    cleanup();

    return EXIT_SUCCESS;
}
