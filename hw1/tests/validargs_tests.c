#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "mtft.h"
#include "magic.h"

static char *progname = "bin/mtft";

Test(validargs_suite, encode_decode_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *argv[] = {progname, "-e", "-d", NULL};
    int argc = (sizeof(argv) / sizeof(char *)) - 1;
    int ret = validargs(argc, argv);
    int exp_ret = -1;
    cr_assert_eq(ret, exp_ret, "Invalid return for validargs.  Got: %d | Expected: %d",
                 ret, exp_ret);
    exit(MAGIC_EXIT_CODE);
}

Test(validargs_suite, bits_error_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *argv[] = {progname, "-e", "-b", "8", NULL};
    int argc = (sizeof(argv) / sizeof(char *)) - 1;
    int ret = validargs(argc, argv);
    int exp_ret = -1;
    cr_assert_eq(ret, exp_ret, "Invalid return for validargs.  Got: %d | Expected: %d",
                 ret, exp_ret);
    exit(MAGIC_EXIT_CODE);
}

Test(validargs_suite, bits_negative_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *argv[] = {progname, "-e", "-b", "-2", NULL};
    int argc = (sizeof(argv) / sizeof(char *)) - 1;
    int ret = validargs(argc, argv);
    int exp_ret = -1;
    cr_assert_eq(ret, exp_ret, "Invalid return for validargs.  Got: %d | Expected: %d",
                 ret, exp_ret);
    exit(MAGIC_EXIT_CODE);
}

Test(validargs_suite, invalid_args_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *argv[] = {progname, "-x", "2", NULL};
    int argc = (sizeof(argv) / sizeof(char *)) - 1;
    int ret = validargs(argc, argv);
    int exp_ret = -1;
    cr_assert_eq(ret, exp_ret, "Invalid return for validargs.  Got: %d | Expected: %d",
                 ret, exp_ret);
    exit(MAGIC_EXIT_CODE);
}

Test(validargs_suite, optional_before_positional_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *argv[] = {progname, "-b", "2", "-e", NULL};
    int argc = (sizeof(argv) / sizeof(char *)) - 1;
    int ret = validargs(argc, argv);
    int exp_ret = -1;
    cr_assert_eq(ret, exp_ret, "Invalid return for validargs.  Got: %d | Expected: %d",
                 ret, exp_ret);
    exit(MAGIC_EXIT_CODE);
}

Test(validargs_suite, ignore_args_after_help_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *argv[] = {progname, "-h", "-x", NULL};
    int argc = (sizeof(argv) / sizeof(char *)) - 1;
    int ret = validargs(argc, argv);
    int exp_ret = 0;
    int opt = global_options;
    int flag = 0x80000000;
    cr_assert_eq(ret, exp_ret, "Invalid return for validargs.  Got: %d | Expected: %d",
                 ret, exp_ret);
    cr_assert_eq(opt & flag, flag, "Correct bit (0x%x) not set for -h. Got: %x",
                 flag, opt);
    exit(MAGIC_EXIT_CODE);
}


Test(validargs_suite, bits_not_specified_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *argv[] = {progname, "-e", "-b", NULL};
    int argc = (sizeof(argv) / sizeof(char *)) - 1;
    int ret = validargs(argc, argv);
    int exp_ret = -1;
    cr_assert_eq(ret, exp_ret, "Invalid return for validargs.  Got: %d | Expected: %d",
                 ret, exp_ret);
    exit(MAGIC_EXIT_CODE);
}


Test(validargs_suite, bits_leading_zeros_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *argv[] = {progname, "-e", "-b","0001", NULL};
    int argc = (sizeof(argv) / sizeof(char *)) - 1;
    int ret = validargs(argc, argv);
    int exp_ret = 0;
    cr_assert_eq(ret, exp_ret, "Invalid return for validargs.  Got: %d | Expected: %d",
                 ret, exp_ret);
    exit(MAGIC_EXIT_CODE);
}

Test(validargs_suite, bits_multi_digit_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *argv[] = {progname,"-e", "-b", "12", NULL};
    int argc = (sizeof(argv) / sizeof(char *)) - 1;
    int ret = validargs(argc, argv);
    int exp_ret = -1;
    cr_assert_eq(ret, exp_ret, "Invalid return for validargs.  Got: %d | Expected: %d",
                 ret, exp_ret);
    exit(MAGIC_EXIT_CODE);
}

Test(validargs_suite, no_dashes_b_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *argv[] = {progname,"-e", "b", "1", NULL};
    int argc = (sizeof(argv) / sizeof(char *)) - 1;
    int ret = validargs(argc, argv);
    int exp_ret = -1;
    cr_assert_eq(ret, exp_ret, "Invalid return for validargs.  Got: %d | Expected: %d",
                 ret, exp_ret);
    exit(MAGIC_EXIT_CODE);
}

Test(validargs_suite, no_args_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *argv[] = {progname, NULL};
    int argc = (sizeof(argv) / sizeof(char *)) - 1;
    int ret = validargs(argc, argv);
    int exp_ret = -1;
    cr_assert_eq(ret, exp_ret, "Invalid return for validargs.  Got: %d | Expected: %d",
                 ret, exp_ret);
    exit(MAGIC_EXIT_CODE);
}
