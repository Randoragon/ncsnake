#include "windows.h"
#include <ncurses.h>
#include <malloc.h>

void windowsInit(Windows *windows)
{
    windows->win  = NULL;
    windows->next = NULL;
}

WINDOW *windowsLink(Windows *windows)
{
    WINDOW *win;
    if (!(win = (WINDOW *)malloc(sizeof(WINDOW)))) {
        return NULL;
    }

    if (!windows->win) {
        windows->win = win;
    } else {
        Windows *last, *new;
        for (last = windows; last->next; last = last->next);
        if (last) {
            if (!(new = (Windows *)malloc(sizeof(Windows)))) {
                return NULL;
            }
            last->next = new;
            new->win   = win;
            new->next  = NULL;
        }
    }

    return win;
}

void windowsUnlink(Windows *windows, WINDOW *win)
{
    Windows *prev = NULL, *found;
    for (found = windows; found->win != win; prev = found, found = found->next);
    if (found) {
        if (prev) {
            prev->next = found->next;
        } else {
            if (found->next) {
                windows->win = found->next->win;
                windows->next = found->next->next;
            } else {
                windows->win = NULL;
                windows->next = NULL;
            }
        }

        if (found->win) {
            free(found->win);
        }
        free(found);
    }
}

void windowsFree(Windows *windows)
{
    Windows *prev = NULL, *cur;
    for (cur = windows; prev || cur; prev = cur, cur = cur->next) {
        if (cur && cur->win) {
            free(cur->win);
        }
        if (prev) {
            free(prev);
        }
    }
}
