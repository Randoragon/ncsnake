#ifndef GAME_H
#define GAME_H

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
typedef struct Snake Snake;
typedef struct SnakeCoords SnakeCoords;
typedef struct Player Player;

typedef struct GameStage {
    GameTile **tile;
    unsigned int w, h;
} GameStage;

typedef struct Snake {
    unsigned int x, y;
    char *ch;
    int ishead;
    SnakeDir dir;
    Snake *next;
} Snake;

typedef struct SnakeCoords {
    unsigned int x, y;
} SnakeCoords;

typedef struct Player {
    Snake *head;
    Player *next;
} Player;

// Functions
Snake *snakeCreate(SnakeCoords *coords, SnakeDir dir);
int snakeAppend(Snake *head, unsigned int y, unsigned int x);
void snakeFree(Snake *head);
SnakeState snakeMove(Snake *head);
Player *playersInit();
int  playersAdd(Player *players, Snake *head);
Player *playersRemove(Player *players, Player *player);
void playersFree(Player *players);
int  playersSpawn(Player *players, GameStage *stage);
int  gameStageCreate(GameStage *stage, unsigned int h, unsigned int w);
void gameStageFill(GameStage *stage, unsigned int h, unsigned int w, unsigned int y, unsigned int x, GameTile value);
void gameStageSetDefault(GameStage *stage);

#endif
