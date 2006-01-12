/* 
*   include/kernel/liste.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

/* Copyright (C) 2001  David Decotigny 

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
       
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
       
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA.
*/

#ifndef _LIST_H_
#define _LIST_H_

/**
 * @file list.h
 *
 * Circular doubly-linked lists implementation entirely based on C
 * macros
 */


/* *_named are used when next and prev links are not exactly next
   and prev. For instance when we have next_in_team, prev_in_team,
   prev_global and next_global */

#define LIST_INIT_NAMED(list,prev,next) \
  ((list) = NULL)

#define LIST_SINGLETON_NAMED(list,item,prev,next) ({ \
  (item)->next = (item)->prev = (item); \
  (list) = (item); \
})

#define LIST_IS_EMPTY_NAMED(list,prev,next) \
  ((list) == NULL)

#define LIST_IS_SINGLETON_NAMED(list,prev,next) \
  ( ((list) != NULL) && ((list)->prev == (list)->next) && ((list) == (list)->next) )

#define LIST_GET_HEAD_NAMED(list,prev,next) \
  (list)

#define LIST_GET_TAIL_NAMED(list,prev,next) \
  ((list)?((list)->prev):NULL)

/* Internal macro : insert before the head == insert at tail */
#define __LIST_INSERT_ATLEFT_NAMED(before_this,item,prev,next) ({ \
   (before_this)->prev->next = (item); \
   (item)->prev = (before_this)->prev; \
   (before_this)->prev = (item); \
   (item)->next = (before_this); \
})

/* @note Before_this and item are expected to be valid ! */
#define LIST_INSERT_BEFORE_NAMED(list,before_this,item,prev,next) ({ \
   __LIST_INSERT_ATLEFT_NAMED(before_this,item,prev,next); \
   if ((list) == (before_this)) (list) = (item); \
})    

/** @note After_this and item are expected to be valid ! */
#define LIST_INSERT_AFTER_NAMED(list,after_this,item,prev,next) ({ \
   (after_this)->next->prev = (item); \
   (item)->next = (after_this)->next; \
   (after_this)->next = (item); \
   (item)->prev = (after_this); \
})

#define LIST_ADD_HEAD_NAMED(list,item,prev,next) ({ \
  if (list) \
    LIST_INSERT_BEFORE_NAMED(list,list,item,prev,next); \
  else \
    LIST_SINGLETON_NAMED(list,item,prev,next); \
  (list) = (item); \
})

#define LIST_ADD_TAIL_NAMED(list,item,prev,next) ({ \
  if (list) \
    __LIST_INSERT_ATLEFT_NAMED(list,item,prev,next); \
  else \
    LIST_SINGLETON_NAMED(list,item,prev,next); \
})

/** @note NO check whether item really is in list ! */
#define LIST_DELETE_NAMED(list,item,prev,next) ({ \
  if ( ((item)->next == (item)) && ((item)->prev == (item)) ) \
    (item)->next = (item)->prev = (list) = NULL; \
  else { \
    (item)->prev->next = (item)->next; \
    (item)->next->prev = (item)->prev; \
    if ((item) == (list)) (list) = (item)->next; \
    (item)->prev = (item)->next = NULL; \
  } \
})

#define LIST_POP_HEAD_NAMED(list,prev,next) ({ \
  typeof(list) __ret_elt = (list); \
  LIST_DELETE_NAMED(list,__ret_elt,prev,next); \
  __ret_elt; })

/** Loop statement that iterates through all of its elements, from
    head to tail */
#define LIST_FOREACH_FORWARD_NAMED(list,iterator,nb_elements,prev,next) \
        for (nb_elements=0, (iterator) = (list) ; \
             (iterator) && (!nb_elements || ((iterator) != (list))) ; \
             nb_elements++, (iterator) = (iterator)->next )

/** Loop statement that iterates through all of its elements, from
    tail back to head */
#define LIST_FOREACH_BACKWARD_NAMED(list,iterator,nb_elements,prev,next) \
        for (nb_elements=0, (iterator) = LIST_GET_TAIL_NAMED(list,prev,next) ; \
             (iterator) && (!nb_elements || \
               ((iterator) != LIST_GET_TAIL_NAMED(list,prev,next))) ; \
             nb_elements++, (iterator) = (iterator)->prev )

#define LIST_FOREACH_NAMED LIST_FOREACH_FORWARD_NAMED

/** True when we exitted early from the foreach loop (ie break) */
#define LIST_FOREACH_EARLY_BREAK(list,iterator,nb_elements) \
  ((list) && ( \
    ((list) != (iterator)) || \
    ( ((list) == (iterator)) && (nb_elements == 0)) ))

/** Loop statement that also removes the item at each iteration. The
    body of the loop is allowed to delete the iterator element from
    memory. */
#define LIST_COLLAPSE_NAMED(list,iterator,prev,next) \
        for ( ; ({ ((iterator) = (list)) ; \
                   if (list) LIST_DELETE_NAMED(list,iterator,prev,next) ; \
                   (iterator); }) ; )


/*
 * the same macros : assume that the prev and next fields are really
 * named "prev" and "next"
 */

#define LIST_INIT(list) \
  LIST_INIT_NAMED(list,prev,next)

#define LIST_SINGLETON(list,item) \
  list_singleton_named(list,item,prev,next)

#define LIST_IS_EMPTY(list) \
  LIST_IS_EMPTY_NAMED(list,prev,next)

#define LIST_IS_SINGLETON(list) \
  LIST_IS_SINGLETON_NAMED(list,prev,next)

#define LIST_GET_HEAD(list) \
  LIST_GET_HEAD_NAMED(list,prev,next) \

#define LIST_GET_TAIL(list) \
  LIST_GET_TAIL_NAMED(list,prev,next) \

/* @note Before_this and item are expected to be valid ! */
#define LIST_INSERT_AFTER(list,after_this,item) \
  LIST_INSERT_AFTER_NAMED(list,after_this,item,prev,next)

/* @note After_this and item are expected to be valid ! */
#define LIST_INSERT_BEFORE(list,before_this,item) \
  LIST_INSERT_BEFORE_NAMED(list,before_this,item,prev,next)

#define LIST_ADD_HEAD(list,item) \
  LIST_ADD_HEAD_NAMED(list,item,prev,next)

#define LIST_ADD_TAIL(list,item) \
  LIST_ADD_TAIL_NAMED(list,item,prev,next)

/* @note NO check whether item really is in list ! */
#define LIST_DELETE(list,item) \
  LIST_DELETE_NAMED(list,item,prev,next)

#define LIST_POP_HEAD(list) \
  LIST_POP_HEAD_NAMED(list,prev,next)

#define LIST_FOREACH_FORWARD(list,iterator,nb_elements) \
  LIST_FOREACH_FORWARD_NAMED(list,iterator,nb_elements,prev,next)

#define LIST_FOREACH_BACKWARD(list,iterator,nb_elements) \
  LIST_FOREACH_BACKWARD_NAMED(list,iterator,nb_elements,prev,next)

#define LIST_FOREACH LIST_FOREACH_FORWARD

#define LIST_COLLAPSE(list,iterator) \
  LIST_COLLAPSE_NAMED(list,iterator,prev,next)

#endif 
