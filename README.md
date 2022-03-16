# `malloc(3)` : the standard dynamic memory allocator

## Project motivations

The project is initialy motivated by EPITA to understand how allocators work and what types of algorithms we can use for certain type of allocations. In order to implement the library in a kernel, I decided to implement the standard `malloc(3)` with a fast, secure and reliable algorithm which is really used in modern systems libraries (like the *glibc*) : the **SLAB allocator** method.

## Project goals

Currently, the project support the following features :

- [x] Reliable single threaded allocations.
- [x] Faster allocation using cached metadata.
- [x] User-space metadata corruption safety.

In order to include this `malloc(3)` library in a micro-kernel, we are working on the following features :

- [ ] Multi-threaded allocations.
- [ ] `aligned_alloc()`, `malloc_usable_size()`, `memalign()` and `posix_memalign()` support.
- [ ] `long double` address alignment for all objects (currently supporting objects upper or equal than `sizeof(long double)` bytes.

## Compilation

To compile the dynamic library, you need to use the following command :

    make library

> Note that you can also use the `make`.

To compile the library in debug mode (debug symbols + `assert(3)` enabled), you need to use the following command :

    make library_debug

Use  `make clean` command to clean the compiled objects.

## Implementation & architecture

To understand the implementation, you should read the [IMPLEMENTATION.md](IMPLEMENTATION.md) file.
If you want to contribute to the project and want to have a better understanding of the architecture, you should read the [ARCHITECTURE.md](ARCHITECTURE.md) file.
