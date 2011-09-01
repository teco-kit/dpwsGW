/* WS4D-gSOAP - Implementation of the Devices Profile for Web Services
 * (DPWS) on top of gSOAP
 * Copyright (C) 2007 University of Rostock
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef WS4D_LIST_H_
#define WS4D_LIST_H_

/**
 * Mechanisms for doubly linked lists
 *
 *
 * @addtogroup DpwsLists Doubly Linked Lists
 * @ingroup WS4D_UTILS
 *
 * @{
 */

#include <stddef.h>


/**
 * cast a member of a structure out to the containing structure
 *
 * @param ptr pointer to the member
 * @param type of the container struct the member is embedded in
 * @param member name of the member in the structure
 */
#define ws4d_container_of(ptr, type, member)                       \
        ((type *)( (char *)ptr - offsetof(type,member)))


/**
 * structure for linked lists. To be able to add a structure to a linked list
 * the structure must contain this structure.
 *
 * @code
 * struct my_struct {
 *   [...]
 *   struct ws4d_list_node list;
 *   [...]
 * };
 * @endcode
 *
 */
struct ws4d_list_node
{
  struct ws4d_list_node *next, *prev;
};


/**
 * Macro to check a list node
 *
 *
 * @param node list node to check
 */
static INLINE void
ws4d_list_check (const struct ws4d_list_node *node)
{
  if (node && node->next && node->prev && node->next->prev
      && node->prev->next && (node->next->prev == node)
      && (node->prev->next == node))
    {
    }
  else
    {
      fprintf (stderr, "corrupt list node: (%#lx)\n", (unsigned long) node);
#ifdef DEBUG
      soap_abort_on_assert ();
#endif
    }
}


/**
 * Macro to initialize a list head at run time
 *
 * @code
 * struct ws4d_list_node head;
 *
 * WS4D_INIT_LIST(&head);
 * @endcode
 *
 * @param ptr list head to initialize
 */
#define WS4D_INIT_LIST(ptr) do { \
  (ptr)->next = (ptr); (ptr)->prev = (ptr); \
} while (0)

#ifndef DOXYGEN_SHOULD_SKIP_THIS
static INLINE void
__ws4d_list_add (struct ws4d_list_node *_new,
                 struct ws4d_list_node *prev, struct ws4d_list_node *next)
{
  next->prev = _new;
  _new->next = next;
  _new->prev = prev;
  prev->next = _new;
}
#endif

/**
 * add a new entry to list
 *
 * @code
 * struct my_struct {
 *   [...]
 *   struct ws4d_list_node list;
 *   [...]
 * };
 *
 * struct my_struct entry;
 * struct ws4d_list_node head;
 *
 * WS4D_INIT_LIST(&head);
 * ws4d_list_add(&entry, &head);
 * @endcode
 *
 * @param _new entry to add to list
 * @param head head of list
 */
static INLINE void
ws4d_list_add (struct ws4d_list_node *_new, struct ws4d_list_node *head)
{
  ws4d_list_check (head);
  __ws4d_list_add (_new, head, head->next);
}


/**
 * add a new entry to the tail of a list
 *
 * @code
 * struct my_struct {
 *   [...]
 *   struct ws4d_list_node list;
 *   [...]
 * };
 *
 * struct my_struct entry;
 * struct ws4d_list_node head;
 *
 * WS4D_INIT_LIST(&head);
 * ws4d_list_add_tail(&entry, &head);
 * @endcode
 *
 * @param _new entry to add to list
 * @param head head of list
 */
static INLINE void
ws4d_list_add_tail (struct ws4d_list_node *_new, struct ws4d_list_node *head)
{
  ws4d_list_check (head);
  __ws4d_list_add (_new, head->prev, head);
}

#ifndef DOXYGEN_SHOULD_SKIP_THIS
static INLINE void
__ws4d_list_del (struct ws4d_list_node *prev, struct ws4d_list_node *next)
{
  /* Added this to prevent seg faults. (SF) */
  if (next != NULL)
    {
      next->prev = prev;
    }

  if (prev != NULL)
    {
      prev->next = next;
    }
}
#endif


/**
 * delete an entry from a list
 *
 * @code
 * struct my_struct {
 *   [...]
 *   struct ws4d_list_node list;
 *   [...]
 * };
 *
 * struct my_struct entry;
 * struct ws4d_list_node head;
 *
 * WS4D_INIT_LIST(&head);
 * ws4d_list_add(&entry, &head);
 *
 * ws4d_list_del(&entry);
 * @endcode
 *
 * @param entry entry to delete from list
 */
static INLINE void
ws4d_list_del (struct ws4d_list_node *entry)
{
  ws4d_list_check (entry);
  __ws4d_list_del (entry->prev, entry->next);
  WS4D_INIT_LIST (entry);
}


/**
 * move an entry to another list
 *
 * @code
 * struct my_struct {
 *   [...]
 *   struct ws4d_list_node list;
 *   [...]
 * };
 *
 * struct my_struct entry;
 * struct ws4d_list_node head1, head2;
 *
 * WS4D_INIT_LIST(&head1);
 * WS4D_INIT_LIST(&head2);
 * ws4d_list_add(&entry, &head1);
 *
 * ws4d_list_move(&entry, &head2);
 * @endcode
 *
 * @param entry entry to move to other list
 * @param head head of list
 */
