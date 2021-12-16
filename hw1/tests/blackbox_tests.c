#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "mtft.h"
#include "magic.h"

static char *progname = "bin/mtft";

Test(blackbox_suite, encode_functionality_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmd = "ulimit -t 5; ulimit -f 2000; bin/mtft -e < tests/rsrc/alphabet.txt > test_output/alphabet.out";
    char *cmp = "cmp test_output/alphabet.out tests/rsrc/alphabet.out";

    system("rm -f test_output/alphabet.out");
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with %d instead of EXIT_SUCCESS",
                 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(blackbox_suite, decode_functionality_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmd = "ulimit -t 5; ulimit -f 2000; bin/mtft -d < tests/rsrc/alphabet.out > test_output/alphabet.txt";
    char *cmp = "cmp test_output/alphabet.txt tests/rsrc/alphabet.txt";

    system("rm -f test_output/alphabet.txt");
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with %d instead of EXIT_SUCCESS",
                 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(blackbox_suite, encode_odd_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmd = "ulimit -t 5; ulimit -f 2000; bin/mtft -e -b 2 < tests/rsrc/loremipsum.txt > test_output/loremipsum.out";

    system("rm -f test_output/loremipsum.out");
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_FAILURE,
                 "Program exited with %d instead of EXIT_FAILURE",
                 return_code);
    exit(MAGIC_EXIT_CODE);
}

Test(blackbox_suite, encode_random_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmd = "ulimit -t 5; ulimit -f 3000; bin/mtft -e -b 2 < tests/rsrc/random > test_output/random_2byte.out";
    char *cmp = "cmp test_output/random_2byte.out tests/rsrc/random_2byte.out";

    system("rm -f test_output/random_2byte.out");
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with %d instead of EXIT_SUCCESS",
                 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}

Test(blackbox_suite, decode_random_2_byte_test, .timeout = 5, .exit_code = MAGIC_EXIT_CODE)
{
    char *cmd = "ulimit -t 5; ulimit -f 3000; bin/mtft -d -b 2 < tests/rsrc/random_2byte.out > test_output/random";
    char *cmp = "cmp test_output/random tests/rsrc/random";

    system("rm -f test_output/random");
    int return_code = WEXITSTATUS(system(cmd));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program exited with %d instead of EXIT_SUCCESS",
                 return_code);
    return_code = WEXITSTATUS(system(cmp));
    cr_assert_eq(return_code, EXIT_SUCCESS,
                 "Program output did not match reference output.");
    exit(MAGIC_EXIT_CODE);
}
