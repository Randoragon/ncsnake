#ifndef NCSNAKE_H
#define NCSNAKE_H

#include <stdio.h>
#include <stdlib.h>

// Forward declarations
void die(char *msg, char *err);


// Implementations
void die(char *msg, char *err)
{
    if (*err)
        fprintf(stderr, "ncsnake: %s: %s\n", msg, err);
    else
        fprintf(stderr, "ncsnake: %s\n", msg);
    exit(EXIT_FAILURE);
}

#endif
