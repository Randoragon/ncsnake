#ifndef WINDOWS_H
#define WINDOWS_H

#include <ncurses.h>

typedef struct Windows {
    WINDOW *win;
    char *caption;
    struct Windows *next;
} Windows;

Windows *windowsInit();
WINDOW  *windowsLink(Windows *windows, char *caption, int height, int width, int starty, int startx);
void     windowsUnlink(Windows *windows, WINDOW *win);
void     windowsFree(Windows *windows);
void     windowsDraw(Windows *windows);

#endif
