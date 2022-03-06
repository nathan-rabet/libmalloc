CC = gcc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -Wall -Wextra -Werror -std=c99  -pedantic -Iinclude 

TARGET_LIB = libmalloc.so
SRC = src/core/slab/cache.c src/core/slab/slab_meta.c src/core/slab/slab_group.c src/core/slab/slab_data.c src/core/utils/maths.c src/core/utils/bit.c src/core/utils/cast.c src/core/utils/overflow.c
OBJS = $(SRC:.c=.o)
SRC_AND_LIB = src/core/slab/cache.c src/core/slab/slab_meta.c src/core/slab/slab_group.c src/core/slab/slab_data.c src/core/utils/maths.c src/core/utils/bit.c src/core/utils/cast.c src/core/utils/overflow.c src/malloc.c src/additional_malloc.c
OBJS_AND_LIB = $(SRC_AND_LIB:.c=.o)

SRC_TESTS = tests/unit_testing/bit_test.c tests/unit_testing/overflow_test.c tests/unit_testing/slab_meta_test.c tests/unit_testing/cache_test.c tests/unit_testing/page_test.c tests/unit_testing/slab_data_test.c tests/unit_testing/maths_test.c tests/unit_testing/slab_group_test.c
OBJ_TESTS = $(SRC_TESTS:.c=.o)

all: library

library: $(TARGET_LIB)
$(TARGET_LIB): CFLAGS += -fvisibility=hidden -fPIC -fno-builtin
$(TARGET_LIB): LDFLAGS += -Wl,--no-undefined -shared
$(TARGET_LIB): $(OBJS_AND_LIB)
	$(CC) $(LDFLAGS) -o $@ $^
	
check: tests
	./tests_suite

checkv: tests
	./tests_suite --verbose

dcheck: $(OBJS) tests/criterion_debug.o
	$(CC) $(CFLAGS) -lcriterion -lm -o debug_criterion $^
	./debug_criterion --debug --verbose

tests: LDFLAGS += -lm -lcriterion -fsanitize=address
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
