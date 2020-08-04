#include "windows.h"
#include <ncurses.h>
#include <malloc.h>
#include <string.h>

Windows *windowsInit()
{
    Windows *windows;
    if (!(windows = (Windows *)malloc(sizeof(Windows)))) {
        return NULL;
    }
    windows->win  = NULL;
    windows->next = NULL;
    return windows;
}

WINDOW *windowsLink(Windows *windows, char *caption, int height, int width, int starty, int startx)
{
    if (!windows)
        return NULL;

    WINDOW  *win;
    Windows *last = NULL, *new;
    if (!(win = newwin(height, width, starty, startx))) {
        return NULL;
    }

    // Find the last list element to inject data after
    if (!windows->win) {
        // List is empty, use the head as new element
        new = windows;
    } else {
        // Create and append a new element at the end
        for (last = windows; last && last->next; last = last->next);
        if (!(new = (Windows *)malloc(sizeof(Windows)))) {
            delwin(win);
            return NULL;
        }
        last->next = new;
    }

    if (!(new->caption = (char *)malloc(sizeof(char) * (strlen(caption) + 1)))) {
        delwin(win);
        if (last) {
            free(new);
        }
        return NULL;
    }
    new->win   = win;
    strcpy(new->caption, caption);
    new->next  = NULL;

    return win;
}

void windowsUnlink(Windows *windows, WINDOW *win)
{
    Windows *prev = NULL, *found;
    for (found = windows; found && found->win != win; prev = found, found = found->next);
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
    Windows *cur, *next;
    cur = windows;
    while (cur) {
        if (cur->win) {
            delwin(cur->win);
            free(cur->caption);
        }
        next = cur->next;
        free(cur);
        cur = next;
    }
}

void windowsDraw(Windows *windows)
{
    if (!windows)
        return;

    for (Windows *w = windows; w; w = w->next) {
        if (w->win) {
            box(w->win, 0, 0);
            mvwprintw(w->win, 1, 1, "%s", w->caption);
            wrefresh(w->win);
        }
    }
}

