/* Generic helper macros for a doubly linked list.
*/
#ifndef GENERIC_LIST_H_
#define GENERIC_LIST_H_

/* LICENSE: FreeBSD License
Copyright (C) 2006-2015 Jay Satiro <raysatiro@yahoo.com>
All rights reserved.

https://github.com/jay/generic_list

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Generic helper macros for a doubly linked list.

DECLARE_NODE_MEMBERS
Declare the node members (prev, next, parent).

DECLARE_LIST_MEMBERS
Declare the list members (head, tail, count).

ZERO_OUT_NODE_MEMBERS
Zero out the node members (prev, next, parent).

ZERO_OUT_LIST_MEMBERS
Zero out the list members (head, tail, count).

UNLINK_NODE
Unlink a node from its list.

LINK_NODE_FIRST
Link a node to a list and position it as the head node.

LINK_NODE_LAST
Link a node to a list and position it as the tail node.

LINK_NODE_BEFORE
Link a node to a list and position it before another node already in the list.

LINK_NODE_AFTER
Link a node to a list and position it after another node already in the list.

---

The input parameters for the macros are evaluated multiple times because they
are generic function-like macros. The parameters must not have side effects.

For whichever is defined last --your node or your list-- a forward declaration
for it must come before both.

If you are not zeroing your struct before using you must zero the node/list
members before using by calling ZERO_OUT_NODE_MEMBERS/ZERO_OUT_LIST_MEMBERS.

For an example refer to example.c
*/


#ifndef MS_INLINE_PRAGMA
#if (_MSC_VER >= 1300)
#define MS_INLINE_PRAGMA(x)   __pragma(x)
#else
#define MS_INLINE_PRAGMA(x)
#endif
#endif


/* DECLARE_NODE_MEMBERS
Declare the node members (prev, next, parent).

Use this declaration in your node struct. Do not declare in your list struct.

This macro adds the following members:
prev : Pointer to the previous node in the list. NULL if none.
next : Pointer to the next node in the list. NULL if none.
parent : Pointer to the list object. NULL if none.

[in] 'node_tag' : Tag name of your node struct.
[in] 'list_tag' : Tag name of your list struct.
*/
#define DECLARE_NODE_MEMBERS(node_tag, list_tag)   \
    struct node_tag *prev, *next; struct list_tag *parent


/* DECLARE_LIST_MEMBERS
Declare the list members (head, tail, count).

Use this declaration in your list struct. Do not declare in your node struct.

This macro adds the following members:
head : Pointer to the first node in the list. NULL if none.
tail : Pointer to the last node in the list. NULL if none.
count : The number of nodes in the list. 0 if none.

[in] 'node_tag' : Tag name of your node struct.
*/
#define DECLARE_LIST_MEMBERS(node_tag)   \
    struct node_tag *head, *tail; size_t count


/* ZERO_OUT_NODE_MEMBERS
Zero out the node members (prev, next, parent).

The input parameters below are evaluated multiple times because this is a
generic function-like macro. The parameters must not have side effects.

[in] 'node' : Pointer to a node.
*/
#define ZERO_OUT_NODE_MEMBERS(node)   \
MS_INLINE_PRAGMA(warning(push)) \
MS_INLINE_PRAGMA(warning(disable:4127)) \
do { \
    if((node)) { \
        (node)->parent = NULL; \
        (node)->prev = (node)->next = NULL; \
    } \
} while(0) \
MS_INLINE_PRAGMA(warning(pop))



/* ZERO_OUT_LIST_MEMBERS
Zero out the list members (head, tail, count).

The input parameters below are evaluated multiple times because this is a
generic function-like macro. The parameters must not have side effects.

[in] 'list' : Pointer to a list.
*/
#define ZERO_OUT_LIST_MEMBERS(list)   \
MS_INLINE_PRAGMA(warning(push)) \
MS_INLINE_PRAGMA(warning(disable:4127)) \
do { \
    if((list)) { \
        (list)->count = 0; \
        (list)->head = (list)->tail = NULL; \
    } \
} while(0) \
MS_INLINE_PRAGMA(warning(pop))


/* UNLINK_NODE
Unlink a node from its list.

The input parameters below are evaluated multiple times because this is a
generic function-like macro. The parameters must not have side effects.

[in] 'node' : Pointer to a node.
*/
#define UNLINK_NODE(node)   \
MS_INLINE_PRAGMA(warning(push)) \
MS_INLINE_PRAGMA(warning(disable:4127)) \
do { \
    if((node)) { \
        if((node)->parent) { \
            if((node)->parent->head == (node)) { \
                (node)->parent->head = (node)->next; \
            } \
            if((node)->parent->tail == (node)) { \
                (node)->parent->tail = (node)->prev; \
            } \
            --(node)->parent->count; \
            (node)->parent = NULL; \
        } \
        if((node)->prev) { \
            (node)->prev->next = (node)->next; \
        } \
        if((node)->next) { \
            (node)->next->prev = (node)->prev; \
        } \
        (node)->prev = (node)->next = NULL; \
    } \
} while(0) \
MS_INLINE_PRAGMA(warning(pop))


