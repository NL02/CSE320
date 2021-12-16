#include "test_common.h"

char program_options[500];
char test_infile[100];
char test_output_subdir[100];
char test_log_outfile[100];

/*
 * The following test setup ensures (hopefully) that the contents
 * of tests/rsrc/test_dir has a standard content, which matches
 * the contents of the reference output for the tests.
 */
void setup_test_dir(void) {
	system("rm -rf tests/rsrc/test_dir");
	system("rm -rf tests/rsrc/sort_dir");
	system("rm -rf tests/rsrc/view_dir");
	system("tar -xzf tests/rsrc/test_dir.tgz -C tests/rsrc");
	system("tar -xzf tests/rsrc/sort_dir.tgz -C tests/rsrc");
	system("tar -xzf tests/rsrc/view_dir.tgz -C tests/rsrc");
}

/*
 * Sets up to run a test.
 * Initialize various filenames, using the name of the test as a base,
 * and then initialize and run a command to remove old output from this test
 * and to make sure that the test output directory exists.
 */
int setup_test(char *name, char *test_name)
{
	char cmd[1000];
	sprintf(test_infile, "%s/%s", TEST_REF_DIR, name);
	sprintf(test_log_outfile, "%s/%s/%s", TEST_OUTPUT_DIR, test_name, test_name);
	sprintf(test_output_subdir, "%s/%s", TEST_OUTPUT_DIR, test_name);
	sprintf(cmd, "rm -f %s/%s.out %s/%s.err %s.out %s.err; rm -fr %s; mkdir -p %s",
		TEST_REF_DIR, test_name, TEST_REF_DIR, test_name, test_log_outfile, test_log_outfile,
		test_output_subdir, test_output_subdir);
	fprintf(stderr, "setup(%s)\n", cmd);
	return system(cmd);
}

/*
 * Run the program as a "black box" using system().
 * A shell command is constructed and run that first performs test setup,
 * then runs the program to be tested with input redirected from a test input
 * file and standard and error output redirected to separate output files.
 */
int run_using_system(char *name, char *test_name, char *pre_cmd, char *valgrind_cmd, char *limits)
{
	char cmd[1000];
	setup_test(name, test_name);
	sprintf(cmd, "%s;%s%s "REF_PROGNAME" %s < %s.in > %s/%s.out 2> %s/%s.err",
		limits, pre_cmd,
		valgrind_cmd, program_options, test_infile, TEST_REF_DIR, test_name,
		TEST_REF_DIR, test_name);
	fprintf(stderr, "run(%s)\n", cmd);
	system(cmd);
	sprintf(cmd, "%s;%s%s "PROGNAME" %s < %s.in > %s.out 2> %s.err",
		limits, pre_cmd,
		valgrind_cmd, program_options, test_infile, test_log_outfile,
		test_log_outfile);
	fprintf(stderr, "run(%s)\n", cmd);
	return system(cmd);
}

void assert_normal_exit(int status)
{
        cr_assert(!WIFSIGNALED(status),
                  "The program terminated with an unexpected signal (%d).\n",
		  WTERMSIG(status));
	cr_assert_eq(status, 0,
		     "The program did not exit normally (status = 0x%x).\n",
		     status);
}

void assert_expected_status(int expected, int status)
{
        cr_assert(!WIFSIGNALED(status),
                  "The program terminated with an unexpected signal (%d).\n",
		  WTERMSIG(status));
	cr_assert_eq(
	    WEXITSTATUS(status), expected,
	    "The program did not exit with the expected status "
	    "(expected 0x%x, was 0x%x).\n",
	    expected, WEXITSTATUS(status));
}

void assert_signaled(int sig, int status)
{
        cr_assert(WIFSIGNALED(status),
                  "The program did not terminate with a signal.\n");
	cr_assert(WTERMSIG(status) == sig,
		  "The program did not terminate with the expected signal "
		  "(expected %d, was %d).\n",
		  sig, WTERMSIG(status));
}

/*
 * Compare the standard output from the program being tested with reference
 * output, after first possibly using "grep" to remove lines that match a filter
 * pattern.
 */
void assert_outfile_matches(char *name, char *filter)
{
	char cmd[500];
	if (filter) {
		sprintf(cmd,
			"grep -v '%s' %s.out > %s_A.out; grep -v '%s' "
			"%s/%s.out > %s_B.out; "
			"diff --ignore-tab-expansion --ignore-trailing-space "
			"--ignore-space-change --ignore-blank-lines %s_A.out "
			"%s_B.out",
			filter, test_log_outfile, name, filter, TEST_REF_DIR,
			name, name, name, name);
	} else {
		sprintf(cmd,
			"diff --ignore-tab-expansion --ignore-trailing-space "
			"--ignore-space-change --ignore-blank-lines %s.out "
			"%s/%s.out",
			test_log_outfile, TEST_REF_DIR, name);
	}
	fprintf(stderr, "diff(%s)\n", cmd);
	int err = system(cmd);
	cr_assert_eq(err, 0,
		     "The output was not what was expected (diff exited with "
		     "status %d).\n",
		     WEXITSTATUS(err));
}

/*
 * Compare the standard error output from the program being tested with
 * reference output, after first possibly using "grep" to remove lines that
 * match a filter pattern.
 */
void assert_errfile_matches(char *name, char *filter)
{
	char cmd[500];
	if (filter) {
		sprintf(cmd,
			"grep -v '%s' %s.err > %s_A.err; grep -v '%s' "
			"%s/%s.err > %s_B.err; "
			"diff ---ignore-tab-expansion --ignore-trailing-space "
			"--ignore-space-change --ignore-blank-lines %s_A.err "
			"%s_B.err",
			filter, test_log_outfile, name, filter, TEST_REF_DIR,
			name, name, name, name);
	} else {
		sprintf(cmd,
			"diff --ignore-tab-expansion --ignore-trailing-space "
			"--ignore-space-change --ignore-blank-lines %s.err "
			"%s/%s.err",
			test_log_outfile, TEST_REF_DIR, name);
	}
	int err = system(cmd);
	cr_assert_eq(err, 0,
		     "The output was not what was expected (diff exited with "
		     "status %d).\n",
		     WEXITSTATUS(err));
}

void assert_no_valgrind_errors(int status)
{
	cr_assert_neq(WEXITSTATUS(status), 37,
		      "Valgrind reported errors -- see %s.err",
		      test_log_outfile);
}
