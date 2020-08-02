#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>

#define FPS 8

// Variables
int running;
WINDOW *window;
unsigned int term_h, term_w;

// Forward declarations
void die(char *msg, char *err);
void updateDims();
void draw();

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
    wborder(window, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(window);
}

int main(int argc, char **argv)
{
    // Initialization
    initscr();
    noecho();
    keypad(stdscr, TRUE);
    updateDims();
    if (!(window = newwin(term_h, term_w, 0, 0))) {
        die("failed to initialize window", "");
    }

    // Enter game loop
    running = 1;
    while(running) {
        draw();

        refresh();
        usleep(1000000 / FPS);
    }

    // Cleanup
    endwin();

    return EXIT_SUCCESS;
}
