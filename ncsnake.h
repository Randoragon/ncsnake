#ifndef NCSNAKE_H
#define NCSNAKE_H

#include "windows.h"
#include "stage.h"

#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

// Variables
int running;
unsigned int term_h, term_w;
GameStage stage;
Windows *windows;

// Forward declarations
void die(char *msg, char *err);
void updateDims();
void init();
void draw();
void cleanup();
void showMsg(char *msg);
int  main(int argc, char **argv);

#endif
