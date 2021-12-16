#include <stdio.h>

#include <criterion/criterion.h>
#include <criterion/logging.h>

#include "test_common.h"

#define STANDARD_LIMITS "ulimit -t 10; ulimit -f 2000"

Test(options_suite, sort_short_option_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "sort_short_option";
	sprintf(program_options, "-s name tests/rsrc/sort_dir/sort_lower_case");
	int err = run_using_system(name, test_name, "", "", STANDARD_LIMITS);
	assert_normal_exit(err);
	assert_outfile_matches(test_name, NULL);
}

Test(options_suite, sort_name_lower_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "sort_name_lower";
	sprintf(program_options, "--sort-key=name tests/rsrc/sort_dir/sort_lower_case");
	int err = run_using_system(name, test_name, "", "", STANDARD_LIMITS);
	assert_normal_exit(err);
	assert_outfile_matches(test_name, NULL);
}

Test(options_suite, sort_name_mixed_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "sort_name_mixed";
	sprintf(program_options, "--sort-key=name tests/rsrc/sort_dir/sort_mixed_case");
	int err = run_using_system(name, test_name, "", "", STANDARD_LIMITS);
	assert_normal_exit(err);
	assert_outfile_matches(test_name, NULL);
}

Test(options_suite, sort_size_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "sort_size";
	sprintf(program_options, "--sort-key=size tests/rsrc/sort_dir/sort_mixed_case/12345678");
	int err = run_using_system(name, test_name, "", "", STANDARD_LIMITS);
	assert_normal_exit(err);
	assert_outfile_matches(test_name, NULL);
}

Test(options_suite, sort_date_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "sort_date";
	sprintf(program_options, "--sort-key=date tests/rsrc/sort_dir/sort_mixed_case");
	int err = run_using_system(name, test_name, "", "", STANDARD_LIMITS);
	assert_normal_exit(err);
	assert_outfile_matches(test_name, NULL);
}

Test(options_suite, sort_none_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "sort_none";
	sprintf(program_options, "--sort-key=none tests/rsrc/sort_dir/sort_mixed_case");
	int err = run_using_system(name, test_name, "", "", STANDARD_LIMITS);
	assert_normal_exit(err);
	assert_outfile_matches(test_name, NULL);
}

Test(options_suite, sort_invalid_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "sort_invalid";
	sprintf(program_options, "-s invalid tests/rsrc/sort_dir/sort_mixed_case/human_readable");
	int err = run_using_system(name, test_name, "", "", STANDARD_LIMITS);
	assert_expected_status(EXIT_FAILURE, err);
}

Test(options_suite, sort_no_arg_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "sort_no_arg";
	sprintf(program_options, "-s tests/rsrc/sort_dir/sort_mixed_case/human_readable");
	int err = run_using_system(name, test_name, "", "", STANDARD_LIMITS);
	assert_expected_status(EXIT_FAILURE, err);
}

Test(options_suite, sort_prefix_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "sort_prefix";
	sprintf(program_options, "--sort=name tests/rsrc/sort_dir/sort_mixed_case/human_readable");
	int err = run_using_system(name, test_name, "", "", STANDARD_LIMITS);
	assert_normal_exit(err);
	assert_outfile_matches(test_name, NULL);
}

Test(options_suite, human_readable_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "human_readable";
	sprintf(program_options, "--human-readable tests/rsrc/sort_dir/sort_mixed_case/human_readable");
	int err = run_using_system(name, test_name, "", "", STANDARD_LIMITS);
	assert_normal_exit(err);
	assert_outfile_matches(test_name, NULL);
}

Test(options_suite, human_readable_short_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "human_readable_short";
	sprintf(program_options, "-h tests/rsrc/sort_dir/sort_mixed_case/human_readable");
	int err = run_using_system(name, test_name, "", "", STANDARD_LIMITS);
	assert_expected_status(EXIT_FAILURE, err);
}

Test(options_suite, human_readable_prefix_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "human_readable_prefix";
	sprintf(program_options, "--human tests/rsrc/sort_dir/sort_mixed_case/human_readable");
	int err = run_using_system(name, test_name, "", "", STANDARD_LIMITS);
	assert_normal_exit(err);
	assert_outfile_matches(test_name, NULL);
}

Test(options_suite, multi_options_size_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "multi_options_size";
	sprintf(program_options, "-s size --human-readable tests/rsrc/sort_dir/sort_mixed_case/human_readable");
	int err = run_using_system(name, test_name, "", "", STANDARD_LIMITS);
	assert_normal_exit(err);
	assert_outfile_matches(test_name, NULL);
}

Test(options_suite, valgrind_sort_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "valgrind_sort";
	sprintf(program_options, "--sort-key=name tests/rsrc/sort_dir/sort_lower_case");
	int err = run_using_system(name, test_name, "", "valgrind --leak-check=full --undef-value-errors=yes --error-exitcode=37", STANDARD_LIMITS);
	assert_no_valgrind_errors(err);
	assert_normal_exit(err);
	assert_outfile_matches(test_name, NULL);
}

Test(options_suite, valgrind_human_readable_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "valgrind_human_readable";
	sprintf(program_options, "--human-readable tests/rsrc/sort_dir/sort_mixed_case/human_readable");
	int err = run_using_system(name, test_name, "", "valgrind --leak-check=full --undef-value-errors=yes --error-exitcode=37", STANDARD_LIMITS);
	assert_no_valgrind_errors(err);
	assert_normal_exit(err);
	assert_outfile_matches(test_name, NULL);
}

Test(options_suite, valgrind_multi_options_test, .init=setup_test_dir, .timeout=30) {
	char *name = "simple_operations";
	char *test_name = "valgrind_multi_options";
	sprintf(program_options, "--sort-key=name --human-readable tests/rsrc/sort_dir/sort_mixed_case/human_readable");
	int err = run_using_system(name, test_name, "", "valgrind --leak-check=full --undef-value-errors=yes --error-exitcode=37", STANDARD_LIMITS);
	assert_no_valgrind_errors(err);
	assert_normal_exit(err);
	assert_outfile_matches(test_name, NULL);
}
