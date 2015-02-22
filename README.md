generic_list
============

generic_list - Generic helper macros for a doubly linked list in C

generic_list.h is a standalone file that can be included in any C project. It can also be included in any C++ project, however it is antiquated by std::list in the C++ Standard Library.

Two structs --a list struct and a node struct-- are required to make use of the macros. The list struct must have `head` and `tail` pointers to the first and last node, respectively, and an unsigned `count` of the number of nodes in the list. The node struct must have `prev` and `next` pointers to the previous and next node, respectively, and a `parent` pointer that points to the list. You can declare the required members using the helper DECLARE macros:
```c
DECLARE_LIST_MEMBERS(node_tag);
DECLARE_NODE_MEMBERS(node_tag, list_tag);
```
Use `DECLARE_LIST_MEMBERS` in your list struct definition and `DECLARE_NODE_MEMBERS` in your node struct definition. `node_tag` is the tag name of your node struct and `list_tag` is the tag name of your list struct. For example:
```c
struct car_list;
struct car_node {
    /* Declare the node members (prev, next, parent). */
    DECLARE_NODE_MEMBERS(car_node, car_list);
    char *name, etc;
};
struct car_list {
    /* Declare the list members (head, tail, count). */
    DECLARE_LIST_MEMBERS(car_node);
    int var1, var2, etc;
};
```
You would then write functions specific to your type and in them you may call the UNLINK/LINK macros to unlink/link a node to a list. A compilable example is in [example.c](https://github.com/jay/generic_list/blob/master/example.c).

List of macros
--------------

#### DECLARE_NODE_MEMBERS
Declare the node members (prev, next, parent).

#### DECLARE_LIST_MEMBERS
Declare the list members (head, tail, count).

#### ZERO_OUT_NODE_MEMBERS
Zero out the node members (prev, next, parent).

#### ZERO_OUT_LIST_MEMBERS
Zero out the list members (head, tail, count).

#### UNLINK_NODE
Unlink a node from its list.

#### LINK_NODE_FIRST
Link a node to a list and position it as the head node.

#### LINK_NODE_LAST
Link a node to a list and position it as the tail node.

#### LINK_NODE_BEFORE
Link a node to a list and position it before another node already in the list.

#### LINK_NODE_AFTER
Link a node to a list and position it after another node already in the list.

Important
---------

The input parameters for the macros are evaluated multiple times because they are generic function-like macros. The parameters must not have side effects or access the list.

***WRONG:*** `UNLINK_NODE(node->next);`  
The list is accessed. When a node is linked/unlinked `next`/`prev`/`parent` may be changed for other nodes and `head`/`tail`/`count` may be changed for the list. `node->next` is evaluated multiple times and may not have the same value at a later statement in the macro.  
***RIGHT:*** `nodetype *temp = node->next; UNLINK_NODE(temp);`

***WRONG:*** `UNLINK_NODE(Function());`  
Function() will be called multiple times in the macro. Also it may access the list or have some side effect.  
***RIGHT:*** `nodetype *temp = Function(); UNLINK_NODE(temp);`

Documentation
-------------

Documentation is in [generic_list.h](https://github.com/jay/generic_list/blob/master/generic_list.h). General information is in the comment block below the license. Each function-like macro is documented in the comment block above its definition.

Other
-----

### Stresstest

Although the algorithm for a doubly linked list is very simple I added a stress test to ensure my implementation was correct. The stress test randomly links and unlinks new and existing nodes in random order and sanity checks each action.

The stresstest has completed hundreds of millions of iterations without a failure. If an iteration of the stress test fails two cpu beeps (\a\a) are sent, the state of the random number generator is saved, the error output is saved and sent to stdout and then `__debugbreak()` is called. The stress test can be restarted using the failed iteration by passing the state of the random number generator at that time (error_DATE_TIME_state_iteration.txt) as an argument to stresstest.

My meta info for debugging may or may not be found in breakpoints.xml, depending on the commit.

### Circularly linked list

The Linux kernel has a great circularly linked list implementation in C. [Linux Kernel Linked List Explained](http://isis.poly.edu/kulesh/stuff/src/klist/).

### License

generic_list.h is licensed under the [FreeBSD license](http://en.wikipedia.org/wiki/BSD_licenses#2-clause) which also referred to as the 2-clause BSD license or simplified BSD license. It is a non-restrictive GPL-compatible free software license. It is similar to the MIT license and is closed source permissive.

stresstest is licensed differently, it must remain open source if redistributed. It is also free software and it is licensed under the [GNU General Public License version 3 (GPLv3)](http://www.gnu.org/copyleft/gpl.html), a license that will keep it free. The source code for stresstest cannot be used in proprietary software, but you can for example execute a free software application from a proprietary software application. **In any case please review the GPLv3 license, which is designed to protect freedom, not take it away.**

You may not remove my copyright or the copyright of any contributors under the terms of either license. 

### Source

The source can be found on [GitHub](https://github.com/jay/generic_list). Since you're reading this maybe you're already there?

### Send me any questions you have

Jay Satiro `<raysatiro$at$yahoo{}com>` and put generic_list in the subject.
