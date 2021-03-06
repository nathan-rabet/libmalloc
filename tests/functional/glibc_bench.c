#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define Len 1000

void bench_malloc_sparse(unsigned long N)
{
    void *p[Len];
    unsigned long i, j;
    for (j = 0; j < N; j++)
    {
        for (i = 0; i < sizeof p / sizeof *p; i++)
        {
            p[i] = malloc(4000);
            memset(p[i], 0, 4000);
        }
        for (i = 0; i < sizeof p / sizeof *p; i++)
            if (i % 150)
                free(p[i]);
        for (i = 0; i < sizeof p / sizeof *p; i += 150)
            free(p[i]);
    }
}
void bench_malloc_bubble(unsigned long N)
{
    void *p[Len];
    unsigned long i, j;
    for (j = 0; j < N; j++)
    {
        for (i = 0; i < sizeof p / sizeof *p; i++)
        {
            p[i] = malloc(4000);
            memset(p[i], 0, 4000);
        }
        for (i = 0; i < sizeof p / sizeof *p; i++)
            free(p[i]);
    }
}
void bench_malloc_tiny1(unsigned long N)
{
    void *p[Len];
    unsigned long i, j;
    for (j = 0; j < N; j++)
    {
        for (i = 0; i < sizeof p / sizeof *p; i++)
        {
            p[i] = malloc((i % 4 + 1) * 16);
        }
        for (i = 0; i < sizeof p / sizeof *p; i++)
        {
            free(p[i]);
        }
    }
}
void bench_malloc_tiny2(unsigned long N)
{
    void *p[Len];
    unsigned long i, j;
    for (j = 0; j < N; j++)
    {
        for (i = 0; i < sizeof p / sizeof *p; i++)
        {
            p[i] = malloc((i % 4 + 1) * 16);
        }
        for (i = 1; i; i = (i + 57) % (sizeof p / sizeof *p))
            free(p[i]);
        free(p[0]);
    }
}
void bench_malloc_big1(unsigned long N)
{
    void *p[Len];
    unsigned long i, j;
    for (j = 0; j < N; j++)
    {
        for (i = 0; i < sizeof p / sizeof *p; i++)
        {
            p[i] = malloc((i % 4 + 1) * 16384);
        }
        for (i = 0; i < sizeof p / sizeof *p; i++)
        {
            free(p[i]);
        }
    }
}
void bench_malloc_big2(unsigned long N)
{
    void *p[Len];
    unsigned i, j;
    for (j = 0; j < N; j++)
    {
        for (i = 0; i < sizeof p / sizeof *p; i++)
        {
            p[i] = malloc((i % 4 + 1) * 16384);
        }
        for (i = 1; i; i = (i + 57) % (sizeof p / sizeof *p))
            free(p[i]);
        free(p[0]);
    }
}
#define SH_COUNT 300
#define PV_COUNT 300
#define MAX_SZ 500
#define DEF_SZ 40

struct foo
{
    void *mem;
    pthread_mutex_t lock;
};

static unsigned rng(unsigned *r)
{
    return *r = *r * 1103515245 + 12345;
}

static unsigned long N;

static void *stress(void *arg)
{
    struct foo *foo = arg;
    unsigned r = (unsigned)pthread_self();
    unsigned long i, j;
    size_t sz;
    void *p;

    for (i = 0; i < N; i++)
    {
        j = rng(&r) % SH_COUNT;
        sz = rng(&r) % MAX_SZ;
        pthread_mutex_lock(&foo[j].lock);
        p = foo[j].mem;
        foo[j].mem = 0;
        pthread_mutex_unlock(&foo[j].lock);
        free(p);
        if (!p)
        {
            p = malloc(sz);
            pthread_mutex_lock(&foo[j].lock);
            if (!foo[j].mem)
                foo[j].mem = p, p = 0;
            pthread_mutex_unlock(&foo[j].lock);
            free(p);
        }
    }
    return 0;
}

void bench_malloc_thread_stress(unsigned long n)
{
    struct foo foo[SH_COUNT] = { { 0 } };
    pthread_t td1, td2;
    void *res;

    N = n;
    pthread_create(&td1, 0, stress, foo);
    pthread_create(&td2, 0, stress, foo);
    pthread_join(td1, &res);
    pthread_join(td2, &res);
}

void bench_malloc_thread_local(unsigned long n)
{
    struct foo foo1[SH_COUNT] = { { 0 } };
    struct foo foo2[SH_COUNT] = { { 0 } };
    pthread_t td1, td2;
    void *res;

    N = n;
    pthread_create(&td1, 0, stress, foo1);
    pthread_create(&td2, 0, stress, foo2);
    pthread_join(td1, &res);
    pthread_join(td2, &res);
}
