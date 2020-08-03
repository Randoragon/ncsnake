#ifndef NCSNAKE_H
#define NCSNAKE_H

#include "stage.h"

// Variables
int running;
unsigned int term_h, term_w;
GameStage stage;

// Forward declarations
void die(char *msg, char *err);
void updateDims();
void init();
void draw();
void cleanup();
int  main(int argc, char **argv);

#endif
