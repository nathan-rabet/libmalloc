CC = gcc
CPPFLAGS = -D_DEFAULT_SOURCE
CFLAGS = -Wall -Wextra -Werror -std=c99  -pedantic -Iinclude 

TARGET_LIB = libmalloc.so
DEBUG_LIB = libmalloc_debug.so

SRC = src/core/slab/cache.c src/core/slab/slab_meta.c src/core/slab/slab_group.c src/core/slab/slab_data.c src/core/utils/maths.c src/core/utils/bit.c src/core/utils/cast.c src/core/utils/overflow.c
OBJS = $(SRC:.c=.o)
SRC_AND_LIB = src/core/slab/cache.c src/core/slab/slab_meta.c src/core/slab/slab_group.c src/core/slab/slab_data.c src/core/utils/maths.c src/core/utils/bit.c src/core/utils/cast.c src/core/utils/overflow.c src/malloc.c src/additional_malloc.c
OBJS_AND_LIB = $(SRC_AND_LIB:.c=.o)

FUNCTIONAL_SRC = tests/functional/glibc_bench.c
FUNCTIONAL_SRC += tests/functional/main_bench.c
FUNCTIONAL_SRC += tests/functional/my_bench.c

FUNCTIONAL_OBJS = $(FUNCTIONAL_SRC:.c=.o)

SRC_TESTS = tests/unit_testing/bit_test.c tests/unit_testing/overflow_test.c tests/unit_testing/slab_meta_test.c tests/unit_testing/cache_test.c tests/unit_testing/page_test.c tests/unit_testing/slab_data_test.c tests/unit_testing/maths_test.c tests/unit_testing/slab_group_test.c
OBJ_TESTS = $(SRC_TESTS:.c=.o)

all: library

library_debug: $(DEBUG_LIB)
$(DEBUG_LIB): CFLAGS += -g -fPIC -fno-builtin
$(DEBUG_LIB): LDFLAGS += -Wl,--no-undefined -shared
$(DEBUG_LIB): $(OBJS_AND_LIB)
	$(CC) $(LDFLAGS) -o $@ $^

library: $(TARGET_LIB)
$(TARGET_LIB): CFLAGS += -fvisibility=hidden -fPIC -fno-builtin
$(TARGET_LIB): LDFLAGS += -Wl,--no-undefined -shared
$(TARGET_LIB): $(OBJS_AND_LIB)
	$(CC) $(LDFLAGS) -o $@ $^
	
check: tests functional_tests $(DEBUG_LIB)
	./tests_suite
	ASAN_OPTIONS=detect_leaks=0 LD_PRELOAD=./$(DEBUG_LIB) ./functional_tests

checkv: tests functional_tests $(DEBUG_LIB)
	./tests_suite --verbose
	ASAN_OPTIONS=detect_leaks=0 LD_PRELOAD=./$(DEBUG_LIB) ./functional_tests

functional_tests: LDFLAGS += -lm -lpthread
functional_tests: CFLAGS = -std=c99 -g
functional_tests: $(FUNCTIONAL_OBJS)
	$(CC) $(CFLAGS) -o functional_tests $^ $(LDFLAGS)

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

preload_debug: $(DEBUG_LIB)
	@echo "preload DEBUG MODE"
	LD_PRELOAD=./$(DEBUG_LIB) $(PROGRAM)

preload: $(TARGET_LIB)
	@echo "preload PRODUCTION MODE"
	LD_PRELOAD=./$(TARGET_LIB) $(PROGRAM)
	
clean:
	$(RM) $(TARGET_LIB) $(DEBUG_LIB) $(OBJS_AND_LIB) \
	$(OBJ_TESTS) $(FUNCTIONAL_OUT) $(FUNCTIONAL_OBJS) \
	tests_suite tests/test_main.o tests/criterion_debug.o \
	test_main debug_criterion functional_tests

.PHONY: all $(TARGET_LIB) clean functional_tests
