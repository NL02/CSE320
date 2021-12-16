#include <stdio.h>

#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "test_common.h"

#define STANDARD_LIMITS "ulimit -t 10; ulimit -f 2000"

Test(view_suite, normal_test, .init=setup_test_dir, .timeout=30) {
	char *name = "normal";
	sprintf(program_options, "tests/rsrc/view_dir/normal");
	int err = run_using_system(name, name, "", "", STANDARD_LIMITS);
	assert_normal_exit(err);
	assert_outfile_matches(name, NULL);
}

Test(view_suite, normal_valgrind_test, .init=setup_test_dir, .timeout=30) {
	char *name = "normal_valgrind";
	sprintf(program_options, "tests/rsrc/view_dir/normal");
	int err = run_using_system(name, name, "", "valgrind --leak-check=full --undef-value-errors=yes --error-exitcode=37", STANDARD_LIMITS);
	assert_no_valgrind_errors(err);
	assert_normal_exit(err);
	assert_outfile_matches(name, NULL);
}

Test(view_suite, long_line_test, .init=setup_test_dir, .timeout=30) {
	char *name = "long_line";
	sprintf(program_options, "tests/rsrc/view_dir/long_line");
	int err = run_using_system(name, name, "", "", STANDARD_LIMITS);
	assert_normal_exit(err);
	assert_outfile_matches(name, NULL);
}

Test(view_suite, long_line_valgrind_test, .init=setup_test_dir, .timeout=30) {
	char *name = "long_line_valgrind";
	sprintf(program_options, "tests/rsrc/view_dir/long_line");
	int err = run_using_system(name, name, "", "valgrind --leak-check=full --undef-value-errors=yes --error-exitcode=37", STANDARD_LIMITS);
	assert_no_valgrind_errors(err);
	assert_normal_exit(err);
	assert_outfile_matches(name, NULL);
}

Test(view_suite, multi_screen_test, .init=setup_test_dir, .timeout=30) {
	char *name = "multi_screen";
	sprintf(program_options, "tests/rsrc/view_dir/multi_screen");
	int err = run_using_system(name, name, "", "", STANDARD_LIMITS);
	assert_normal_exit(err);
	assert_outfile_matches(name, NULL);
}

Test(view_suite, multi_screen_valgrind_test, .init=setup_test_dir, .timeout=30) {
	char *name = "multi_screen_valgrind";
	sprintf(program_options, "tests/rsrc/view_dir/multi_screen");
	int err = run_using_system(name, name, "", "valgrind --leak-check=full --undef-value-errors=yes --error-exitcode=37", STANDARD_LIMITS);
	assert_no_valgrind_errors(err);
	assert_normal_exit(err);
	assert_outfile_matches(name, NULL);
}
