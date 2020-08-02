#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#include "ncsnake.h"

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

int main(int argc, char **argv)
{
    // Initialization
    initscr();
    updateDims();

    // Enter game loop
    running = 1;
    while(running) {

        refresh();
        usleep(1000000 / FPS);
    }

    // Cleanup
    endwin();

    return EXIT_SUCCESS;
}
