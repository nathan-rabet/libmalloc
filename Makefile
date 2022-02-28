CC = gcc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -Wall -Wextra -Werror -std=c99 -fPIC -fno-builtin -Iinclude
LDFLAGS = -shared

TARGET_LIB = libmalloc.so
SRC = $(shell find src -name '*.c')
OBJS = $(SRC:.c=.o)

SRC_TESTS = $(shell find tests/src -name '*.c')
OBJ_TESTS = $(SRC_TESTS:.c=.o)

all: library

library: $(TARGET_LIB)
$(TARGET_LIB): CFLAGS += -pedantic -fvisibility=hidden
$(TARGET_LIB): LDFLAGS += -Wl,--no-undefined
$(TARGET_LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

debug: CFLAGS += -g
debug: clean $(TARGET_LIB)
	
check: tests
	./tests_suite

tests: $(OBJS) $(OBJS_TESTS)
	$(CC) -lcriterion -o tests_suite $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

# %: $(TARGET_LIB)
# 	LD_PRELOAD=$(TARGET_LIB) ./$@
	
clean:
	$(RM) $(TARGET_LIB) $(OBJS)

.PHONY: all $(TARGET_LIB) clean
