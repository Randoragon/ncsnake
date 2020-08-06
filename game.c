#include "game.h"
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#define GAME_STAGE_MIN_W 4
#define GAME_STAGE_MIN_H 4

Snake *snakesInit()
{
    Snake *ret;
    if (!(ret = (Snake *)malloc(sizeof(Snake)))) {
        return NULL;
    }
    ret->head = NULL;
    ret->next = NULL;
    return ret;
}

void snakesFree(Snake *snakes)
{
    Snake *s, *tmp;
    s = snakes;
    while (s) {
        if (s->head) {
            SnakeSegment *seg, *tmp;
            seg = s->head;
            while (seg) {
                tmp = seg;
                seg = seg->next;
                free(tmp);
            }
        }
        tmp = s;
        s = s->next;
        free(tmp);
    }
}

int snakeCreate(Snake *snakes, SnakeCoords *coords, size_t coordslen, SnakeDir dir)
{
    if (!snakes)
        return 1;

    // Create new snake struct
    Snake *snew;
    if (!(snew = (Snake *)malloc(sizeof(Snake)))) {
        return 2;
    }

    // Create snake segments inside the snake
    SnakeSegment *head = NULL, *new = NULL, *last;
    for (int i = 0; i < coordslen; i++) {
        last = new;
        if (!(new = (SnakeSegment *)malloc(sizeof(SnakeSegment)))) {
            free(snew);
            if (head) {
                snakeSegFree(head);
            }
            return 2;
        }
        if (last) {
            last->next = new;
        }
        new->x = coords[i].x;
        new->y = coords[i].y;
        head   = (!head ? new : head);
    }

    // Copy snake metadata
    snew->dir = dir;
    snew->state = SNAKE_STATE_ALIVE;
    snew->growth = 0;

    // Append new snake to the snakes list
    if (!snakes->head) {
        snakes->head = head;
    } else {
        Snake *slast;
        for (slast = snakes; slast && slast->next; slast = slast->next);
        slast->next = snew;
    }
    return 0;
}

Snake *snakeRemove(Snake *snakes, Snake *snake)
{
    if (snakes && snake) {
        if (snake == snakes) {
            /* Normally if the element to remove is the first one,
             * it's enough to simply move the pointer forward.
             * However, if there is only one element, don't free
             * it and instead reset its memory to NULL.
             */
            if (snakes->next) {
                snakes = snakes->next;
            } else {
                snakes->head = NULL;
            }
        } else {
            Snake *prev;
            for (prev = snake; prev && prev->next != snake; prev = prev->next);
            if (prev) {
                prev->next = snake->next;
            }
        }

        // Free snake segments memory
        snakeSegFree(snake->head);
    }
    return snakes;
}

int snakeGrow(Snake *snake, unsigned int y, unsigned int x)
{
    if (!snake || !snake->head)
        return 1;

    // Create new snake segment on given coords
    SnakeSegment *new, *last;
    if (!(new = (SnakeSegment *)malloc(sizeof(SnakeSegment)))) {
        return 2;
    }
    new->x = x;
    new->y = y;

    // Append the new segment to the snake
    for (last = snake->head; last && last->next; last = last->next);
    last->next = new;

    return 0;
}

void snakesTurn(Snake *snakes, SnakeDir dir)
{
    for (Snake *s = snakes; s; s = s->next) {
        switch(dir) {
            case SNAKE_DIR_UP:
                if (s->dir == SNAKE_DIR_DOWN)
                    return;
                break;
            case SNAKE_DIR_LEFT:
                if (s->dir == SNAKE_DIR_RIGHT)
                    return;
                break;
            case SNAKE_DIR_DOWN:
                if (s->dir == SNAKE_DIR_UP)
                    return;
                break;
            case SNAKE_DIR_RIGHT:
                if (s->dir == SNAKE_DIR_LEFT)
                    return;
                break;
            default:
                break;
        }
        s->dir = dir;
    }
}

int snakesAdvance(Snake *snakes)
{
    for (Snake *s = snakes; s; s = s->next) {
        if (s->head) {
            unsigned int nx, ny, lx, ly;

            // Get new position for head
            nx = s->head->x;
            ny = s->head->y;
            switch(s->dir) {
                case SNAKE_DIR_UP:
                    ny -= 1;
                    break;
                case SNAKE_DIR_LEFT:
                    nx -= 1;
                    break;
                case SNAKE_DIR_DOWN:
                    ny += 1;
                    break;
                case SNAKE_DIR_RIGHT:
                    nx += 1;
                    break;
                default:
                    return 1;
            }

            // Update all segments' positions and keep the order
            SnakeSegment *seg, *tmp;
            int ishead = 1;
            seg = s->head;
            while(seg) {
                if (ishead) {
                    lx = seg->x;
                    ly = seg->y;
                    seg->x = nx;
                    seg->y = ny;
                    ishead = 0;
                } else {
                    nx = lx;
                    ny = ly;
                    lx = seg->x;
                    ly = seg->y;
                    seg->x = nx;
                    seg->y = ny;
                }
                seg = seg->next;
            }

            // Append new segment in place of former last one if food was eaten
            if (s->growth) {
                snakeGrow(s, ly, lx);
            }
        }
    }
    return 0;
}

int snakesDraw(Snake *snakes, GameStage *stage)
{
    for (Snake *s = snakes; s; s = s->next) {
        int ishead = 1;
        for (SnakeSegment *seg = s->head; seg; seg = seg->next) {
            if (seg->y >= stage->h || seg->x >= stage->w) {
                return 1;
            }
            stage->tile[seg->y][seg->x] = (ishead ? GAME_TILE_SNAKE_HEAD : GAME_TILE_SNAKE_BODY);
            ishead = 0;
        }
    }
    return 0;
}

int snakesUndraw(Snake *snakes, GameStage *stage)
{
    for (Snake *s = snakes; s; s = s->next) {
        for (SnakeSegment *seg = s->head; seg; seg = seg->next) {
            if (seg->y >= stage->h || seg->x >= stage->w) {
                return 1;
            }
            stage->tile[seg->y][seg->x] = GAME_TILE_EMPTY;
        }
    }
    return 0;
}

void snakeSegFree(SnakeSegment *head)
{
    SnakeSegment *seg, *tmp;
    seg = head;
    while(seg) {
        tmp = seg;
        seg = seg->next;
        free(tmp);
    }
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

