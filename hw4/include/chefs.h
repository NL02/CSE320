#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include "cookbook.h"

#ifndef CHEFS_H
#define CHEFS_H


int argumentParse(int argc, char *argv[], int *err, char **cookbook, char **mainRecipe, int *maxCooks);
int cook(COOKBOOK *cbp, char *mainRecipe, int maxCooks); 
#endif