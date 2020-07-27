#ifndef WINDOWS_H
#define WINDOWS_H

#include <ncurses.h>

typedef struct Windows {
    WINDOW *win;
    struct Windows *next;
} Windows;

void    windowsInit(Windows *windows);
WINDOW *windowsLink(Windows *windows);
void    windowsUnlink(Windows *windows, WINDOW *win);
void    windowsFree(Windows *windows);

#endif
