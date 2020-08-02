#ifndef STAGE_H
#define STAGE_H

typedef enum {
    EMPTY = 0, WALL, SNAKE, FOOD
} GameField;

typedef GameField **GameStage;

int stageCreate(GameStage stage, unsigned int h, unsigned int w);
void stagePopulate(GameStage stage, unsigned int h, unsigned int w, GameField value);

#endif
