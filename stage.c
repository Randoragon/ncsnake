#include "stage.h"
#include <malloc.h>
#include <string.h>

int gameStageCreate(GameStage *stage, unsigned int h, unsigned int w)
{
    if (!(stage->tile = (GameTile **)malloc(sizeof(GameTile *) * h))) {
        return 1;
    }
    for (int i = 0; i < h; i++) {
        if (!(stage->tile[i] = (GameTile *)malloc(sizeof(GameTile) * w))) {
            return 1;
        }
    }
    stage->h = h;
    stage->w = w;
    return 0;
}

void gameStageFill(GameStage *stage, GameTile value)
{
    for (int i = 0; i < stage->h; i++) {
        for (int j = 0; j < stage->w; j++) {
            stage->tile[i][j] = value;
        }
    }
}

