#ifndef NCSNAKE_H
#define NCSNAKE_H

#include "windows.h"
#include "game.h"

#define MIN(x, y) (((x) > (y)) ? (y) : (x))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

#define FPS 60
#define KEYBUF_SIZE 20 /* see the listen() function implementation */

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

typedef enum {
    KEYBUF_KEY_NONE = 0,
    KEYBUF_KEY_UP,
    KEYBUF_KEY_LEFT,
    KEYBUF_KEY_DOWN,
    KEYBUF_KEY_RIGHT
} KeybufKey;

// Variables
int running, gameover, paused, speed, speedstep, foodcount;
KeybufKey keybuf[KEYBUF_SIZE];
unsigned long long tick;
GameStage layers[LAYER_COUNT];
Windows *windows;
Snake *snakes;

// Forward declarations
void timestamp(char *str);
void die(char *msg, char *err);
void warn(char *msg, char *err);
void debug(char *msg, char *err);
void init();
void listen();
void step();
void draw();
void clean();
void cleanup();
void showMsg(char *msg, void (*hook)());
void msghookUnpause();
void msghookGameOver();
int  isGameStep();
void keybufPush(int key);
KeybufKey keybufPop();
int  main(int argc, char **argv);

#endif
