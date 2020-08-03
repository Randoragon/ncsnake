#include "stage.h"
#include <malloc.h>
#include <string.h>

int gameStageCreate(GameStage *stage, unsigned int h, unsigned int w)
{
    if (!(stage->field = (GameField **)malloc(sizeof(GameField *) * h))) {
        return 1;
    }
    for (int i = 0; i < h; i++) {
        if (!(stage->field[i] = (GameField *)malloc(sizeof(GameField) * w))) {
            return 1;
        }
    }
    stage->h = h;
    stage->w = w;
    return 0;
}

void gameStageFill(GameStage *stage, GameField value)
{
    for (int i = 0; i < stage->h; i++) {
        for (int j = 0; j < stage->w; j++) {
            stage->field[i][j] = value;
        }
    }
}

