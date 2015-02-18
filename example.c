/* LICENSE: FreeBSD License
Copyright (C) 2015 Jay Satiro <raysatiro@yahoo.com>
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

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "generic_list.h"

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

void CarListAppend(const char *name, struct car_list *list) {
    struct car_node *node = NULL;
    if(!name || !list) {
        return;
    }
    node = calloc(1, sizeof(*node));
    if(!node) {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    /* If you don't zero out the node struct you must use
    ZERO_OUT_NODE_MEMBERS(node);
    */
    node->name = strdup(name);
    if(!node->name) {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    LINK_NODE_LAST(node, list);
    return;
}

void CarListShow(struct car_list *list) {
    struct car_node *node = NULL;
    if(!list) {
        return;
    }
    printf("Cars:\n-----\n");
    for(node = list->head; node; node = node->next) {
        printf("%s\n", node->name ? node->name : "<empty>");
    }
}

void CarNodeFree(struct car_node *node) {
    if(!node) {
        return;
    }
    UNLINK_NODE(node);
    free(node->name);
    free(node);
    return;
}

void CarListFree(struct car_list *list) {
    struct car_node *node = NULL;
    if(!list) {
        return;
    }
    for(node = list->head; node; ) {
        struct car_node *temp = node->next;
        CarNodeFree(node);
        node = temp;
    }
    free(list);
    return;
}

int main(int argc, char *argv[]) {
    struct car_list *cars = NULL;
    (void)argc;
    (void)argv;
    cars = calloc(1, sizeof(*cars));
    if(!cars) {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    /* If you don't zero out the list struct you must use
    ZERO_OUT_LIST_MEMBERS(cars);
    */
    CarListAppend("Jack", cars);
    CarListAppend("Dusty", cars);
    CarListAppend("Harry", cars);
    CarListShow(cars);
    CarListFree(cars);
    return 0;
}
