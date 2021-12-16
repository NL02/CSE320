# Homework 2 Debugging and Fixing - CSE 320 - Fall 2021
#### Professor Eugene Stark

### **Due Date: Friday 10/1/2021 @ 11:59pm**

# Introduction

In this assignment you are tasked with updating an old piece of
software, making sure it compiles, and that it works properly
in your VM environment.

Maintaining old code is a chore and an often hated part of software
engineering. It is definitely one of the aspects which are seldom
discussed or thought about by aspiring computer science students.
However, it is prevalent throughout industry and a worthwhile skill to
learn.  Of course, this homework will not give you a remotely
realistic experience in maintaining legacy code or code left behind by
previous engineers but it still provides a small taste of what the
experience may be like.  You are to take on the role of an engineer
whose supervisor has asked you to correct all the errors in the
program, plus add additional functionality.

By completing this homework you should become more familiar
with the C programming language and develop an understanding of:

- How to use tools such as `gdb` and `valgrind` for debugging C code.
- Modifying existing C code.
- C memory management and pointers.
- Working with files and the C standard I/O library.

## The Existing Program

Your goal will be to debug and extend an old program called `browse`,
which I wrote in 1993 as an example for use in a C programming class.
The version I am handing out is very close to the original version,
except that I have made a few changes for this assignment.
First of all, I rearranged the source tree and re-wrote the `Makefile`
to conform to what we are using for the other assignments in this course.
Then, I corrected one or two anachronisms that I did not want you to have
to worry about.
Finally, I introduced a few bugs here and there to make things more
interesting and educational for you :wink:.
Aside from these changes and the introduced bugs, which only involve a few
lines, the code is identical to the original, functioning version.

