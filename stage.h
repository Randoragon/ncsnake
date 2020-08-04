#ifndef STAGE_H
#define STAGE_H

typedef enum {
    GAME_TILE_EMPTY = 0,
    GAME_TILE_WALL,
    GAME_TILE_SNAKE,
    GAME_TILE_FOOD,
    GAME_TILE_UNKNOWN
} GameTile;

typedef struct {
    GameTile **tile;
    unsigned int w, h;
} GameStage;

int gameStageCreate(GameStage *stage, unsigned int h, unsigned int w);
void gameStageFill(GameStage *stage, unsigned int h, unsigned int w, unsigned int y, unsigned int x, GameTile value);

#endif
