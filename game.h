#ifndef GAME_H
#define GAME_H

#include <stdlib.h>

// Constants
typedef enum {
    GAME_TILE_EMPTY = 0,
    GAME_TILE_WALL,
    GAME_TILE_SNAKE_HEAD,
    GAME_TILE_SNAKE_BODY,
    GAME_TILE_FOOD,
    GAME_TILE_UNKNOWN
} GameTile;

typedef enum {
    SNAKE_STATE_ALIVE,
    SNAKE_STATE_DEAD,
    SNAKE_STATE_ERROR
} SnakeState;

typedef enum {
    SNAKE_DIR_UP,
    SNAKE_DIR_LEFT,
    SNAKE_DIR_DOWN,
    SNAKE_DIR_RIGHT,
    SNAKE_DIR_NONE
} SnakeDir;

// Structs
typedef struct GameStage GameStage;
typedef struct SnakeSegment SnakeSegment;
typedef struct SnakeCoords SnakeCoords;
typedef struct Snake Snake;

typedef struct GameStage {
    GameTile **tile;
    unsigned int w, h;
} GameStage;

typedef struct SnakeCoords {
    unsigned int x, y;
} SnakeCoords;

typedef struct SnakeSegment {
    unsigned int x, y;
    SnakeSegment *next;
} SnakeSegment;

typedef struct Snake {
    SnakeSegment *head;
    SnakeDir dir;
    SnakeState state;
    int growth;
    Snake *next;
} Snake;

// Functions
Snake *snakesInit();
void  snakesFree(Snake *snakes);
int   snakeCreate(Snake *snakes, SnakeCoords *coords, size_t coordslen, SnakeDir dir);
Snake *snakeRemove(Snake *snakes, Snake *snake);
int   snakeGrow(Snake *snake, unsigned int y, unsigned int x);
void  snakesTurn(Snake *snakes, SnakeDir dir);
int   snakesAdvance(Snake *snakes);
int   snakesDraw(Snake *snake, GameStage *stage);
int  snakesUndraw(Snake *snakes, GameStage *stage);
void  snakeSegFree(SnakeSegment *head);
int  gameStageCreate(GameStage *stage, unsigned int h, unsigned int w);
void gameStageFill(GameStage *stage, unsigned int h, unsigned int w, unsigned int y, unsigned int x, GameTile value);
void gameStageSetDefault(GameStage *stage);

#endif