static INLINE void
ws4d_list_move (struct ws4d_list_node *entry, struct ws4d_list_node *head)
{
  ws4d_list_check (entry);
  __ws4d_list_del (entry->prev, entry->next);
  ws4d_list_add (entry, head);
}


/**
 * move an entry to the tail of another list
 *
 * @code
 * struct my_struct {
 *   [...]
 *   struct ws4d_list_node list;
 *   [...]
 * };
 *
 * struct my_struct entry;
 * struct ws4d_list_node head1, head2;
 *
 * WS4D_INIT_LIST(&head1);
 * WS4D_INIT_LIST(&head2);
 * ws4d_list_add(&entry, &head1);
 *
 * ws4d_list_move_tail(&entry, &head2);
 * @endcode
 *
 * @param entry entry to move to other list
 * @param head head of list
 */
static INLINE void
ws4d_list_move_tail (struct ws4d_list_node *entry,
                     struct ws4d_list_node *head)
{
  ws4d_list_check (entry);
  __ws4d_list_del (entry->prev, entry->next);
  ws4d_list_add_tail (entry, head);
}


/**
 * tests whether a list is empty or not
 *
 * @param head head of list
 * @returns
 */
static INLINE int
ws4d_list_empty (const struct ws4d_list_node *head)
{
  struct ws4d_list_node *next = head->next;
  ws4d_list_check (head);
  return (next == head) && (next == head->prev);
}


/**
 * get the struct for this entry. This macro is used to get the structure of
 * an entry from its ws4d_list_node element.
 *
 * @code
 * struct my_struct {
 *   [...]
 *   struct ws4d_list_node list;
 *   [...]
 * };
 *
 * struct my_struct elem, *elem_ptr = NULL;
 * struct ws4d_list_node *list_ptr;
 *
 * list_ptr = &(elem.list);
 * elem_ptr = ws4d_list_entry(list_ptr, struct mystruct, list);
 * @endcode
 *
 * @param ptr pointer to the ws4d_list_node element of the structure
 * @param type type of the structure that holds the ws4d_list_node element
 * @param member name of the ws4d_list_node element in structure
 */
#define ws4d_list_entry(ptr, type, member) \
  ws4d_container_of(ptr, type, member)


/**
 * iterate over list of given type
 *
 * @code
 * struct my_struct {
 *   int val;
 *   struct ws4d_list_node list;
 *   [...]
 * };
 *
 * struct my_struct elem1, elem2, *pos = NULL, *next = NULL;
 * struct ws4d_list_node *head;
 *
 * WS4D_INIT_LIST(&head);
 * elem1.val = 1;
 * elem2.val = 2;
 *
 * ws4d_list_add(&elem1.list, &head);
 * ws4d_list_add_tail(&elem2.list, &head);
 *
 * ws4d_list_foreach(pos, next, &head, struct my_struct, list)
 * {
 *   printf("my_struct.val= %d", pos->val);
 * }
 * @endcode
 *
 * @param pos ws4d_list_node element pointer as iteration variable
 * @param n ws4d_list_node element pointer to remember last entry
 * @param head head of list
 * @param type type of the structure that holds the ws4d_list_node element
 * @param member name of the ws4d_list_node element in structure
 */
#define ws4d_list_foreach(pos, n, head, type, member)      \
  for (pos = ws4d_list_entry((head)->next, type, member),  \
    n = ws4d_list_entry(pos->member.next, type, member); \
       &pos->member != (head);          \
       pos = n, n = ws4d_list_entry(n->member.next, type, member))


/**
 * prepare a pos entry for use as a start point in
 *
 * @param pos entry to use as start point
 * @param head the head of the list
 * @param type the type of pos
 * @param member the ws4d_list_node member name in pos
 */
#define ws4d_list_prepare_entry(pos, head, type, member) \
  ((pos) ? pos : ws4d_list_entry(head, type, member))


/**
 * continue to iterate over list of given type
 *
 * @code
 * struct my_struct {
 *   int val;
 *   struct ws4d_list_node list;
 *   [...]
 * };
 *
 * struct my_struct elem1, elem2, *pos = NULL, *next = NULL;
 * struct ws4d_list_node *head;
 *
 * WS4D_INIT_LIST(&head);
 * elem1.val = 1;
 * elem2.val = 2;
 *
 * ws4d_list_add(&elem1.list, &head);
 * ws4d_list_add_tail(&elem2.list, &head);
 *
 * ws4d_list_foreach(pos, next, &head, struct my_struct, list)
 * {
 *   printf("my_struct.val= %d", pos->val);
 *   if (pos->val == 1)
 *     break;
 * }
 *
 * ws4d_list_foreach_continue(pos, &head, struct my_struct, list)
 * {
 *   printf("my_struct.val= %d", pos->val);
 * }
 * @endcode
 *
 * @param pos ws4d_list_node element pointer as iteration variable
 * @param head head of list
 * @param type type of the structure that holds the ws4d_list_node element
 * @param member name of the ws4d_list_node element in structure
 */
#define ws4d_list_foreach_continue(pos, head, type, member)     \
  for (pos = ws4d_list_entry(pos->member.next, type, member);  \
       &pos->member != (head);          \
       pos = ws4d_list_entry(pos->member.next, type, member))


/** @} */

#endif /*WS4D_LIST_H_ */
