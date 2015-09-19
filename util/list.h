#ifndef LIST_H_included
#define LIST_H_included

/**
 * Remove an element from list.
 *
 * \param elem element to remove.
 */
#define list_remove(elem)			\
do {						\
   (elem)->next->prev = (elem)->prev;		\
   (elem)->prev->next = (elem)->next;		\
} while (0)

/**
 * Insert an element to the list head.
 *
 * \param list list.
 * \param elem element to insert.
 */
#define list_prepend(list, elem)		\
do {						\
   (elem)->prev = list;				\
   (elem)->next = (list)->next;			\
   (list)->next->prev = elem;			\
   (list)->next = elem;				\
} while(0)

/**
 * Insert an element to the list tail.
 *
 * \param list list.
 * \param elem element to insert.
 */
#define list_append(list, elem)			\
do {						\
   (elem)->next = list;				\
   (elem)->prev = (list)->prev;			\
   (list)->prev->next = elem;			\
   (list)->prev = elem;				\
} while(0)

/**
 * Make a empty list empty.
 *
 * \param sentinal list (sentinal element).
 */
#define list_create(sentinal)			\
do {						\
   (sentinal)->next = sentinal;			\
   (sentinal)->prev = sentinal;			\
} while (0)

/**
 * Get list first element.
 *
 * \param list list.
 *
 * \return pointer to first element.
 */
#define list_first(list)       ((list)->next)

/**
 * Get list last element.
 *
 * \param list list.
 *
 * \return pointer to last element.
 */
#define list_last(list)        ((list)->prev)

/**
 * Get next element.
 *
 * \param elem element.
 *
 * \return pointer to next element.
 */
#define list_next(elem)        ((elem)->next)

/**
 * Get previous element.
 *
 * \param elem element.
 *
 * \return pointer to previous element.
 */
#define list_prev(elem)        ((elem)->prev)

/**
 * Test whether element is at end of the list.
 * 
 * \param list list.
 * \param elem element.
 * 
 * \return non-zero if element is at end of list, or zero otherwise.
 */
#define list_at_end(list, elem) ((elem) == (list))

/**
 * Test if a list is empty.
 * 
 * \param list list.
 * 
 * \return non-zero if list empty, or zero otherwise.
 */
#define list_is_empty(list)    ((list)->next == (list))

/**
 * Walk through the elements of a list.
 *
 * \param ptr pointer to the current element.
 * \param list list.
 *
 * \note It should be followed by a { } block or a single statement, as in a \c
 * for loop.
 */
#define list_foreach(ptr, list)\
        for (ptr=(list)->next; ptr!=list;  ptr=(ptr)->next)

/**
 * Walk through the elements of a list.
 *
 * Same as #foreach but lets you unlink the current value during a list
 * traversal.  Useful for freeing a list, element by element.
 * 
 * \param ptr pointer to the current element.
 * \param t temporary pointer.
 * \param list list.
 *
 * \note It should be followed by a { } block or a single statement, as in a \c
 * for loop.
 */
#define list_foreach_s(ptr, t, list)\
        for (ptr=(list)->next,t=(ptr)->next; list != ptr; ptr=t, t=(t)->next)

/**
 * Walk backwards through the elements of a list.
 *
 * \param ptr pointer to the current element.
 * \param list list.
 *
 * \note It should be followed by a { } block or a single statement, as in a \c
 * for loop.
 */
#define list_forback(ptr, list)\
        for (ptr=(list)->prev; ptr!=list;  ptr=(ptr)->prev)

/**
 * Walk backwards through the elements of a list.
 *
 * Same as #forback but lets you unlink the current value during a list
 * traversal.  Useful for freeing a list, element by element.
 * 
 * \param ptr pointer to the current element.
 * \param t temporary pointer.
 * \param list list.
 *
 * \note It should be followed by a { } block or a single statement, as in a \c
 * for loop.
 */
#define list_forback_s(ptr, t, list)\
        for (ptr=(list)->prev,t=(ptr)->prev; list != ptr; ptr=t, t=(t)->prev)

#endif
