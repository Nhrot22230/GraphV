#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"

#include <raylib.h>

typedef struct {
    struct Graph g;
    struct SearchState state;
} Plug;


#define LIST_FUNC \
    X(state_init, void, void) \
    X(state_pre_reload, void *, void) \
    X(state_post_reload, void, void *) \
    X(state_update, void, void)  


#define X(name, ret, ...) typedef ret (name##_t)(__VA_ARGS__);
LIST_FUNC
#undef X
