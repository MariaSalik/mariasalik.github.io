#ifndef CISJ_H
#define CISJ_H

#include <stdio.h>

#define POW_2(num) (1 << (num))
#define VALID_J(j, s) ((POW_2(s - 1)) >= j)

typedef struct node_set {
    int *nodes;
    ssize_t size;
    ssize_t offset;
} node_set;

node_set *cis(int i, int s);
void set_free(node_set *nodes);

#endif
