CC = gcc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -Wall -Wextra -Werror -std=c99  -pedantic -Iinclude 
LDFLAGS = 

TARGET_LIB = libmalloc.so
SRC = $(shell find src/core -name '*.c')
OBJS = $(SRC:.c=.o)
SRC_AND_LIB = $(shell find src -name '*.c')
OBJS_AND_LIB = $(SRC_AND_LIB:.c=.o)

SRC_TESTS = $(shell find tests/unit_testing -name '*.c')
OBJ_TESTS = $(SRC_TESTS:.c=.o)

all: library

library: $(TARGET_LIB)
$(TARGET_LIB): CFLAGS += -pedantic -fvisibility=hidden -fPIC -fno-builtin
$(TARGET_LIB): LDFLAGS += -Wl,--no-undefined -shared
$(TARGET_LIB): $(OBJS_AND_LIB) src/malloc.c src/additional_malloc.c
	$(CC) $(LDFLAGS) -o $@ $^

debug: CFLAGS += -g
debug: clean $(TARGET_LIB)
	
check: tests
	./tests_suite

tests: CFLAGS += -g
tests: $(OBJS) $(OBJ_TESTS)
	$(CC) $(CPPFLAGS) $(CFLAGS) -lcriterion -lm -o tests_suite $^

test_main: CFLAGS += -g
test_main: LDFLAGS += -lm
test_main: $(OBJS) tests/test_main.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o test_main $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# %: $(TARGET_LIB)
# 	LD_PRELOAD=$(TARGET_LIB) ./$@
	
clean:
	$(RM) $(TARGET_LIB) $(OBJS_AND_LIB) $(OBJ_TESTS) tests_suite tests/test_main.o

.PHONY: all $(TARGET_LIB) clean
