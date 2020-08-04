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

void gameStageFill(GameStage *stage, unsigned int h, unsigned int w, unsigned int y, unsigned int x, GameTile value)
{
    // Make sure coordinates are within bounds
    if (x >= stage->w || y >= stage->h)
        return;
    w = (x + w >= stage->w ? stage->w - x - 1 : w);
    h = (y + h >= stage->h ? stage->h - y - 1 : h);

    for (int i = y; i < h; i++) {
        for (int j = x; j < w; j++) {
            stage->tile[i][j] = value;
        }
    }
}

