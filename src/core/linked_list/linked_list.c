#include "linked_list.h"

// Lockfree algorithms

char find_first_predicate(linked_list_t *list, char (*predicate)(void *))
{
    linked_list_t *scrut = list;
    while (scrut != NULL)
    {
        if (predicate(scrut))
            return 1;
        scrut = scrut->next;
    }
    return 0;
}

char find_first_predicate_and_insert(linked_list_t *list,
                                     char (*predicate)(void *), void *element)
{
    linked_list_t *scrut = list;
    while (scrut != NULL)
    {
        if (predicate(scrut))
        {
            insert_after(scrut, element);
            return 1;
        }
        scrut = scrut->next;
    }
    return 0;
}

// Lockfree algorithms, use builtins to stay atomic
void insert_after(linked_list_t *list, void *element)
{
    linked_list_t *new_element = (linked_list_t *)element;
    // new_element->next = list->next;
    // list->next = new_element;
    linked_list_t *pikalul = list->next;
    do
    {
        list->next = pikalul;
        // set up `elem->head` so the list will still be linked
        // correctly the instant the element is inserted
    } while (!__sync_bool_compare_and_swap(&list->next, pikalul, new_element));
    // il faut memcpy
}
