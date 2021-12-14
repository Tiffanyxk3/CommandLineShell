# Output binary name
bin=fish

# Set the following to '0' to disable log messages:
LOGGER ?= 1

# Compiler/linker flags
CFLAGS += -g -Wall -fPIC -DLOGGER=$(LOGGER)
LDLIBS += -lm -lreadline
LDFLAGS +=

src=history.c shell.c ui.c util.c builtin.c job.c
obj=$(src:.c=.o)

all: $(bin) libshell.so

$(bin): $(obj)
	$(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(obj) -o $@

libshell.so: $(obj)
	$(CC) $(CFLAGS) $(LDLIBS) $(LDFLAGS) $(obj) -shared -o $@

shell.o: shell.c history.h ui.h util.h builtin.h logger.h
history.o: history.c history.h ui.h util.h builtin.h logger.h
ui.o: ui.c ui.h util.h builtin.h logger.h
util.o: util.c util.h history.h ui.h builtin.h logger.h
builtin.o: builtin.c builtin.h history.h ui.h logger.h
job.o: job.c job.h history.h ui.h util.h builtin.h

clean:
	rm -f $(bin) $(obj) libshell.so vgcore.*


# Tests --

test: $(bin) libshell.so ./tests/run_tests
	@DEBUG="$(debug)" ./tests/run_tests $(run)

testupdate: testclean test

./tests/run_tests:
	rm -rf tests
	git clone https://github.com/USF-OS/Shell-Tests.git tests

testclean:
	rm -rf tests
