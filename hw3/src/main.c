#include <stdio.h>
#include "sfmm.h"

int main(int argc, char const *argv[]) {
    double* ptr = sf_malloc(sizeof(double));
    double* ptra = sf_malloc(sizeof(double));
    // double* ptrb = sf_malloc(1);
    // (void)ptra;
    // (void)ptrb;
    *ptr = 320320320e-320;
    // *ptr = 10;
    sf_show_heap();
    printf("%f\n", *ptr);
    // sf_free(0);
    // sf_free(ptr);
    sf_free(ptra);

    return EXIT_SUCCESS;
}
