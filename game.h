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
    /* To make things simple for collision checking,
     * the following steps are taken on each game step:
     *      1) calculate new snake positions
     *      2) resolve resulting collisions (if any)
     *      3) actually move the snakes (and draw)
     * This system removes the need to do complicated
     * and possibly buggy checking WITHOUT having moved
     * the snakes. The downside is, we need to store
     * two snake positions in every snake, hence the
     * two SnakeSegment linked lists (*head is the
     * real-time position, *newhead is the future
     * position used for collision checks).
     */
    SnakeSegment *head, *newhead;
    SnakeDir dir;
    SnakeState state;
    int growth;
    Snake *next;
} Snake;

// Functions
Snake *snakesInit();
void  snakesFree(Snake *snakes);
int   snakeCreate(Snake *snakes, SnakeCoords *coords, size_t coordslen, SnakeDir dir);
SnakeSegment *snakeSegCreate(SnakeCoords *coords, size_t coordslen);
Snake *snakeRemove(Snake *snakes, Snake *snake);
void  snakesTurn(Snake *snakes, SnakeDir dir);
int   snakesAdvance(Snake *snakes);
int   snakesCollision(Snake *snakes, GameStage *stages, size_t stagec);
int   snakesUpdate(Snake *snakes);
int   snakesDraw(Snake *snake, GameStage *stage);
int   snakeSegGrow(SnakeSegment *segments, unsigned int y, unsigned int x);
void  snakeSegFree(SnakeSegment *head);
int   snakeSegCopy(SnakeSegment *dest, SnakeSegment *src);
int  gameStageCreate(GameStage *stage, unsigned int h, unsigned int w);
void gameStageFill(GameStage *stage, unsigned int h, unsigned int w, unsigned int y, unsigned int x, GameTile value);
void gameStageSetDefault(GameStage *stage);
int  coordsEmpty(GameStage *stages, size_t stagec, Snake *snakes, unsigned int y, unsigned int x);
void spawnFood(GameStage *stages, size_t stagec, Snake *snakes, GameStage *target);

#endif
