#ifndef NCSNAKE_H
#define NCSNAKE_H

#include "windows.h"
#include "game.h"

#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

/* Order here goes from bottom to top.
 * Enumeration must start from 0 and
 * LAYER_COUNT must be last for it to
 * represent the total number of LAYERS.
 */
typedef enum {
    LAYER_WALL,
    LAYER_SNAKE,
    LAYER_COUNT
} Layer;

// Variables
int running, paused, speed, speedstep, foodcount;
unsigned long long tick;
GameStage layers[LAYER_COUNT];
Windows *windows;
Snake *snakes;

// Forward declarations
void timestamp(char *str);
void die(char *msg, char *err);
void warn(char *msg, char *err);
void init();
void listen();
void step();
void draw();
void clean();
void cleanup();
void showMsg(char *msg);
int  isKeyValid(int ch);
int  isGameStep();
int  main(int argc, char **argv);

#endif
