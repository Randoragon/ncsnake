#ifndef STAGE_H
#define STAGE_H

typedef enum {
    GAME_FIELD_EMPTY = 0,
    GAME_FIELD_WALL,
    GAME_FIELD_SNAKE,
    GAME_FIELD_FOOD,
    GAME_FIELD_UNKNOWN
} GameField;

typedef struct {
    GameField **field;
    unsigned int w, h;
} GameStage;

int gameStageCreate(GameStage *stage, unsigned int h, unsigned int w);
void gameStageFill(GameStage *stage, GameField value);

#endif