The `browse` program is a simple filesystem browser program.  Rather than
providing a graphical user interface, which is what such a program might
typically do today, the program assumes that the display is a "smart terminal",
which is a character-oriented display that understands commands for placing
characters at arbitrarily specified row and column positions, as well as
cursor-control commands for moving a cursor around on the screen.
To control the terminal, the `browse` program uses a library called `ncurses`.
This library has evolved over many years from the original `curses` library
that was part of Unix System V.  It is currently a part of essentially every
Linux and Unix distribution, and it is used to support the behavior of
terminal-based programs like `less` and `top`.

  > :nerd: Documentation for the `ncurses` API can be found online
  > [here](https://invisible-island.net/ncurses/man/ncurses.3x.html).

In its original form, the `browse` program takes one optional command-line argument,
which is the pathname of the directory to browse.  When the program starts, it
clears the screen and displays at the top a single `ls`-style line of information
about the specified directory (or the current directory, if no argument was given).
It then enters a command loop in which it will respond to single-character
commands entered by the user.  The commands understood are:

- `n`:  Move the cursor to the next line (if any).
- `N`:  Move the cursor to the next screen (if any).
- `p`:  Move the cursor to the previous line (if any).
- `P`:  Move the cursor to the previous screen (if any).
- `o`:  Open the directory at the current line and display an indented list of its contents.
- `c`:  Close the directory at the current line, removing the indented list of its contents.
- `q`:  Quit the program.
- `\f` ("form feed", typed as `CTRL-L`):  Clear the screen and refresh the contents.
- `v`:  Open the file at the current line and enter "view mode".

In "view mode", the contents of a file are displayed, rather than information about
files and directories.  The file is assumed to contain text.  While in view mode,
the `o`, `c`, `v`, and `q` commands are disabled.  In addition, the following new command
becomes available:

- `ESC`: Exit view mode, returning to the initial display mode (file and directory info).

What you have to do is to first get the program to compile (for the most part,
I did not modify the original code, which requires some changes for it
to compile cleanly with the compiler and settings we are using).
Then, you need to test the program and find and fix the bugs that prevent it
from functioning properly.  Some of the bugs existed in the original version and
some I introduced for the purposes of this assignment.
Finally, you will make some modifications to the program.

As you work on the program, limit the changes you make to the minimum necessary
to achieve the specified objectives.  Don't rewrite the program;
assume that it is essentially correct and just fix a few compilation errors and
bugs as described below.  You will likely find it helpful to use `git` for this (I did).
Make exploratory changes first on a side branch (*i.e.* not the master branch),
then when you think you have understood the proper changes that need to be made,
go back and apply those changes to the master branch.  Using `git` will help you
to back up if you make changes that mess something up.

### Getting Started - Obtain the Base Code

Fetch base code for `hw2` as you did for the previous assignments.
You can find it at this link:
[https://gitlab02.cs.stonybrook.edu/cse320/hw2](https://gitlab02.cs.stonybrook.edu/cse320/hw2).

Once again, to avoid a merge conflict with respect to the file `.gitlab-ci.yml`,
use the following command to merge the commits:

<pre>
  git merge -m "Merging HW2_CODE" HW2_CODE/master --strategy-option=theirs
</pre>

  > :nerd: I hope that by now you would have read some `git` documentation to find
  > out what the `--strategy-option=theirs` does, but in case you didn't :angry:
  > I will say that merging in `git` applies a "strategy" (the default strategy
  > is called "recursive", I believe) and `--strategy-option` allows an option
  > to be passed to the strategy to modify its behavior.  In this case, `theirs`
  > means that whenever a conflict is found, the version of the file from
  > the branch being merged (in this case `HW2_CODE/master`) is to be used in place
  > of the version from the currently checked-out branch.  An alternative to
  > `theirs` is `ours`, which makes the opposite choice.  If you don't specify
  > one of these options, `git` will leave conflict indications in the file itself
  > and it will be necessary for you to edit the file and choose the code you want
  > to use for each of the indicated conflicts.

Here is the structure of the base code:

<pre>
.
├── .gitignore
├── .gitlab-ci.yml
└── hw2
    ├── include
    │   └── browse.h
    ├── Makefile
    ├── src
    │   ├── command.c
    │   ├── display.c
    │   ├── info.c
    │   ├── main.c
    │   └── view.c
    └── tests
        ├── basecode_tests.c
        ├── curses_stub.c
        ├── rsrc
        │   ├── start_EOF.in
        │   ├── start_EOF.out
        │   ├── start_quit.in
        │   ├── start_quit.out
        │   ├── start_refresh.in
        │   ├── start_refresh.out
        │   ├── test_dir.tgz
        │   ├── valgrind_leak.in
        │   ├── valgrind_leak.out
        │   ├── valgrind_undef.in
        │   ├── valgrind_undef.out
        │   ├── view_mode.in
        │   └── view_mode.out
        ├── test_common.c
        └── test_common.h
</pre>

The `src` directory contains the following:

- `command.c` -- Contains the command-loop that responds to user commands.
- `display.c` -- Contains functions that interface between the `ncurses` library
and the rest of the program.
- `info.c` -- Contains functions to obtain information about a file and insert
it into a list of lines to be displayed.
- `main.c` --  Contains the top-level `main()` function.
- `view.c` -- Implements "view mode" functionality.

The `tests` directory contains C source code (in file `basecode_tests.c`) for some Criterion
tests that can help guide you toward bugs in the program.  These are not necessarily complete
or exhaustive.  The `test_common.c` and `test_common.h` contain auxiliary code used
by the tests.  The subdirectory `tests/rsrc` contains input files and reference output files
that are used by the tests.

The `browse` program was not designed to be particularly conducive to unit testing,
so all the tests we will make (including the tests used in grading) will be so-called
"black box" tests, which test the input-output behavior of the program running as a
separate process from the test driver.
As `browse` is a screen-oriented program designed to interact with user, we need to use
some hackery to make it possible to perform automated testing on it.
This is the purpose of the `tests/curses_stub.c` source file.  It contains "stubs"
for each of the `ncurses` functions that are called by the program.  Instead of sending
binary commands to control the screen, the stubs produce textual output to `stdout`.
This will enable us to redirect the output of the `browse` program to a file during a
test and use a script to compare the output so produced to reference output.
The `getch()` function used to read from the keyboard also has a stub, which
reads from `stdin`.  It expects each command to be entered on a separate line:
the actual command character is the first character on the line and the line
is terminated by a newline character (intervening characters are ignored).

Instead of running `bin/browse`, which expects to interact with the screen,
the test functions in `tests/basecode_tests.c` run the executable `bin/browse_stubbed`.
This executable is compiled from the same source code as `bin/browse`, except it is
linked with the stubs from `curses_stub.c` rather than with the `ncurses` library.
You can also run `bin/browse_stubbed` from the command line; in that case it will
not take over the terminal window but will instead produce textual output.

Before you begin work on this assignment, you should read the rest of this
document.  In addition, we additionally advise you to read the
[Debugging Document](DebuggingRef.md).

# Part 1: Debugging and Fixing

You are to complete the following steps:

1. Clean up the code; fixing any compilation issues, so that it compiles
   without error using the compiler options that have been set for you in
   the `Makefile`.
   Use `git` to keep track of the changes you make and the reasons for them, so that you can
   later review what you have done and also so that you can revert any changes you made that
   don't turn out to be a good idea in the end.

2. Fix bugs.

    Run the program, exercising the various options, and look for cases in which the program
    crashes or otherwise misbehaves in an obvious way.  We are only interested in obvious
    misbehavior here; don't agonize over program behavior that might just have been the choice
    of the original author.  You should use the provided Criterion tests to help point the way,
	though they are not guaranteed to be exhaustive.

3. Use `valgrind` to identify any memory leaks or other memory access errors.
   Fix any errors you find.

    Run `valgrind` using a command of the following form:

    <pre>
      $ valgrind --leak-check=full --show-leak-kinds=all --undef-value-errors=yes [BROWSE PROGRAM AND ARGS]
    </pre>

    Note that the bugs that are present will all manifest themselves in some way
    either as incorrect output, program crashes or as memory errors that can be
	detected by `valgrind`.  It is not necessary to go hunting for obscure issues
	with the program output.
    Also, do not make gratuitous changes to the program output, as this will
    interfere with our ability to test your code.

   > :scream:  Note that we are not considering memory that is "still reachable"
   > to be a memory leak.  This corresponds to memory that is in use when
   > the program exits and can still be reached by following pointers from variables
   > in the program.  Although some people consider it to be untidy for a program
   > to exit with "still reachable" memory, it doesn't cause any particular problem.

   > :scream: You are **NOT** allowed to share or post on PIAZZA
   > solutions to the bugs in this program, as this defeats the point of
   > the assignment. You may provide small hints in the right direction,
   > but nothing more.

# Part 2: Changes to the Program

## Rewrite/Extend Options Processing

As indicated above, the basecode version of `browse` takes a single optional
command-line argument, which is interpreted as the name of a directory to browse.
Options processing is performed as part of the function `main()`.
Since the options possibilities are so simple, the basecode version uses
*ad hoc* techniques to process the options, which is typical of what many simple
C programs might do.  However, as options processing is a common function that is
performed by most programs, and it is desirable for programs on the same system to be
consistent in how they interpret their arguments,
there have been more elaborate standardized libraries that have been written
for this purpose.  In particular, the POSIX standard specifies a `getopt()` function,
which you can read about by typing `man 3 getopt`.  A significant advantage to using a
standard library function like `getopt()` for processing command-line arguments,
rather than implementing *ad hoc* code to do it, is that all programs that uses
the standard function will perform argument processing in the same way
rather than having each program implement its own quirks that the user has to
remember.

For this part of the assignment, you are to replace the original argument-processing
code in `main()` by code that uses the GNU `getopt` library package.
In addition to the POSIX standard `getopt()` function, the GNU `getopt` package
provides a function `getopt_long()` that understands "long forms" of option
arguments in addition to the traditional single-letter options.
In your revised program, `main()` should use `getopt_long()` to traverse the
command-line arguments, and it should support the following options
(which are not supported by the original program -- you have to add support for them):

  - `-s {name|date|size|none}`: Within each directory, the list of files and subdirectories
	of that	directory is to be displayed sorted in increasing order according to the
    specified sort key.  If the key is `name`, then the sorting is to be done
    lexicographically (case-insensitive) by file name.  If the key is `date`, the sorting
    is to be done by the file creation date, most recently created first.
    If the key is `size`, then the sorting is to be done by file size, smallest file first.
	If the key is `none`, then no sorting is done and the files and subdirectories are
	displayed in the order in which they are produced by `readdir(3)` (this is what the
	basecode does).
  - `--sort-key={name|date|size|none}`: These are alternative "long forms" for the
    `-s` options just described.
  - `--human-readable`: If this option is specified, then the file sizes are displayed in
	"human-readable form", rather than just as a numeral.  A "human-readable" file size
	is just a numeral, for file sizes less than 1024 bytes.  Files with sizes greater than
	or equal to one KiB (*i.e.* 2<sup>10</sup> bytes) and less than one MiB
    (*i.e.* 2<sup>20</sup> bytes) are displayed as a numeral followed immediately the
    letter `K` (example: `11K`).  In this case, the numeral is the file size divided by
	2<sup>10</sup>, rounded down to the nearest integer.  Similarly files with sizes
    greater than or equal to one MiB and less than one GiB (*i.e.* 2<sup>30</sup> bytes)
    are displayed as a numeral folled by the letter `M` (example: `2M`).

    Note that the `--human-readable` option has no short form.

You will probably need to read the Linux "man page" on the `getopt` package.
This can be accessed via the command `man 3 getopt`.  If you need further information,
search for "GNU getopt documentation" on the Web.

> :scream: You MUST use the `getopt_long()` function to process the command line
> arguments passed to the program.  Your program should be able to handle cases where
> the (non-positional) flags are passed IN ANY order.  Make sure that you test the
> program with prefixes of the long option names, as well as the full names.

## Eliminate MAXLINE Limitation

The original program defines a constant `MAXLINE` (as a macro) and imposes an arbitrary
limit of `MAXLINE` characters as the maximum length of a line of information to be
displayed about a file or directory, and as the maximum length of a line of text
from a file displayed in view mode.
Your job is to eliminate this limitation, so that the program can handle arbitrarily
long lines using dynamically allocated memory.
To support this change, the header file `browse.h` has conditionalized the use of
`MAXLINE` on the preprocessor symbol `NO_MAXLINE` not having been defined, so that
if `gcc` is run with the option `-DNO_MAXLINE`, then `MAXLINE` will be omitted,
and if `gcc` is run without this option, then `MAXLINE` will be included.
The `Makefile` has also been written so that it tests whether the `make` variable
`NO_MAXLINE` is defined, and supplies the `-DNO_MAXLINE` flag to `gcc` accordingly.
The upshot of all this is that if you compile your program using `make NO_MAXLINE=1`,
then the definition of `MAXLINE` in `browse.h` will be omitted and the definition
of type `NODE` will be changed so that the `data` field has type `char *` rather
than type `char [MAXLINE+1]`.
You will need to change the rest of the code to avoid the use of `MAXLINE`
(by dynamically allocating the storage for the `data` strings in nodes)
if the `NO_MAXLINE` preprocessor symbol has been defined.
During grading, we will attempt to compile your code using `make NO_MAXLINE=1`.
If this fails, then as a fallback we will attempt to compile using just `make`,
but in that case you will not get any points for this part of the assignment.

> :scream: Do not make any changes to the `Makefile` or to the `browse.h` header file,
> as these will be replaced during grading.

In order to avoid the `MAXLINE` limitation, you will need to dynamically allocate
storage for the `data` strings in nodes, and you will need to store pointers to
these dynamically allocated strings in the `data` field, rather than copying the
data into the `data` array as is done in the basecode.
In some cases, you may wish to use `strdup()` to allocate the memory for the `data`
string.  However, the function `cvt_info` uses `sprintf()` to format the contents
of this string and it is awkward to precompute the size needed for the string,
so that appropriate memory can be allocated using `malloc()`.
In this case, you should use the function `open_memstream()` to create an "output"
stream whose target is dynamically allocated memory.  You will need to read the
`man` page for this function, but the short story is that you would open a stream
using `open_memstream()`, print data into this stream using `fprintf()`,
use `fclose()` to close the stream, and then store a pointer to the string that
has been created into the `data` field of a `NODE`.

## Free Memory

The original program dynamically allocates storage for a list of "nodes" to
be displayed using `malloc()`.  However, under certain circumstances it fails
to properly `free()` this storage and it is possible for nodes to be leaked.
The function `opendir()` used to read the contents of a directory also dynamically
allocates storage and this is not properly freed by the basecode.
Finally, if you eliminated the `MAXLINE` limitation as discussed above, then
you also allocated storage, and if you did not take steps to free this storage,
then it will be leaked.

What you are to do is to ensure that the program does not leak any memory,
either in normal operation or during the handling of any exceptional conditions.
Leaked memory is reported by `valgrind` as "lost".  You do not have to
worry about memory reported by `valgrind` as "still reachable" when the program
terminates.
In order to be successful at this part of the assignment, you have to analyze
the existing program to determine what memory is allocated, when (if ever) that
memory is freed, and to decide appropriate points in the code at which to
free memory to avoid leaks.
You are likely to find this somewhat difficult.
If you are successful with these modifications, when you run the program with
`valgrind` it will not report any memory that is "lost" at the end of execution.

# Part 3: Testing the Program

For this assignment, you have been provided with a basic set of
Criterion tests to help you debug the program.

In the `tests/basecode_tests.c` file, there are five test examples.
You can run these with the following command:

<pre>
    $ bin/browse_tests
</pre>

To obtain more information about each test run, you can supply the
additional option `--verbose=1`.
You can also specify the option `-j1` to cause the tests to be run sequentially,
rather than in parallel using multiple processes, as is the default.
The `-j1` flag is necessary if the tests could interfere with each other in
some way if they are run in parallel (such as writing the same output file).
You will probably find it useful to know this; however the basecode tests have
been written so that they each use output files named after the test and
(hopefully) will not interfere with each other.

The tests have been constructed so that they will point you at most of the
problems with the program.
Each test has one or more assertions to make sure that the code functions
properly.  If there was a problem before an assertion, such as a "segfault",
the test will print the error to the screen and continue to run the
rest of the tests.
Some of the tests use `valgrind` to verify that no memory errors are found.
If errors are found, then you can look at the log file that is left behind
(in the `tests.out` directory) by the test code.
Alternatively, you can better control the information that `valgrind` provides
if you run it manually.

The basecode test cases check the program operation by reading input from
a pre-defined input file, redirecting `stdout` and `stderr` to output files,
and comparing the output produced against pre-defined reference files.
In order to avoid variation (such as creation dates, order in which files
appear in directories, *etc*.) that can occur, the tests use a "test tree"
that is re-created each time the tests are run.  The source for the test tree
is the "tarball" `tests/rsrc/test_dir.tgz`.  Each test runs a setup function
that deletes any existing version of the test directory and recreates it by
unpacking the tarball.

The tests included in the base code are not true "unit tests", because they all
run the program as a black box using `system()`.
You should be able to follow the pattern to construct some additional tests of
your own, and you might find this helpful while working on the program.
You are encouraged to try to write some of these tests so that you learn how
to do it.  Note that in the next homework assignment unit tests will likely
be very helpful to you and you will be required to write some of your own.
Criterion documentation for writing your own tests can be found
[here](http://criterion.readthedocs.io/en/master/).

  > :scream: Be sure that you test non-default program options to make sure that
  > the program does not crash or otherwise misbehave when they are used.

# Part 4: Debugging Hints

When the `ncurses` library is initialized, it co-opts the normal terminal
behavior.  This can make debugging awkward.
There are two ways to work around this issue.  One is to do your debugging
on the `browse_stubbed` program, which does not use the `ncurses` library.
This approach might be helpful if you have already identified an issue with
the behavior of the program.  However, it will be difficult to look at the
textual output produced by this version of the program and to try to imagine
what it might look like when displayed using `ncurses`.

An alternative approach is to use `gdb` to do the debugging, but to start
`gdb` in a separate terminal window and then attach to the process to be
debugged.
Here is how you would do this:  First, start `bin/browse` in one terminal
window (which it will take over with `ncurses`).  Then, in another terminal
window, run `ps -a` to find out the process ID of the process running
`bin/browse`.  Start `gdb` via `gdb bin/browse` as usual.  Once `gdb` is running,
issue the command `attach nnn`, where `nnn` is the process ID of the process
you want to debug.  If all goes well, `gdb` should attach to the process,
the process should be stopped, and you will be able to browse the stack,
variables, etc.  It might be that when you try to attach you will get
a permission failure.  This is because restrictions are placed on which
processes can attach to which other processes for debugging, to avoid various
security holes.  If this occurs, then on your VM, you should be able to work
around it by starting `gdb` with `sudo`.

You may of course also use the macros in `debug.h` to cause your program to
produce debugging output when it is compiled with `make debug`.
This is also inconvenient when `ncurses` is active, because the debugging
output will appear intermixed with the normal output of the program in the
terminal window, but that  might be good enough in some cases.

# Hand-in Instructions

Ensure that all files you expect to be on your remote repository are committed
and pushed prior to submission.

This homework's tag is: `hw2`

<pre>
$ git submit hw2
</pre>
