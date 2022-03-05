CC = gcc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -Wall -Wextra -Werror -std=c99  -pedantic -Iinclude 

TARGET_LIB = libmalloc.so
SRC = $(shell find src/core -name '*.c')
OBJS = $(SRC:.c=.o)
SRC_AND_LIB = $(shell find src -name '*.c')
OBJS_AND_LIB = $(SRC_AND_LIB:.c=.o)

SRC_TESTS = $(shell find tests/unit_testing -name '*.c')
OBJ_TESTS = $(SRC_TESTS:.c=.o)

all: library

library: $(TARGET_LIB)
$(TARGET_LIB): CFLAGS += -fvisibility=hidden -fPIC -fno-builtin
$(TARGET_LIB): LDFLAGS += -Wl,--no-undefined -shared
$(TARGET_LIB): $(OBJS_AND_LIB)
	$(CC) $(LDFLAGS) -o $@ $^
	
check: tests
	./tests_suite

dcheck: $(OBJS) tests/criterion_debug.o
	$(CC) $(CFLAGS) -lcriterion -lm -o debug_criterion $^
	./debug_criterion --debug --verbose

tests: LDFLAGS += -lm -lcriterion
tests: CFLAGS += -g
tests: $(OBJS) $(OBJ_TESTS)
	$(CC) $(CFLAGS) -o tests_suite $^ $(LDFLAGS)

test_main: CFLAGS += -g
test_main: LDFLAGS += -lm
test_main: $(OBJS) tests/test_main.o
	$(CC) $(CFLAGS) -o test_main $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# %: $(TARGET_LIB)
# 	LD_PRELOAD=$(TARGET_LIB) ./$@
	
clean:
	$(RM) $(TARGET_LIB) $(OBJS_AND_LIB) $(OBJ_TESTS) \
	tests_suite tests/test_main.o tests/criterion_debug.o \
	test_main debug_criterion

.PHONY: all $(TARGET_LIB) clean
