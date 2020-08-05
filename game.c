#include "game.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#define GAME_STAGE_MIN_W 4
#define GAME_STAGE_MIN_H 4

Snake *snakeCreate(SnakeCoords *coords, size_t coordslen, SnakeDir dir)
{
    int ishead = 1;
    Snake *head = NULL, *new = NULL, *last;
    for (int i = 0; i < coordslen; i++) {
        last = new;
        if (!(new = (Snake *)malloc(sizeof(Snake)))) {
            if (head) {
                snakeFree(head);
            }
            return NULL;
        }
        if (last) {
            last->next = new;
        }
        new->x = coords[i].x;
        new->y = coords[i].y;
        new->dir = SNAKE_DIR_NONE;
        new->ishead = ishead;
        if (ishead) {
            new->dir = dir;
            head = new;
            ishead = 0;
        }
    }
    return head;
}

int snakeAppend(Snake *head, unsigned int y, unsigned int x)
{
    if (!head)
        return 1;

    Snake *new, *last;
    if (!(new = (Snake *)malloc(sizeof(Snake)))) {
        return 2;
    }
    new->x = x;
    new->y = y;
    new->dir = SNAKE_DIR_NONE;
    new->ishead = 0;

    for (last = head; last && last->next; last = last->next);
    last->next = new;

    return 0;
}

void snakeFree(Snake *head)
{
    Snake *seg, *tmp;
    seg = head;
    while (seg) {
        tmp = seg;
        seg = seg->next;
        free(tmp);
    }
}

Player *playersInit()
{
    Player *ret;
    if (!(ret = (Player *)malloc(sizeof(Player)))) {
        return NULL;
    }
    ret->head = NULL;
    ret->next = NULL;
    return ret;
}

int playersAdd(Player *players, Snake *head)
{
    if (!players)
        return 1;

    if (!players->head) {
        players->head = head;
    } else {
        Player *last, *new;
        for (last = players; last && last->next; last = last->next);
        if (!(new = (Player *)malloc(sizeof(Player)))) {
            return 1;
        }
        last->next = new;
        new->head  = head;
        new->next  = NULL;
    }
    return 0;
}

Player *playersRemove(Player *players, Player *player)
{
    if (players && player) {
        if (player == players) {
            players = player->next;
        } else {
            Player *prev;
            for (prev = players; prev && prev->next != player; prev = prev->next);
            if (prev) {
                prev->next = player->next;
            }
        }
        snakeFree(player->head);
    }
    return players;
}

void playersFree(Player *players)
{
    Player *p, *tmp;
    p = players;
    while (p) {
        if (p->head) {
            snakeFree(p->head);
        }
        tmp = p;
        p = p->next;
        free(tmp);
    }
}

int playersDraw(Player *players, GameStage *stage)
{
    for (Player *player = players; player; player = player->next) {
        int ishead = 1;
        for (Snake *seg = player->head; seg; seg = seg->next) {
            if (seg->y >= stage->h || seg->x >= stage->w) {
                return 1;
            }
            stage->tile[seg->y][seg->x] = (ishead ? GAME_TILE_SNAKE_HEAD : GAME_TILE_SNAKE_BODY);
            ishead = 0;
        }
    }
    return 0;
}

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

