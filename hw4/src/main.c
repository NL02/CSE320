#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#include "cookbook.h"
#include "csapp.h"
#include "chefs.h"

int main(int argc, char *argv[]) {

    // Beginning of my code 
    int err = 0; // used for parsing file and checking whether mainRecipe is modified
    char *cookbook = "cookbook.ckb"; // default value
    char *mainRecipe = "first"; // default value, if the name of the main recipe is omitted we assume first recipe is main recipe
    int maxCooks = 1; // default value of 1 max cook

    // Arugment Parser
    if (argumentParse(argc, argv, &err, &cookbook, &mainRecipe, &maxCooks) != 0)
    {  
        exit(1);
    }
    COOKBOOK *cbp;
    FILE *in;
    if((in = fopen(cookbook, "r")) == NULL) 
    {
        fprintf(stderr, "Can't open cookbook '%s': %s\n", cookbook, strerror(errno));
        exit(1);
    }
    cbp = parse_cookbook(in, &err);
    if(err) 
    {
        fprintf(stderr, "Error parsing cookbook '%s'\n", cookbook);
        exit(1);
    }
    if (cook(cbp, mainRecipe, maxCooks) != 0)
    {
        fprintf(stderr, "Error cooking cookbook\n");
        exit(1);
    }
    exit(0);
}
