#include "game.h"
#include <malloc.h>
#include <string.h>

#define GAME_STAGE_MIN_W 4
#define GAME_STAGE_MIN_H 4

int gameStageCreate(GameStage *stage, unsigned int h, unsigned int w)
{
    if (h < GAME_STAGE_MIN_H || w < GAME_STAGE_MIN_W) {
        return 2;
    }
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
    w = (x + w > stage->w ? stage->w - 1 - x : w);
    h = (y + h > stage->h ? stage->h - 1 - y : h);

    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            stage->tile[i][j] = value;
        }
    }
}

void gameStageSetDefault(GameStage *stage)
{
    unsigned int w, h;
    w = stage->w;
    h = stage->h;
    gameStageFill(stage, h, 1, 0, 0, GAME_TILE_WALL);
    gameStageFill(stage, h, 1, 0, w - 1, GAME_TILE_WALL);
    gameStageFill(stage, 1, w - 2, 0, 1, GAME_TILE_WALL);
    gameStageFill(stage, 1, w - 2, h - 1, 1, GAME_TILE_WALL);
    gameStageFill(stage, h - 2, w - 2, 1, 1, GAME_TILE_EMPTY);
}

