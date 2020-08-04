#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#include "ncsnake.h"
#include "snake.h"
#include "stage.h"

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

void updateDims()
{
    getmaxyx(stdscr, term_h, term_w);
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

    // Get terminal size in rows and cols
    updateDims();

    // Initialize game stage grid and set it to empty
    if (gameStageCreate(&stage, term_h, term_w / 2)) {
        die("failed to create stage", "malloc failed");
    }
    gameStageFill(&stage, GAME_TILE_EMPTY);

    // Initialize colors that represent various tiles
    init_pair(1 + GAME_TILE_EMPTY  , COLOR_BLACK, COLOR_BLACK );
    init_pair(1 + GAME_TILE_WALL   , COLOR_BLACK, COLOR_WHITE );
    init_pair(1 + GAME_TILE_SNAKE  , COLOR_BLACK, COLOR_YELLOW);
    init_pair(1 + GAME_TILE_FOOD   , COLOR_BLACK, COLOR_GREEN );
    init_pair(1 + GAME_TILE_UNKNOWN, COLOR_BLACK, COLOR_RED   );

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
    windowsDraw(windows);
    refresh();
}

void cleanup()
{
    windowsFree(windows);
    endwin();
}

void showMsg(char *msg)
{
    int y, x, w, h;
    w = MIN(term_w - 2, strlen(msg) + 2);
    h = 3;
    y = (term_h - h) / 2;
    x = (term_w - w) / 2;
    if (!windowsLink(windows, msg, h, w, y, x)) {
        die("showMsg error", "windowsLink returned non-zero");
    }
}

int main(int argc, char **argv)
{
    init();

    showMsg("work in progress");

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
