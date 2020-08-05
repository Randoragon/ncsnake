#ifndef NCSNAKE_H
#define NCSNAKE_H

#include "windows.h"
#include "game.h"

#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

// Variables
int running;
GameStage stage;
Windows *windows;
Snake *snakes;

// Forward declarations
void die(char *msg, char *err);
void init();
void draw();
void cleanup();
void showMsg(char *msg);
int  main(int argc, char **argv);

#endif
