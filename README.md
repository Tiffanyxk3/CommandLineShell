# Command Line Shell

## Description
- An implementation of command line shell.
- Upon startup, the shell prints its prompt and wait for user input. 
- The shell is able to run commands in both the current directory and those in the PATH environment variable. 
- The shell prompt displays some helpful information including:
  1. Command number (starting from 1)
  2. User name and host name: (username)@(hostname) followed by :
  3. The current working directory
  4. Process exit status
- Supports scripting mode to run the test cases.
- Supports built-in commands including:
  1. `cd`: Changing the CWD. cd without arguments returns to the user’s home directory.
  2. `#` (comments): Strings prefixed with # are ignored by the shell.
  3. `history`: Printing the last 100 commands entered with their command numbers.
  4. `!` (history execution): Entering !39 will re-run command number 39, and !! re-runs the last command that was entered. !ls re-runs the last command that starts with ‘ls.’ 
  5. `jobs`: Listing currently-running background jobs.
  6. `exit`: Exiting the shell.

## Usage
```bash
make
./<shell name>
```

## Included files
- `Makefile`: Used to compile and run the program.
- `shell.c`: Driver of the program.
- `ui.c`: Building the shell.
- `util.c`: Includes helper functions for other files.
- `builtin.c`: Supports my own built-in functions.
- `history.c`: Functions and data to store history commands.
- `job.c`: Functions and data to store background jobs.
- `ui.h`, `util.h`, `builtin.h`, `history.h`, `job.h`: Header files.

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
