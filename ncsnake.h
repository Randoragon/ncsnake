#ifndef NCSNAKE_H
#define NCSNAKE_H

// Variables
int running;
unsigned int term_h, term_w;

// Forward declarations
void die(char *msg, char *err);
void updateDims();
void draw();
void cleanup();
int  main(int argc, char **argv);

#endif
