# Project architecture

This markdown explain how the project is organized.

## Documentation

All the source code use the [Doxygen](https://www.doxygen.org/) documentation system.

## Source code organization

### `src/`

The `src/` directory contains the source code of the project.

It contains 2 main files :

- `malloc.c` : This file contains the `malloc`, `calloc`, `realloc` and `free` functions.
- `additional_mallloc.c` : This file contains the `aligned_alloc`, `malloc_usable_size`, `memalign` and `posix_memalign` functions.

Only these functions symbols are exported on `make library`.

### `include/`

The `include/` directory contains the header files of the project.

All the header files are placed at the root of this directory.

### `tests/`

The `tests/` directory contains the tests of the project, such as :

- `unit_testing/` : This directory contains the unit tests.
- `functional/` : This directory contains the functional tests.
- `cri` : This directory contains the EPITA provided tests (compiled programs, no sources).

All of these tests are launched with the `make check` command.

`tests/` also contains the `test_main.c` file, which is used to debug manualy some functions. This file can be compiled using `make test_main`.

## Conditional compilation for debug mode

Some parts of the project (like `assert()`'s) are only compiled in debug mode. This is done by using the `DEBUG` macro.
