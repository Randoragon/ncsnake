#ifndef NCSNAKE_H
#define NCSNAKE_H

#include <ncurses.h>

// Variables
int running;
WINDOW *window;
unsigned int term_h, term_w;

// Forward declarations
void die(char *msg, char *err);
void updateDims();
void draw();

#endif
