#ifndef CHAD_LINKED_LIST_H
#define CHAD_LINKED_LIST_H

#include <stddef.h>
#include <stdint.h>

typedef struct _linked_list
{
    struct _linked_list *next;
    size_t sz;
} linked_list_t;

char find_first_predicate(linked_list_t *list, char (*predicate)(void *));
char find_first_predicate_and_insert(linked_list_t *list,
                                     char (*predicate)(void *), void *element);

#endif // CHAD_LINKED_LIST_H
