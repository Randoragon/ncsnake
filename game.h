#ifndef GAME_H
#define GAME_H

typedef enum {
    GAME_TILE_EMPTY = 0,
    GAME_TILE_WALL,
    GAME_TILE_SNAKE,
    GAME_TILE_FOOD,
    GAME_TILE_UNKNOWN
} GameTile;

typedef struct {
    GameTile **tile;
    unsigned int w, h;
} GameStage;

typedef enum {
    SNAKE_STATE_ALIVE,
    SNAKE_STATE_DEAD,
    SNAKE_STATE_ERROR
} SnakeState;

typedef enum {
    SNAKE_DIR_UP,
    SNAKE_DIR_LEFT,
    SNAKE_DIR_DOWN,
    SNAKE_DIR_RIGHT
} SnakeDir;

typedef struct Snake {
    unsigned int x, y;
    char *ch;
    int ishead;
    struct Snake *next;
} Snake;

SnakeState snakeMove(Snake *head, SnakeDir dir);
int gameStageCreate(GameStage *stage, unsigned int h, unsigned int w);
void gameStageFill(GameStage *stage, unsigned int h, unsigned int w, unsigned int y, unsigned int x, GameTile value);
void gameStageSetDefault(GameStage *stage);

#endif

