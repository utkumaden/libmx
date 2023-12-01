/**
 * @file list.h Array List
 * This is an array list implementation inspired by the stb data structures
 * single header library. The list itself works like any C pointer, except
 * the real base pointer is before the pointer you will see.
 */
#ifndef _MX_LIST_H_
#define _MX_LIST_H_

#include "mx/base.h"
#include "mx/assert.h"
#include <stdlib.h>
#include <string.h>

struct mx_list_self_t {
    int count;
};

/**
 * @brief Internal function for retreiving the list header.
 * @param list The list.
 * @returns The list header.
 */
#define mx_list_self(list) (((struct mx_list_self_t*)(list))-1)

/**
 * @brief The number of items in the list.
 * @param list The list.
 * @returns The number of items in the list.
*/
#define mx_list_count(list) (((list) == NULL) ? 0 : mx_list_self(list)->count)

#define mx_list_resize(list, count) do { \
    struct mx_list_self_t *self = (list == NULL) ? NULL : mx_list_self(list); \
    self = realloc(self, sizeof(*self) + sizeof(*(list)) * (count)); \
    if (self != NULL) { \
        self->count = (count); \
        (list) = (void*)(self+1); \
    } \
} while (0)

/**
 * @brief Insert an item into the list (with assignment)
 * @param list The list.
 * @param index The index to insert the item into.
 * @param item The item to insert.
 * @remarks This overload is for things that can be assigned, like base types. Useful for rvalues.
 */
#define mx_list_insert(list, index, item) do { \
    int idx = (index); \
    if (list == NULL) { \
        MX_ASSERT(idx == 0, "mx_list_insert() can only insert at 0 for an empty list."); \
        mx_list_resize(list, 1); \
        list[0] = item; \
    } else { \
        int count = mx_list_count(list) \
        MX_ASSERT(idx > 0 && idx <= count, "mx_list_insert() expected a range between 0 and mx_list_count()."); \
        mx_list_resize(list, count + 1); \
        memmove(&list[idx]+1, &list[idx], (count - idx) * sizeof(*list)); \
        list[idx] = item; \
    } \
} while(0)

/**
 * @brief Insert an item into the list (with copy)
 * @param list The list.
 * @param index The index to insert the item into.
 * @param item The item to insert.
 * @remarks This overload is for things that can be copied, like structs. Item must be an addressable lvalue.
 */
#define mx_list_insert_copy(list, index, item) do { \
    int idx = (index); \
    if (list == NULL) { \
        MX_ASSERT(idx == 0, "mx_list_insert() can only insert at 0 for an empty list."); \
        mx_list_resize(list, 1); \
        memcpy(&list[0], &item, sizeof(*item)); \
    } else { \
        int count = mx_list_count(list) \
        MX_ASSERT(idx > 0 && idx <= count, "mx_list_insert() expected a range between 0 and mx_list_count()."); \
        mx_list_resize(list, count + 1); \
        memmove(&list[idx]+1, &list[idx], (count - idx) * sizeof(*list)); \
        memcpy(&list[idx], &item, sizeof(*item)); \
    } \
} while(0)

/**
 * @brief Free the list, completely destroying any associated memory.
 * @param list The list to free.
 */
#define mx_list_free(list) do { \
    if (list != NULL) { \
        free(mx_list_self(list)); \
        list = NULL; \
    } \
} while(0)

#endif
