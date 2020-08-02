#include "stage.h"
#include <malloc.h>
#include <string.h>

int stageCreate(GameStage stage, unsigned int h, unsigned int w)
{
    if (!(stage = (GameField **)malloc(sizeof(GameField *) * h))) {
        return 1;
    }
    for (int i = 0; i < h; i++) {
        if (!(stage[i] = (GameField *)malloc(sizeof(GameField) * w))) {
            return 1;
        }
    }
    return 0;
}

void stagePopulate(GameStage stage, unsigned int h, unsigned int w, GameField value)
{
    for (int i = 0; i < h; i++) {
        memset(stage[i], value, w);
    }
}

