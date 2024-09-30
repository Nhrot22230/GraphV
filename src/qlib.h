#ifndef QLIB_H
#define QLIB_H

#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph.h"

#include "../raylib/src/raylib.h"

struct StackId{
    int st[10];
    int top;
};

enum{
    SOUND_NOT_DEF = -1,
    SOUND_NODE = 0,
    SOUND_EDGE = 1,
    SOUND_MOVE = 2,
    SOUND_CLEAN = 3,
    SOUND_SEARCH_STEP = 4,
    SOUND_SEARCH_SWITCH = 5,
    TOTAL_SOUNDS = 6
};

enum{
    NOTE_G4 = 0,
    NOTE_A4 = 1,
    NOTE_B4 = 2,
    NOTE_C5 = 3,
    NOTE_D5 = 4,
    NOTE_E5 = 5,
    NOTE_F5 = 6
};


typedef struct {
    Vector2 position;
    Color color;
    float speed;
} Snowflake;

typedef struct {
    struct Graph g;
    struct SearchState state;
    struct ListNode list_id;
    struct StackId node_stack;
    int src_node;

    Sound sfx[10];

    Snowflake snowfall[200];

    Sound notas[10];
    Sound navidad[30];
    int nav_cur;
    int nav_len;
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


void StackId_init(struct StackId *stack);
void StackId_push(struct StackId *stack, int id);
int StackId_pop(struct StackId *stack);
void StackId_update_graph(struct StackId *stack, struct Graph *g);
void StackId_print(struct StackId *stack);
void StackId_clean(struct StackId *stack);

void graph_edges_render(struct Graph *g, float radius);
void graph_extra_render(struct Graph *g);
void graph_nodes_render(struct Graph *g, float radius);
void graph_draw(struct Graph *g);
void search_state_draw(struct SearchState *state, int src_node);

// State_functions

void state_addNode(struct Graph *g, struct ListNode *list);
void state_delNode(struct Graph *g, struct StackId *stack, struct ListNode *list);
void state_restart(Plug *p);
void state_restart_search(Plug *p);
void state_print(Plug *p);
// Snow Functions

void InitializeSnowfall(Snowflake snowfall[], int screenWidth, int screenHeight);
void UpdateSnowfall(Snowflake snowfall[], int screenWidth, int screenHeight);
void DrawSnowfall(Snowflake snowfall[]);

#endif //QLIB_H
