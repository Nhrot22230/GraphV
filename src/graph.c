// graph.c

#include "graph.h"

#include <stdio.h>
#include <stdlib.h>


// Function to create a new Node
struct Node* node_create(int id) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode != NULL) {
        newNode->id = id;
        newNode->w = 0;
        newNode->next = NULL;
    }
    return newNode;
}

int IsNodeHere(struct Graph *g, float posX, float posY, float radius){
    float max_Xrange = posX + radius;
    float min_Xrange = posX - radius;

    float max_Yrange = posY + radius;
    float min_Yrange = posY - radius;

    for(int id = 0; id<g->n_vertex; ++id){
        if(g->arr[id].id == -1) continue;

        float x = g->arr[id].curX;
        float y = g->arr[id].curY;

        if( (min_Xrange <= x && x <= max_Xrange) && (min_Yrange <= y && y <= max_Yrange) ) 
            return id;
    }

    return -1;
}

void node_move(struct Graph *g, int id, float posX, float posY){
    g->arr[id].posX = posX;
    g->arr[id].posY = posY;

    g->arr[id].curX = posX;
    g->arr[id].curY = posY;
}

void node_alt_state(struct Graph *g, int id){
    g->arr[id].state = (g->arr[id].state + 1) % (TOTAL_STATES + 1);
    
    if(g->arr[id].state == 0) g->arr[id].state = DEFAULT_STATE;
}


// Function to initialize a new ListNode
struct ListNode* list_init() {
    struct ListNode* newList = (struct ListNode*)malloc(sizeof(struct ListNode));
    if (newList != NULL) {
        newList->head = NULL;
        newList->length = 0;
    }
    return newList;
}

int list_hasId(struct ListNode* list, int id) {
    struct Node* current = list->head;
    while (current != NULL) {
        if (current->id == id) {
            return 1; // Found the id in the list
        }
        current = current->next;
    }
    return -1; // id not found in the list
}

// Function to add a new Node to the list
void list_addNode(struct ListNode* list, int id) {
    struct Node* newNode = node_create(id);
    if (newNode != NULL) {
        newNode->next = list->head;
        list->head = newNode;
        list->length++;
    }
}

// Function to delete a Node from the list
void list_deleteNode(struct ListNode* list, int id) {
    struct Node* current = list->head;
    struct Node* prev = NULL;

    while (current != NULL && current->id != id) {
        prev = current;
        current = current->next;
    }

    if (current != NULL) {
        if (prev != NULL) {
            prev->next = current->next;
        } else {
            list->head = current->next;
        }

        free(current);
        list->length--;
    }
}

// Function to free the memory allocated for the list
void list_free(struct ListNode* list) {
    struct Node* current = list->head;
    while (current != NULL) {
        struct Node* nextNode = current->next;
        current->next = NULL;
        free(current);
        current = nextNode;
    }
    free(list);
    list = NULL;
}

// Function to check if the list is empty
int list_empty(struct ListNode* list) {
    return (list->length == 0);
}

// Function to create a new GraphNode
struct GraphNode* graphNode_create(int id, float x, float y) {
    struct GraphNode* newNode = (struct GraphNode*)malloc(sizeof(struct GraphNode));
    if (newNode != NULL) {
        newNode->id = id;
        newNode->posX = x;
        newNode->posY = y;

        newNode->curX = x;
        newNode->curY = y;
        
        newNode->adj = list_init();
    }
    return newNode;
}

// Function to initialize a new Graph
void graph_create(struct Graph* g) {
    g->arr_visited = NULL;
    g->distances = NULL;
    g->arr = NULL;
    g->n_vertex = 0;
}

// Function to initialize a new Graph with 'n' vertices
void graph_init(struct Graph* g, int n) {
    g->arr_visited = (int *) malloc(n * sizeof(int));
    
    if(g->arr_visited == NULL){
        printf("BUY RAM xd\n");
        return;
    }

    g->arr = (struct GraphNode*)malloc(n * sizeof(struct GraphNode));
    
    if (g->arr == NULL) {
        printf("BUY MORE RAM lol.\n");
        return;
    }

    g->distances = (float *)malloc(n * sizeof(float));

    g->n_vertex = n;
    for (int i = 0; i < n; i++) {
        g->arr[i].id = NODE_UNDEF; // Indicates an uninitialized node
        g->arr[i].state = DEFAULT_STATE;
        g->arr[i].adj = list_init();
        g->arr_visited[i] = NODE_NOT_VISITED;
    }
}

