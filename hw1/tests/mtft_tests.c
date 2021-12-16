#include <criterion/criterion.h>
#include <criterion/logging.h>

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

#include "mtft.h"
#include "magic.h"
#include "mtft_utils.h"

static char *progname = "bin/mtft";

/* ------------------------------------------------------------------------- */
/* ------------------------- mtf_encode unit tests ------------------------- */
/* ------------------------------------------------------------------------- */

Test(mtft_encode_suite, 1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/coronavirus.out tests/rsrc/coronavirus.out";
    system("rm -f test_output/coronavirus.out");
    freopen("tests/rsrc/coronavirus.txt", "r", stdin);
    freopen("test_output/coronavirus.out", "w", stdout);
    global_options = 0x40000001;
    int ret = mtf_encode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_encode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
          "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_encode_suite, 2_byte_even_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/coronavirus_2byte.out tests/rsrc/coronavirus_2byte.out";
    system("rm -f test_output/coronavirus_2byte.out");
    freopen("tests/rsrc/coronavirus.txt", "r", stdin);
    freopen("test_output/coronavirus_2byte.out", "w", stdout);
    global_options = 0x40000002;
    int ret = mtf_encode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_encode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
          "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_encode_suite, empty_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/empty.out tests/rsrc/empty.out";
    system("rm -f test_output/empty.out");
    freopen("tests/rsrc/empty.txt", "r", stdin);
    freopen("test_output/empty.out", "w", stdout);
    global_options = 0x40000001;
    int ret = mtf_encode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_encode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
      "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_encode_suite, empty_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/empty.out tests/rsrc/empty.out";
    system("rm -f test_output/empty.out");
    freopen("tests/rsrc/empty.txt", "r", stdin);
    freopen("test_output/empty.out", "w", stdout);
    global_options = 0x40000002;
    int ret = mtf_encode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_encode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
          "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_encode_suite, binary_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/binary.out tests/rsrc/binary.out";
    system("rm -f test_output/binary.out");
    freopen("tests/rsrc/binary", "r", stdin);
    freopen("test_output/binary.out", "w", stdout);
    global_options = 0x40000001;
    int ret = mtf_encode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_encode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
          "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_encode_suite, random_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/random.out tests/rsrc/random.out";
    system("rm -f test_output/random.out");
    freopen("tests/rsrc/random", "r", stdin);
    freopen("test_output/random.out", "w", stdout);
    global_options = 0x40000001;
    int ret = mtf_encode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_encode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
          "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_encode_suite, all_ones_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/all_ones.out tests/rsrc/all_ones.out";
    system("rm -f test_output/all_ones.out");
    freopen("tests/rsrc/all_ones", "r", stdin);
    freopen("test_output/all_ones.out", "w", stdout);
    global_options = 0x40000001;
    int ret = mtf_encode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_encode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
          "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

/* ------------------------------------------------------------------------- */
/* ----------------------- mtf_map_encode unit tests ----------------------- */
/* ------------------------------------------------------------------------- */

Test(mtft_map_encode_suite, functionality_1_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/1_1.txt", "r", stdin);
    int size = sizeof(exp_code_1_1_byte) / sizeof(exp_code_1_1_byte[0]);
    global_options = 0x40000001;
    int sym, code;
    for (int i = 0; i < size; i++)
    {
        sym = input_sym_1_1_byte[i];
        code = mtf_map_encode(sym);
        assert_map_encode_val(code, exp_code_1_1_byte[i]);
    }
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_encode_suite, functionality_2_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/2_1.txt", "r", stdin);
    int size = sizeof(exp_code_2_1_byte) / sizeof(exp_code_2_1_byte[0]);
    global_options = 0x40000001;
    int sym, code;
    for (int i = 0; i < size; i++)
    {
        sym = input_sym_2_1_byte[i];
        code = mtf_map_encode(sym);
        assert_map_encode_val(code, exp_code_2_1_byte[i]);
    }
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_encode_suite, functionality_2_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/2_2.txt", "r", stdin);
    int size = sizeof(exp_code_2_2_byte) / sizeof(exp_code_2_2_byte[0]);
    global_options = 0x40000002;
    int sym, code;
    for (int i = 0; i < size; i++)
    {
        sym = input_sym_2_2_byte[i];
        code = mtf_map_encode(sym);
        assert_map_encode_val(code, exp_code_2_2_byte[i]);
    }
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_encode_suite, functionality_4_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/4_2.txt", "r", stdin);
    int size = sizeof(exp_code_4_2_byte) / sizeof(exp_code_4_2_byte[0]);
    global_options = 0x40000002;
    int sym, code;
    for (int i = 0; i < size; i++)
    {
        sym = input_sym_4_2_byte[i];
        code = mtf_map_encode(sym);
        assert_map_encode_val(code, exp_code_4_2_byte[i]);
    }
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_encode_suite, functionality_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/coronavirus.txt", "r", stdin);
    int size = sizeof(exp_code_coronavirus_1_byte) / sizeof(exp_code_coronavirus_1_byte[0]);
    global_options = 0x40000001;
    int sym, code;
    for (int i = 0; i < size; i++)
    {
        sym = input_sym_coronavirus_1_byte[i];
        code = mtf_map_encode(sym);
        assert_map_encode_val(code, exp_code_coronavirus_1_byte[i]);
    }
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_encode_suite, functionality_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/coronavirus.txt", "r", stdin);
    int size = sizeof(exp_code_coronavirus_2_byte) / sizeof(exp_code_coronavirus_2_byte[0]);
    global_options = 0x40000002;
    int sym, code;
    for (int i = 0; i < size; i++)
    {
        sym = input_sym_coronavirus_2_byte[i];
        code = mtf_map_encode(sym);
        assert_map_encode_val(code, exp_code_coronavirus_2_byte[i]);
    }
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_encode_suite, tree_1_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/1_1.txt", "r", stdin);
    int size = sizeof(exp_code_1_1_byte) / sizeof(exp_code_1_1_byte[0]);
    global_options = 0x40000001;
    int sym, code;
    for (int i = 0; i < size; i++)
    {
        sym = input_sym_1_1_byte[i];
        code = mtf_map_encode(sym);
        assert_map_encode_val(code, exp_code_1_1_byte[i]);
    }
    assert_number_of_tree_nodes(1, 3);
    assert_tree_height(1, 2);
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_encode_suite, tree_2_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/2_1.txt", "r", stdin);
    int size = sizeof(exp_code_2_1_byte) / sizeof(exp_code_2_1_byte[0]);
    global_options = 0x40000001;
    int sym, code;
    for (int i = 0; i < size; i++)
    {
        sym = input_sym_2_1_byte[i];
        code = mtf_map_encode(sym);
        assert_map_encode_val(code, exp_code_2_1_byte[i]);
    }
    assert_number_of_tree_nodes(1, 4);
    assert_tree_height(1, 3);
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_encode_suite, tree_2_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/2_2.txt", "r", stdin);
    int size = sizeof(exp_code_2_2_byte) / sizeof(exp_code_2_2_byte[0]);
    global_options = 0x40000002;
    int sym, code;
    for (int i = 0; i < size; i++)
    {
        sym = input_sym_2_2_byte[i];
        code = mtf_map_encode(sym);
        assert_map_encode_val(code, exp_code_2_2_byte[i]);
    }
    assert_number_of_tree_nodes(1, 3);
    assert_tree_height(1, 2);
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_encode_suite, tree_4_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/4_2.txt", "r", stdin);
    int size = sizeof(exp_code_4_2_byte) / sizeof(exp_code_4_2_byte[0]);
    global_options = 0x40000002;
    int sym, code;
    for (int i = 0; i < size; i++)
    {
        sym = input_sym_4_2_byte[i];
        code = mtf_map_encode(sym);
        assert_map_encode_val(code, exp_code_4_2_byte[i]);
    }
    assert_number_of_tree_nodes(1, 4);
    assert_tree_height(1, 3);
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_encode_suite, tree_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/coronavirus.txt", "r", stdin);
    int size = sizeof(exp_code_coronavirus_1_byte) / sizeof(exp_code_coronavirus_1_byte[0]);
    global_options = 0x40000001;
    int sym, code;
    for (int i = 0; i < size; i++)
    {
        sym = input_sym_coronavirus_1_byte[i];
        code = mtf_map_encode(sym);
        assert_map_encode_val(code, exp_code_coronavirus_1_byte[i]);
    }
    assert_number_of_tree_nodes(123, 123 + 10);
    assert_tree_height(16, 16);
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_encode_suite, tree_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/coronavirus.txt", "r", stdin);
    int size = sizeof(exp_code_coronavirus_2_byte) / sizeof(exp_code_coronavirus_2_byte[0]);
    global_options = 0x40000002;
    int sym, code;
    for (int i = 0; i < size; i++)
    {
        sym = input_sym_coronavirus_2_byte[i];
        code = mtf_map_encode(sym);
        assert_map_encode_val(code, exp_code_coronavirus_2_byte[i]);
    }
    assert_number_of_tree_nodes(207, 207 + 10);
    assert_tree_height(15, 15);
    exit(MAGIC_EXIT_CODE);
}

/* ------------------------------------------------------------------------- */
/* ------------------------- mtf_decode unit tests ------------------------- */
/* ------------------------------------------------------------------------- */

Test(mtft_decode_suite, 1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/coronavirus.txt tests/rsrc/coronavirus.txt";
    system("rm -f test_output/coronavirus.txt");
    freopen("tests/rsrc/coronavirus.out", "r", stdin);
    freopen("test_output/coronavirus.txt", "w", stdout);
    global_options = 0x20000001;
    int ret = mtf_decode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_decode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
          "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_decode_suite, 2_byte_even_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/coronavirus.txt tests/rsrc/coronavirus.txt";
    system("rm -f test_output/coronavirus.txt");
    freopen("tests/rsrc/coronavirus_2byte.out", "r", stdin);
    freopen("test_output/coronavirus.txt", "w", stdout);
    global_options = 0x20000002;
    int ret = mtf_decode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_decode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
          "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_decode_suite, empty_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/empty.txt tests/rsrc/empty.txt";
    system("rm -f test_output/empty.txt");
    freopen("tests/rsrc/empty.out", "r", stdin);
    freopen("test_output/empty.txt", "w", stdout);
    global_options = 0x20000002;
    int ret = mtf_decode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_decode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
      "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_decode_suite, empty_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/empty.txt tests/rsrc/empty.txt";
    system("rm -f test_output/empty.txt");
    freopen("tests/rsrc/empty.out", "r", stdin);
    freopen("test_output/empty.txt", "w", stdout);
    global_options = 0x20000002;
    int ret = mtf_decode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_decode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
          "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_decode_suite, binary_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/binary tests/rsrc/binary";
    system("rm -f test_output/binary");
    freopen("tests/rsrc/binary.out", "r", stdin);
    freopen("test_output/binary", "w", stdout);
    global_options = 0x20000001;
    int ret = mtf_decode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_decode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
          "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_decode_suite, random_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/random tests/rsrc/random";
    system("rm -f test_output/random");
    freopen("tests/rsrc/random.out", "r", stdin);
    freopen("test_output/random", "w", stdout);
    global_options = 0x20000001;
    int ret = mtf_decode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_decode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
          "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_decode_suite, all_ones_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmp = "cmp test_output/all_ones tests/rsrc/all_ones";
    system("rm -f test_output/all_ones");
    freopen("tests/rsrc/all_ones.out", "r", stdin);
    freopen("test_output/all_ones", "w", stdout);
    global_options = 0x20000001;
    int ret = mtf_decode();
    fflush(stdout);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for mtf_decode.  Got: %d | Expected: %d",
              ret, exp_ret);
    ret = WEXITSTATUS(system(cmp));
    cr_assert_eq(ret, EXIT_SUCCESS,
          "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

/* ------------------------------------------------------------------------- */
/* ----------------------- mtf_map_decode unit tests ----------------------- */
/* ------------------------------------------------------------------------- */

Test(mtft_map_decode_suite, functionality_1_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/1_1.out", "r", stdin);
    int size = sizeof(exp_sym_1_1_byte) / sizeof(exp_sym_1_1_byte[0]);
    global_options = 0x20000001;
    int code;
    for (int i = 0; i < size; i++)
    {
        code = input_code_1_1_byte[i];
        int sym = mtf_map_decode(code - 1);
        assert_map_decode_val(sym, exp_sym_1_1_byte[i]);
    }
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_decode_suite, functionality_2_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/2_1.out", "r", stdin);
    int size = sizeof(exp_sym_2_1_byte) / sizeof(exp_sym_2_1_byte[0]);
    global_options = 0x20000001;
    int code;
    for (int i = 0; i < size; i++)
    {
        code = input_code_2_1_byte[i];
        int sym = mtf_map_decode(code - 1);
        assert_map_decode_val(sym, exp_sym_2_1_byte[i]);
    }
    exit(MAGIC_EXIT_CODE);
 }

Test(mtft_map_decode_suite, functionality_2_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/2_2.out", "r", stdin);
    int size = sizeof(exp_sym_2_2_byte) / sizeof(exp_sym_2_2_byte[0]);
    global_options = 0x20000002;
    int code;
    for (int i = 0; i < size; i++)
    {
        code = input_code_2_2_byte[i];
        int sym = mtf_map_decode(code - 1);
        assert_map_decode_val(sym, exp_sym_2_2_byte[i]);
    }
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_decode_suite, functionality_4_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/4_2.out", "r", stdin);
    int size = sizeof(exp_sym_4_2_byte) / sizeof(exp_sym_4_2_byte[0]);
    global_options = 0x20000002;
    int code;
    for (int i = 0; i < size; i++)
    {
        code = input_code_4_2_byte[i];
        int sym = mtf_map_decode(code - 1);
        assert_map_decode_val(sym, exp_sym_4_2_byte[i]);
    }
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_decode_suite, functionality_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/coronavirus.out", "r", stdin);
    int size = sizeof(exp_sym_coronavirus_1_byte) / sizeof(exp_sym_coronavirus_1_byte[0]);
    global_options = 0x20000001;
    int code;
    for (int i = 0; i < size; i++)
    {
        code = input_code_coronavirus_1_byte[i];
        int sym = mtf_map_decode(code - 1);
        assert_map_decode_val(sym, exp_sym_coronavirus_1_byte[i]);
    }
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_decode_suite, functionality_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/coronavirus_2byte.out", "r", stdin);
    int size = sizeof(exp_sym_coronavirus_2_byte) / sizeof(exp_sym_coronavirus_2_byte[0]);
    global_options = 0x20000002;
    int code, i = 0;
    for (int i = 0; i < size; i++)
    {
        code = input_code_coronavirus_2_byte[i];
        int sym = mtf_map_decode(code - 1);
        assert_map_decode_val(sym, exp_sym_coronavirus_2_byte[i]);
    }
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_decode_suite, tree_1_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/1_1.out", "r", stdin);
    int size = sizeof(exp_sym_1_1_byte) / sizeof(exp_sym_1_1_byte[0]);
    global_options = 0x20000001;
    int code;
    for (int i = 0; i < size; i++)
    {
        code = input_code_1_1_byte[i];
        int sym = mtf_map_decode(code - 1);
        assert_map_decode_val(sym, exp_sym_1_1_byte[i]);
    }
    assert_number_of_tree_nodes(1, 3);
    assert_tree_height(1, 2);
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_decode_suite, tree_2_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/2_1.out", "r", stdin);
    int size = sizeof(exp_sym_2_1_byte) / sizeof(exp_sym_2_1_byte[0]);
    global_options = 0x20000001;
    int code;
    for (int i = 0; i < size; i++)
    {
        code = input_code_2_1_byte[i];
        int sym = mtf_map_decode(code - 1);
        assert_map_decode_val(sym, exp_sym_2_1_byte[i]);
    }
    assert_number_of_tree_nodes(1, 4);
    assert_tree_height(1, 3);
    exit(MAGIC_EXIT_CODE);
 }

Test(mtft_map_decode_suite, tree_2_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/2_2.out", "r", stdin);
    int size = sizeof(exp_sym_2_2_byte) / sizeof(exp_sym_2_2_byte[0]);
    global_options = 0x20000002;
    int code;
    for (int i = 0; i < size; i++)
    {
        code = input_code_2_2_byte[i];
        int sym = mtf_map_decode(code - 1);
        assert_map_decode_val(sym, exp_sym_2_2_byte[i]);
    }
    assert_number_of_tree_nodes(1, 3);
    assert_tree_height(1, 2);
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_decode_suite, tree_4_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/4_2.out", "r", stdin);
    int size = sizeof(exp_sym_4_2_byte) / sizeof(exp_sym_4_2_byte[0]);
    global_options = 0x20000002;
    int code;
    for (int i = 0; i < size; i++)
    {
        code = input_code_4_2_byte[i];
        int sym = mtf_map_decode(code - 1);
        assert_map_decode_val(sym, exp_sym_4_2_byte[i]);
    }
    assert_number_of_tree_nodes(1, 4);
    assert_tree_height(1, 3);
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_decode_suite, tree_1_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/coronavirus.out", "r", stdin);
    int size = sizeof(exp_sym_coronavirus_1_byte) / sizeof(exp_sym_coronavirus_1_byte[0]);
    global_options = 0x20000001;
    int code;
    for (int i = 0; i < size; i++)
    {
        code = input_code_coronavirus_1_byte[i];
        int sym = mtf_map_decode(code - 1);
        assert_map_decode_val(sym, exp_sym_coronavirus_1_byte[i]);
    }
    assert_number_of_tree_nodes(123, 123 + 10);
    assert_tree_height(16, 16);
    exit(MAGIC_EXIT_CODE);
}

Test(mtft_map_decode_suite, tree_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    freopen("tests/rsrc/coronavirus_2byte.out", "r", stdin);
    int size = sizeof(exp_sym_coronavirus_2_byte) / sizeof(exp_sym_coronavirus_2_byte[0]);
    global_options = 0x20000002;
    int code;
    for (int i = 0; i < size; i++)
    {
        code = input_code_coronavirus_2_byte[i];
        int sym = mtf_map_decode(code - 1);
        assert_map_decode_val(sym, exp_sym_coronavirus_2_byte[i]);
    }
    assert_number_of_tree_nodes(207, 207 + 10);
    assert_tree_height(15, 15);
    exit(MAGIC_EXIT_CODE);
}
