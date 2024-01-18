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
    X(state_logic, void, void) \
    X(state_update, void, void) \
    X(state_render, void, void)


#define X(name, ret, ...) typedef ret (name##_t)(__VA_ARGS__);
LIST_FUNC
#undef X

struct StackId{
    int st[10];
    int len;
};

void StackId_push(struct StackId *stack, int id);
int StackId_pop(struct StackId *stack);
void StackId_update_graph(struct StackId *stack, struct Graph *g);
void StackId_clean(struct StackId *stack);

void graph_edges_render(struct Graph *g);
void graph_extra_render(struct Graph *g);
void graph_nodes_render(struct Graph *g);
void graph_draw(struct Graph *g);
