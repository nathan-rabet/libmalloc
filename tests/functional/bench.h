#ifndef BENCH_H
#define BENCH_H

// libc benchmarkings
void bench_malloc_sparse(int N);
void bench_malloc_bubble(int N);
void bench_malloc_tiny1(int N);
void bench_malloc_tiny2(int N);
void bench_malloc_big1(int N);
void bench_malloc_big2(int N);
void bench_malloc_thread_stress(int n);
void bench_malloc_thread_local(int n);

// my_malloc benchmarkings
void malloc_then_free_then_malloc(void);
void heap_fucker(void);
void basic1(void);
void basic2(void);

#endif /* BENCH_H */
