#ifndef WINDOWS_H
#define WINDOWS_H

#include <ncurses.h>

typedef struct Windows {
    WINDOW *win;
    char *caption;
    void (*hook)();
    struct Windows *next;
} Windows;

Windows *windowsInit();
WINDOW  *windowsPush(Windows *windows, char *caption, int height, int width, int starty, int startx, void (*hook)());
void     windowsPop(Windows *windows, int runhook);
void     windowsFree(Windows *windows);
int      windowsDraw(Windows *windows);
int      windowsExist(Windows *windows);

#endif
