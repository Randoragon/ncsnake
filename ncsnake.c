#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

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
    exit(EXIT_FAILURE);
}

void updateDims()
{
    getmaxyx(stdscr, term_h, term_w);
}

void draw()
{

}

void cleanup()
{
    endwin();
}

int main(int argc, char **argv)
{
    // Initialization
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    updateDims();
    GameStage stage;
    if (gameStageCreate(&stage, term_h, term_w)) {
        die("failed to create stage", "malloc failed");
    }
    gameStageFill(&stage, term_h, term_w, GAME_FIELD_EMPTY);

    // Enter game loop
    running = 1;
    while(running) {
        draw();

        refresh();
        usleep(1000000 / FPS);
    }

    // Cleanup
    cleanup();

    return EXIT_SUCCESS;
}
