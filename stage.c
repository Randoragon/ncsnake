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
    return 0;
}

void gameStageFill(GameStage *stage, unsigned int h, unsigned int w, GameField value)
{
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            stage->field[i][j] = value;
        }
    }
}

