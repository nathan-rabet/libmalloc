# IMPLEMENTATION : Theorical explanation

In order to use this allocator library in a kernel, this implementation of the `malloc(3)` library is based on the [SLAB allocator](https://en.wikipedia.org/wiki/Slab_allocation) method.

SLAB allocation is useful for kernel because it ensure a good memory optimizatition for objects that have the same size (such as semaphore, mutex, threads, ...), this is why SLAB allocation is the most efficient in the kernel context.

## SLAB allocation overview

This type of allocator use 4 types of objects :

- SLAB groups
- SLAB caches
- SLAB meta
- SLABS

## SLAB

A SLAB is a block of memory that is used to store objects. It is composed of :

- The address of a SLAB meta (with an offset, to point to the current SLAB information in the SLAB meta free list)
- Canaries for memory protection
- The user addressable memory

The size of a SLAB corresponds to the size of the user addressable memory. For example, if a user can place 4 bytes in the memory (because he called `malloc(4)` for example), the size of the SLAB is 4 bytes.

### Canary protection

To protect the user from memory corruption, canaries are placed between the address of it's SLAB meta pointer. The canary is used to detect memory corruption. If the canary is modified, `free()` and `realloc()` will not be able to perform any action on this object (but they will not crash).

### Virgin slab

A virgin SLAB is a SLAB which was not used yet. It's user addressable memory has only zeros. (Useful for `calloc()`)

### Dirty slab

A dirty SLAB is a SLAB which has been used. It's user addressable memory may have been modified. (Useful for `calloc()`)

## SLAB meta

A SLAB meta is a linked-list that contains informations about a bunch of SLABs (all with the same size). This structure is different from a SLAB metadata in the user-space because it is on another page & handle multiple SLABs.

It is composed of :

- A pointer to the next SLAB meta
- A pointer to the corresponding SLAB group
- A pointer to the beginning of the handled SLABs
- A contigious free-list of SLABs (boolean array)
- A contigious list of dirty SLABs (boolean array)

## SLAB groups

A **SLAB group** is an element which regroup SLABS meta that can handle objects of a certain size. All SLAB (and all SLAB groups) sizes are a power of 2 (1, 2, 4, 8, 16, 32, ...)

For example, a object with a size of 12 bytes can be handled by a SLAB group with SLABS of size 16 bytes.

A SLAB group is composed of :

- A pointer to the next SLAB group
- A size of the handled SLABs
- A pointer to the first SLAB meta
- A cache of free SLABs


## Minimum SLAB group

A minimum SLAB group is a SLAB group which have a size as power of 2 immediatly superior or equal to the size of the object.

For example, a object with a size of 12 bytes can be handled by a minimum SLAB group of size 16 bytes. For a object with a size of 32 bytes, it can be handled by a minimum SLAB group of size 32 bytes.

## SLAB cache

A **SLAB cache** is contained in a SLAB group. It is a list of free SLABS that can be used to allocate objects.

This list is updated when an object is allocated or freed.

This way, the allocator can find a free SLAB quickly, instead of searching in all the SLAB meta linked-list.

## Allocation & Deallocation workflows

### `malloc()` workflow

Here is a pseudo code for the `malloc()` workflow :

- Search a minimum SLAB group that can handle the size of the object.
- If the minimum SLAB group is found.
  - Search in the cache if there is a free SLAB
  - If there is a free SLAB, mark it as used and dirty, and return the corresponding pointer to the user.
- If no SLAB group is found, create a new SLAB group
- Search in the free-list of the first SLAB meta
  - If there is a free SLAB, use it, mark it as used and dirty, and return the corresponding pointer to the user.
- Else, search in the free-list of the next SLAB meta
- If there is no more available SLAB in any SLAB meta, create a new SLAB meta and use its first SLAB (and mark it as used and dirty).

### `calloc()` workflow

This is the same as `malloc()` but with the addition of a virginity checking (on the dirty array of the SLAB meta).

### `free()` workflow

Here is a pseudo code for the `free()` workflow :

- Check if the pointer is valid
- Check if the canaries are valid
- If the cannaries are not valid, do nothing
- Else, go to the corresponding SLAB meta
- Add the SLAB to the cache of free SLABs
- Mark the SLAB as unused in the SLAB meta

### `realloc()` workflow

Here is a pseudo code for the `realloc()` workflow :

- If a reallocation must be done (cf the standard manual)
  - Verify the canaries of the old object
  - If the canaries are modified, return NULL
  - Else, `malloc()` a new object
  - Copy the old object to the new location. (note that the new size may be smaller than the old one, so the copy may be truncated)
  - `free()` the old object
  - Return the new object
