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
    ret->head    = NULL;
    ret->newhead = NULL;
    ret->next    = NULL;
    return ret;
}

void snakesFree(Snake *snakes)
{
    Snake *s, *tmp;
    s = snakes;
    while (s) {
        snakeSegFree(s->head);
        snakeSegFree(s->newhead);
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

    // Create snake segments
    SnakeSegment *head, *newhead;
    if (!(head = snakeSegCreate(coords, coordslen))) {
        free(snew);
        return 2;
    }
    if (!(newhead = (SnakeSegment *)malloc(sizeof(SnakeSegment)))) {
        free(snew);
        if (head) {
            snakeSegFree(head);
        }
        return 2;
    }

    // Copy snake metadata
    snew->head    = head;
    snew->newhead = newhead;
    snew->dir     = dir;
    snew->state   = SNAKE_STATE_ALIVE;
    snew->growth  = 0;

    // Append new snake to the snakes list
    if (!snakes->head) {
        snakes->head    = snew->head;
        snakes->newhead = snew->newhead;
        snakes->dir     = snew->dir;
        snakes->state   = snew->state;
        snakes->growth  = snew->growth;
        free(snew);
    } else {
        Snake *slast;
        for (slast = snakes; slast && slast->next; slast = slast->next);
        slast->next = snew;
    }
    return 0;
}

SnakeSegment *snakeSegCreate(SnakeCoords *coords, size_t coordslen)
{
    SnakeSegment *head = NULL, *new = NULL, *last;
    for (int i = 0; i < coordslen; i++) {
        last = new;
        if (!(new = (SnakeSegment *)malloc(sizeof(SnakeSegment)))) {
            if (head) {
                snakeSegFree(head);
            }
            return NULL;
        }
        if (last) {
            last->next = new;
        }
        new->x = coords[i].x;
        new->y = coords[i].y;
        head   = (!head ? new : head);
    }
    return head;
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
        snakeSegFree(snake->newhead);
    }
    return snakes;
}

int snakeSegGrow(SnakeSegment *segments, unsigned int y, unsigned int x)
{
    if (!segments)
        return 1;

    // Create new snake segment on given coords
    SnakeSegment *new, *last;
    if (!(new = (SnakeSegment *)malloc(sizeof(SnakeSegment)))) {
        return 2;
    }
    new->x = x;
    new->y = y;

    // Append the new segment to the snake
    for (last = segments; last && last->next; last = last->next);
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

            // Deep copy position to newhead
            snakeSegCopy(s->newhead, s->head);

            // Calculate future position for head
            nx = s->newhead->x;
            ny = s->newhead->y;
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
            seg = s->newhead;
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
                snakeSegGrow(s->newhead, ly, lx);
            }
        }
    }
    return 0;
}

int snakesCollision(Snake *snakes, GameStage *stages, size_t stagec)
{
    for (Snake *s = snakes; s; s = s->next) {
        SnakeState state = s->state;
        for (int i = 0; i < stagec; i++) {
            int x, y;
            x = s->newhead->x;
            y = s->newhead->y;
            GameStage st = stages[i];
            if (x < 0 || x >= st.w || y < 0 || y >= st.h) {
                return 1;
            }
            switch(st.tile[y][x]) {
                case GAME_TILE_WALL: case GAME_TILE_SNAKE_HEAD: case GAME_TILE_SNAKE_BODY:
                    state = SNAKE_STATE_DEAD;
                    break;
                default:
                    break;
            }
        }
        s->state = state;
    }
    return 0;
}

int snakesUpdate(Snake *snakes)
{
    for (Snake *s = snakes; s; s = s->next) {
        if (s->state == SNAKE_STATE_ALIVE) {
            if (snakeSegCopy(s->head, s->newhead)) {
                return 1;
            }
        }
    }
    return 0;
}

int snakesDraw(Snake *snakes, GameStage *stage)
{
    gameStageFill(stage, stage->h, stage->w, 0, 0, GAME_TILE_EMPTY);
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

int snakeSegCopy(SnakeSegment *dest, SnakeSegment *src)
{
    // Force dest list to be the length of src
    size_t len1 = 0, len2 = 0;
    for (SnakeSegment *seg = src;  seg; seg = seg->next, len1++);
    for (SnakeSegment *seg = dest; seg; seg = seg->next, len2++);
    if (len1 > len2) {
        // Allocate memory on newhead to match that of head's
        SnakeCoords *coords;
        if (!(coords = (SnakeCoords *)malloc((len1 - len2) * sizeof(SnakeCoords)))) {
            return 2;
        }
        SnakeSegment *last;
        for (last = dest; last && last->next; last = last->next);
        if (!last) {
            free(coords);
            return 3;
        }
        if (!(last->next = snakeSegCreate(coords, len1 - len2))) {
            free(coords);
            return 2;
        }
        free(coords);
    } else if (len1 < len2) {
        /* This shouldn't be possible unless I add some gameplay mechanic
         * to shrink the snake, but handle it anyways. Deallocate surplus
         * newhead memory to match that of head's.
         */
        SnakeSegment *last;
        int i;
        for (i = len1, last = dest; i; i--, last = last->next);
        snakeSegFree(last);
    }
    SnakeSegment *seg1, *seg2;

    // Both lists are now equal in size, iterate and copy values one by one
    for (seg1 = src, seg2 = dest; seg1 && seg2; seg1 = seg1->next, seg2 = seg2->next) {
        seg2->x = seg1->x;
        seg2->y = seg1->y;
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

