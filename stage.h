#ifndef STAGE_H
#define STAGE_H

typedef enum {
    GAME_FIELD_EMPTY = 0,
    GAME_FIELD_WALL,
    GAME_FIELD_SNAKE,
    GAME_FIELD_FOOD
} GameField;

typedef struct {
    GameField **field;
} GameStage;

int gameStageCreate(GameStage *stage, unsigned int h, unsigned int w);
void gameStageFill(GameStage *stage, unsigned int h, unsigned int w, GameField value);

#endif
