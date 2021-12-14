# Project 2: Command Line Shell

See: https://www.cs.usfca.edu/~mmalensek/cs326/assignments/project-2.html

To compile and run:

```bash
make
./fish
```

## Description

This program is an implementation of my own shell. In Unix-based systems, the shell is generally a command line interface. 

## Included files

* Makefile: Used to compile and run the program.
* shell.c: Driver of the program.
* ui.c: Building the shell.
* util.c: Includes helper functions for other files.
* builtin.c: Supports my own built-in functions.
* history.c: Functions and data to store history commands.
* job.c: Functions and data to store background jobs.
* Header files of ui.c, util.c, builtin.c, history.c job.c.

## Testing

To execute the test cases, use `make test`. To pull in updated test cases, run `make testupdate`. You can also run a specific test case instead of all of them:

```
# Run all test cases:
make test

# Run a specific test case:
make test run=4

# Run a few specific test cases (4, 8, and 12 in this case):
make test run='4 8 12'
```
