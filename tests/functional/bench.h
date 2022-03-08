#ifndef BENCH_H
#define BENCH_H

// libc benchmarkings
void bench_malloc_sparse(unsigned long N);
void bench_malloc_bubble(unsigned long N);
void bench_malloc_tiny1(unsigned long N);
void bench_malloc_tiny2(unsigned long N);
void bench_malloc_big1(unsigned long N);
void bench_malloc_big2(unsigned long N);
void bench_malloc_thread_stress(unsigned long n);
void bench_malloc_thread_local(unsigned long n);

// my_malloc benchmarkings
void malloc_then_free_then_malloc(void);
void heap_fucker(void);
void basic1(void);
void basic2(void);

#endif /* BENCH_H */
