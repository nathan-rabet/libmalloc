#include <stdio.h>
#include <stdlib.h>

#include "bench.h"

int main(void)
{
    // my_malloc benchmarkings
    malloc_then_free_then_malloc();
    basic1();
    basic2();
    heap_fucker();

    // glibc benchmarkings
    bench_malloc_sparse(42);
    bench_malloc_bubble(42);
    bench_malloc_big2(42);
    bench_malloc_big1(42);
    bench_malloc_tiny1(42);
    bench_malloc_tiny2(42);

    // TODO: multithreaded benchmarks
    // bench_malloc_thread_local(42);
    // bench_malloc_thread_stress(42);

    return 0;
}