/* LINK_NODE_FIRST
Link a node to a list and position it as the head node.

If 'node' is already part of a list it is unlinked from that list before being
linked to 'list'.

If 'list' has a node count equal to the maximum value of size_t then no action
is taken. If 'node' is already part of a list it is not unlinked.

The input parameters below are evaluated multiple times because this is a
generic function-like macro. The parameters must not have side effects.

[in] 'node' : Pointer to a node.
[in] 'list' : Pointer to a list.
*/
#define LINK_NODE_FIRST(node, list)   \
MS_INLINE_PRAGMA(warning(push)) \
MS_INLINE_PRAGMA(warning(disable:4127)) \
do { \
    if((node) && (list) && ((node) != (list)->head) \
        && ((list)->count != (size_t)-1)) \
    { \
        UNLINK_NODE((node)); \
        (node)->next = (list)->head; \
        (node)->prev = NULL; \
        if(!(list)->tail) { \
            (list)->tail = (node); \
        } \
        if((list)->head) { \
            (list)->head->prev = (node); \
        } \
        (list)->head = (node); \
        ++(list)->count; \
        (node)->parent = (list); \
    } \
} while(0) \
MS_INLINE_PRAGMA(warning(pop))


/* LINK_NODE_LAST
Link a node to a list and position it as the tail node.

If 'node' is already part of a list it is unlinked from that list before being
linked to 'list'.

If 'list' has a node count equal to the maximum value of size_t then no action
is taken. If 'node' is already part of a list it is not unlinked.

The input parameters below are evaluated multiple times because this is a
generic function-like macro. The parameters must not have side effects.

[in] 'node' : Pointer to a node.
[in] 'list' : Pointer to a list.
*/
#define LINK_NODE_LAST(node, list)   \
MS_INLINE_PRAGMA(warning(push)) \
MS_INLINE_PRAGMA(warning(disable:4127)) \
do { \
    if((node) && (list) && ((node) != (list)->tail) \
        && ((list)->count != (size_t)-1)) \
    { \
        UNLINK_NODE((node)); \
        (node)->next = NULL; \
        (node)->prev = (list)->tail; \
        if(!(list)->head) { \
            (list)->head = (node); \
        } \
        if((list)->tail) { \
            (list)->tail->next = (node); \
        } \
        (list)->tail = (node); \
        ++(list)->count; \
        (node)->parent = (list); \
    } \
} while(0) \
MS_INLINE_PRAGMA(warning(pop))


/* LINK_NODE_BEFORE
Link a node to a list and position it before another node already in the list.

If 'node' is already part of a list it is unlinked from that list before being
linked to the list that 'position_node' is a part of.

If 'position_node' is not part of a list (position_node->parent == NULL) it is
still linked with node.

If 'position_node' is part of a list that has a node count equal to the maximum
value of size_t then no action is taken. If 'node' is already part of a list it
is not unlinked.

The input parameters below are evaluated multiple times because this is a
generic function-like macro. The parameters must not have side effects.

[in] 'node' : Pointer to a node.
[in] 'position_node' : Pointer to a node that's part of a list.
*/
#define LINK_NODE_BEFORE(node, position_node)   \
MS_INLINE_PRAGMA(warning(push)) \
MS_INLINE_PRAGMA(warning(disable:4127)) \
do { \
    if((node) && (position_node) && ((node) != (position_node)) \
        && (!(node)->parent || ((node)->parent->count != (size_t)-1))) \
    { \
        UNLINK_NODE((node)); \
        (node)->next = (position_node); \
        (node)->prev = (position_node)->prev; \
        if((position_node)->prev) { \
            (position_node)->prev->next = (node); \
        } \
        (position_node)->prev = (node); \
        if((position_node)->parent) { \
            if((position_node)->parent->head == (position_node)) { \
                (position_node)->parent->head = (node); \
            } \
            ++(position_node)->parent->count; \
        } \
        (node)->parent = (position_node)->parent; \
    } \
} while(0) \
MS_INLINE_PRAGMA(warning(pop))


/* LINK_NODE_AFTER
Link a node to a list and position it after another node already in the list.

If 'node' is already part of a list it is unlinked from that list before being
linked to the list that 'position_node' is a part of.

If 'position_node' is not part of a list (position_node->parent == NULL) it is
still linked with node.

If 'position_node' is part of a list that has a node count equal to the maximum
value of size_t then no action is taken. If 'node' is already part of a list it
is not unlinked.

The input parameters below are evaluated multiple times because this is a
generic function-like macro. The parameters must not have side effects.

[in] 'node' : Pointer to a node.
[in] 'position_node' : Pointer to a node that's part of a list.
*/
#define LINK_NODE_AFTER(node, position_node)   \
MS_INLINE_PRAGMA(warning(push)) \
MS_INLINE_PRAGMA(warning(disable:4127)) \
do { \
    if((node) && (position_node) && ((node) != (position_node)) \
        && (!(node)->parent || ((node)->parent->count != (size_t)-1))) \
    { \
        UNLINK_NODE((node)); \
        (node)->next = (position_node)->next; \
        (node)->prev = (position_node); \
        if((position_node)->next) { \
            (position_node)->next->prev = (node); \
        } \
        (position_node)->next = (node); \
        if((position_node)->parent) { \
            if((position_node)->parent->tail == (position_node)) { \
                (position_node)->parent->tail = (node); \
            } \
            ++(position_node)->parent->count; \
        } \
        (node)->parent = (position_node)->parent; \
    } \
} while(0) \
MS_INLINE_PRAGMA(warning(pop))

#endif /* GENERIC_LIST_H_ */
