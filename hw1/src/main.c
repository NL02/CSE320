#include <stdio.h>
#include <stdlib.h>

#include "mtft.h"
#include "debug.h"
// #include "mtft_utils.h"
static int input_code_1_1_byte[] = {354};
static int input_sym_1_1_byte[] = {97};
static int exp_code_1_1_byte[] = {353};
static int exp_sym_1_1_byte[] = {97};
#ifdef _STRING_H
#error "Do not #include <string.h>. You will get a ZERO."
#endif

#ifdef _STRINGS_H
#error "Do not #include <strings.h>. You will get a ZERO."
#endif

#ifdef _CTYPE_H
#error "Do not #include <ctype.h>. You will get a ZERO."
#endif

int main(int argc, char **argv)
{
    freopen("tests/rsrc/1_1.txt", "r", stdin);
    int size = sizeof(exp_code_1_1_byte) / sizeof(exp_code_1_1_byte[0]);
    global_options = 0x40000001;
    int sym, code;
    for (int i = 0; i < size; i++)
    {
        sym = input_sym_1_1_byte[i];
        code = mtf_map_encode(sym);
        (void) code;
        // assert_map_encode_val(code, exp_code_1_1_byte[i]);
    }
    // exit(MAGIC_EXIT_CODE);
    // if(validargs(argc, argv))
    //     USAGE(*argv, EXIT_FAILURE);
    // if(global_options & HELP_OPTION)
    //     USAGE(*argv, EXIT_SUCCESS);
    // // TO BE IMPLEMENTED
    // if((global_options == 0x40000001) || global_options == 0x40000002)
    // {
    //     return mtf_encode();
    // }
    // else if((global_options == 0x20000001) || global_options == 0x20000002)
    // {
    //     return mtf_decode();
    // }
    // return EXIT_FAILURE;
}

/*
 * Just a reminder: All non-main functions should
 * be in another file not named main.c
 */
