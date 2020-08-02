#ifndef SNAKE_H
#define SNAKE_H

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
    struct Snake *next;
} Snake;

SnakeState snakeMove(Snake *head, SnakeDir dir);

#endif