// Function to add a new vertex to the graph
void graph_addVertex(struct Graph* g, int id, float x, float y) {
    if (id >= 0 && id < g->n_vertex && g->arr[id].id == NODE_UNDEF) {
        g->arr[id].id = id;
        g->arr[id].posX = x;
        g->arr[id].posY = y;
        g->arr[id].curX = x;
        g->arr[id].curY = y;
    }
}

// Function to add an edge between two vertices in the graph
void graph_addEdge(struct Graph* g, int id_src, int id_dest) {
    if(id_src == id_dest | id_src < 0 | id_dest < 0) return;

    if(list_hasId(g->arr[id_src].adj, id_dest) == 1) return;

    if (id_src >= 0 && id_src < g->n_vertex && id_dest >= 0 && id_dest < g->n_vertex) {
        list_addNode(g->arr[id_src].adj, id_dest);
        //list_addNode(g->arr[id_dest].adj, id_src);
    }
}

// Function for not connected nodes -> connect this vertex to all other vertex
void graph_connect_2_all(struct Graph *g, int id){
    if(g->arr[id].adj->length > 0) return;

    for(int i=0; i<g->n_vertex; ++i){
        if(g->arr[i].id == NODE_UNDEF || i == id) continue;

        list_addNode(g->arr[i].adj, id);
        list_addNode(g->arr[id].adj, i);
    }
}

// Function to delete an edge between two vertices in the graph
void graph_deleteEdge(struct Graph* g, int id_src, int id_dest) {
    if (id_src >= 0 && id_src < g->n_vertex && id_dest >= 0 && id_dest < g->n_vertex) {
        list_deleteNode(g->arr[id_src].adj, id_dest);
        list_deleteNode(g->arr[id_dest].adj, id_src);
    }
}

// Function to delete a vertex from the graph
void graph_deleteNode(struct Graph* g, int id) {
    g->arr[id].id = NODE_UNDEF;
    
    list_free(g->arr[id].adj);

    g->arr[id].adj = list_init();
    g->arr[id].state = DEFAULT_STATE;
    g->arr[id].posX = 0;
    g->arr[id].posY = 0;
    g->arr[id].curX = 0;
    g->arr[id].curY = 0;

    for(int i=0; i<g->n_vertex; ++i){
        if(g->arr[i].id == NODE_UNDEF) continue;

        list_deleteNode(g->arr[i].adj, id);
    }
}

// Function to print the graph
void graph_print(struct Graph* g) {
    for (int i = 0; i < g->n_vertex; i++) {
        if (g->arr[i].id == NODE_UNDEF) continue;

        printf("Vertex %d (%5.2f, %5.2f) - Adjacent: ", g->arr[i].id, g->arr[i].posX, g->arr[i].posY);
        struct Node* current = g->arr[i].adj->head;
        while (current != NULL) {
            printf("%d ", current->id);
            current = current->next;
        }
        printf("\n");
    }
}

void graph_clean(struct Graph *g){
    for(int i=0; i<g->n_vertex; ++i){
        if(g->arr[i].id == NODE_UNDEF) continue;
        
        graph_deleteNode(g, i);
        g->arr_visited[i] = NODE_NOT_VISITED;
        g->arr[i].state = DEFAULT_STATE;
    }
}

void graph_clean_visited(struct Graph *g){
    for(int id=0; id<g->n_vertex; ++id){
        if(g->arr[id].id == NODE_UNDEF) continue;

        g->arr_visited[id] = NODE_NOT_VISITED;
    }
}

// Function to free the memory allocated for the graph
void graph_free(struct Graph* g) {
    for (int i = 0; i < g->n_vertex; i++) {
        list_free(g->arr[i].adj);
    }
    free(g->arr);
    free(g->arr_visited);
    g->n_vertex = 0;
}

void graph_dfs(struct Graph* g, int id){
    if (id < 0 || id >= g->n_vertex) {
        return; // Invalid vertex
    }

    if (g->arr_visited[id] == NODE_VISITED) {
        return; // Vertex already visited
    }

    g->arr_visited[id] = NODE_VISITED; // Mark the current vertex as visited

    struct Node* current = g->arr[id].adj->head;
    while (current != NULL) {
        graph_dfs(g, current->id);
        current = current->next;
    }
}


void search_state_init(struct SearchState *state, int n_vertex) {
    state->stack = (int*)malloc(n_vertex * sizeof(int));
    state->top = NODE_UNDEF;
    state->mode = DEFAULT_SEARCH_MODE;
}

void search_state_restart(struct SearchState *state, int vertex){
    search_state_clean(state);
    if(state->mode == DFS) search_state_push(state, vertex);
    else if(state->mode == BFS) search_state_enque(state, vertex);
    else search_state_push(state, vertex);
}

void search_state_switch_mode(struct SearchState *state){
    int mode = state->mode;
    mode = (mode + 1)%(TOTAL_MODES + 1);
    if(mode == 0) mode = DEFAULT_SEARCH_MODE;

    state->mode = mode;
}

void search_state_push(struct SearchState *state, int vertex) {
    state->stack[++state->top] = vertex;
}

int search_state_pop(struct SearchState *state) {
    return state->stack[state->top--];
}

void search_state_enque(struct SearchState *state, int vertex){
    state->top = state->top + 1;
    state->stack[state->top] = vertex;
}

int search_state_deque(struct SearchState *state){
    int val = state->stack[0];

    for(int i=1; i<=state->top; ++i){
        state->stack[i-1] = state->stack[i];
    }
    
    state->stack[state->top] = 0;
    state->top = state->top - 1;
    return val;
}

int search_state_empty(struct SearchState *state) {
    return state->top == -1;
}

void search_state_clean(struct SearchState *state){
    while(state->top != -1) search_state_pop(state);
}

void search_state_print(struct SearchState *state){
    const char *s_BFS = "Breadth First Search";
    const char *s_DFS = "Depth First Search";

    printf("SearchState status:\n");
    if(state->mode == DFS) printf("   - Mode: %s\n", s_DFS);
    else if(state->mode == BFS) printf("   - Mode: %s\n", s_BFS);
    else printf("   - Mode: %s\n", s_DFS);

    printf("   - Stored Values:");
    for(int i=0; i<= state->top; ++i) printf(" %0d", state->stack[i]);
    printf("\n");
}

int dfs_step(struct Graph* g, struct SearchState *state) {
    if (search_state_empty(state)) return -1; // Search completed
    

    int current = search_state_pop(state);

    if (g->arr_visited[current] == NODE_VISITED) {
        return 0; // Already visited, move to the next step
    }

    g->arr_visited[current] = NODE_VISITED;

    struct Node* neighbor = g->arr[current].adj->head;
    while (neighbor != NULL) {
        if (g->arr_visited[neighbor->id] == NODE_NOT_VISITED) {
            search_state_push(state, neighbor->id);
        }
        neighbor = neighbor->next;
    }

    return 1; // Move to the next step
}


int bfs_step(struct Graph *g, struct SearchState *state){
    if(search_state_empty(state)) return -1;

    int cur = search_state_deque(state);

    if(g->arr_visited[cur] == NODE_VISITED) return 0;

    g->arr_visited[cur] = NODE_VISITED;

    struct Node *neighbor = g->arr[cur].adj->head;
    while(neighbor != NULL){
        if(
                g->arr_visited[neighbor->id] != NODE_VISITED & 
                g->arr_visited[neighbor->id] != NODE_ENQUED
                ){
                g->arr_visited[neighbor->id] = NODE_ENQUED;
                search_state_enque(state, neighbor->id);
        }
        
        neighbor = neighbor->next;
    }

    return 1;
}


int search_step(struct Graph *g, struct SearchState *state){
    if(state->mode == DFS) return dfs_step(g, state);
    else if(state->mode == BFS) return bfs_step(g, state);
    
    return dfs_step(g, state);
}
