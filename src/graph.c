// graph.c

#include "graph.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>


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

float GetDistance(float x1, float y1, float x2, float y2) {
    return sqrtf( (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2) );
}

int IsNodeHere(struct Graph* g, float posX, float posY, float radius) {
    radius = 1.25f * radius;
    float minDistance = FLT_MAX;
    int nearestNode = -1;

    for (int id = 0; id < g->n_vertex; ++id) {
        if (g->arr[id].id == -1) continue;

        float x = g->arr[id].curX;
        float y = g->arr[id].curY;

        if (x >= (posX - radius) && x <= (posX + radius) && y >= (posY - radius) && y <= (posY + radius)) {
            // Node is within the search radius
            float distance = GetDistance(posX, posY, x, y);
            if (distance < minDistance) {
                minDistance = distance;
                nearestNode = id;
            }
        }
    }

    return nearestNode;
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


struct ListNode* list_create() {
    struct ListNode* newList = (struct ListNode*)malloc(sizeof(struct ListNode));
    if (newList != NULL) {
        newList->head = NULL;
        newList->tail = NULL;
        newList->len = 0;
    }
    return newList;
}

void list_init(struct ListNode *list){
    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
}

// Function to check if a given id exists in the list
int list_hasId(struct ListNode* list, int id) {
    struct Node* current = list->head;
    while (current != NULL) {
        if (current->id == id) return 1; // Found the id in the list
        current = current->next;
    }
    return -1; // Id not found
}

// Function to add a node to the left of the list
void list_addNodeLeft(struct ListNode* list, int id) {
    struct Node* newNode = node_create(id);
    if (newNode != NULL) {
        newNode->next = list->head;
        list->head = newNode;
        if (list->tail == NULL) {
            list->tail = newNode;
        }
        list->len++;
    }
}

// Function to add a node to the right of the list
void list_addNodeRight(struct ListNode* list, int id) {
    struct Node* newNode = node_create(id);
    if (newNode != NULL) {
        if (list->tail == NULL) {
            list->head = newNode;
            list->tail = newNode;
        } else {
            list->tail->next = newNode;
            list->tail = newNode;
        }
        list->len++;
    }
}

// Function to delete a node with a given id from the list
void list_deleteNode(struct ListNode* list, int id) {
    struct Node* current = list->head;
    struct Node* prev = NULL;

    while (current != NULL) {
        if (current->id == id) {
            if (prev == NULL) {
                // If the node to be deleted is the head
                list->head = current->next;
                if (list->head == NULL) {
                    // If the list becomes empty
                    list->tail = NULL;
                }
            } else {
                // If the node to be deleted is not the head
                prev->next = current->next;
                if (prev->next == NULL) {
                    // If the node to be deleted is the tail
                    list->tail = prev;
                }
            }

            // Free the memory of the node
            free(current);
            list->len--;
            return;
        }

        prev = current;
        current = current->next;
    }
}

// Function to free the memory of the list and its nodes
void list_free(struct ListNode* list) {
    struct Node* current = list->head;
    struct Node* nextNode;

    while (current != NULL) {
        nextNode = current->next;
        free(current);
        current = nextNode;
    }

    list->head = NULL;
    list->tail = NULL;
    list->len = 0;
}

int list_empty(struct ListNode *list){
    return list->len == 0;
}

int list_get_len(struct ListNode *list){
    return list->len;
}

// Function to pop a node from the left of the list
int list_popLeft(struct ListNode* list) {
    if (list->head != NULL) {
        struct Node* temp = list->head;
        list->head = temp->next;
        if (list->head == NULL) {
            // If the list becomes empty after popping
            list->tail = NULL;
        }
        int val = temp->id;
        free(temp);
        list->len--;
        return val;
    }

    return -1;
}

// Function to pop a node from the right of the list
int list_popRight(struct ListNode* list) {
    if (list->head != NULL) {
        int val = list->tail->id;
        if (list->head == list->tail) {
            // If there is only one node in the list
            free(list->head);
            list->head = NULL;
            list->tail = NULL;
        } else {
            struct Node* current = list->head;
            while (current->next != list->tail) {
                current = current->next;
            }
            free(list->tail);
            current->next = NULL;
            list->tail = current;
        }
        list->len--;
        return val;
    }

    return NODE_UNDEF;
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
        list_init(&g->arr[i].adj);
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
    if( (id_src == id_dest) | (id_src < 0) | (id_dest < 0) ) return;

    if(list_hasId(&g->arr[id_src].adj, id_dest) == 1) return;

    if (id_src >= 0 && id_src < g->n_vertex && id_dest >= 0 && id_dest < g->n_vertex) {
        list_addNodeLeft(&g->arr[id_src].adj, id_dest);
        //list_addNodeLeft(g->arr[id_dest].adj, id_src);
    }
}

// Function for not connected nodes -> connect this vertex to all other vertex
void graph_connect_2_all(struct Graph *g, int id){
    if(list_get_len(&g->arr[id].adj) > 0) return;

    for(int i=0; i<g->n_vertex; ++i){
        if(g->arr[i].id == NODE_UNDEF || i == id) continue;

        list_addNodeLeft(&g->arr[i].adj, id);
        list_addNodeLeft(&g->arr[id].adj, i);
    }
}

// Function to delete an edge between two vertices in the graph
void graph_deleteEdge(struct Graph* g, int id_src, int id_dest) {
    if (id_src >= 0 && id_src < g->n_vertex && id_dest >= 0 && id_dest < g->n_vertex) {
        list_deleteNode(&g->arr[id_src].adj, id_dest);
        //list_deleteNode(g->arr[id_dest].adj, id_src);
    }
}

// Function to delete a vertex from the graph
void graph_deleteNode(struct Graph* g, int id) {
    g->arr[id].id = NODE_UNDEF;
    
    list_free(&g->arr[id].adj);

    list_init(&g->arr[id].adj);
    g->arr[id].state = DEFAULT_STATE;
    g->arr[id].posX = 0;
    g->arr[id].posY = 0;
    g->arr[id].curX = 0;
    g->arr[id].curY = 0;

    for(int i=0; i<g->n_vertex; ++i){
        if(g->arr[i].id == NODE_UNDEF) continue;

        list_deleteNode(&g->arr[i].adj, id);
    }
}

// Function to print the graph
void graph_print(struct Graph* g) {
    for (int i = 0; i < g->n_vertex; i++) {
        if (g->arr[i].id == NODE_UNDEF) continue;

        printf("Vertex %d (%5.2f, %5.2f) - Adjacent: ", g->arr[i].id, g->arr[i].posX, g->arr[i].posY);
        struct Node* cur = g->arr[i].adj.head;
        while (cur != NULL) {
            printf("%d ", cur->id);
            cur = cur->next;
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
        g->arr_visited[id] = NODE_NOT_VISITED;
    }
}

// Function to free the memory allocated for the graph
void graph_free(struct Graph* g) {
    for (int i = 0; i < g->n_vertex; i++) {
        list_free(&g->arr[i].adj);
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

    g->arr_visited[id] = NODE_VISITED; // Mark the cur vertex as visited

    struct Node* cur = g->arr[id].adj.head;
    while (cur != NULL) {
        graph_dfs(g, cur->id);
        cur = cur->next;
    }
}


void search_state_init(struct SearchState *state, int n_vertex) {
    state->stack = (int*)malloc(n_vertex * sizeof(int));
    
    for(int i=0; i<n_vertex; ++i) 
        state->stack[i] = NODE_UNDEF;

    state->front = NODE_UNDEF;
    state->back = NODE_UNDEF;
    state->top = NODE_UNDEF;
    state->len = n_vertex;
    state->mode = DEFAULT_SEARCH_MODE;
}

void search_state_switch_mode(struct SearchState *state){
    int mode = state->mode;
    mode = (mode + 1)%(TOTAL_MODES + 1);
    
    if(mode == 0) mode = DEFAULT_SEARCH_MODE;

    state->mode = mode;
}

void search_state_polish(struct SearchState *state, int vertex){
    
    for(int i=0; i<state->len; ++i){
        if(state->stack[i] == vertex) state->stack[i] = -1;
    }

    int i=0, j=0;
    for(i=0, j=0; i<state->len; ++i)
        if(state->stack[i] != -1) state->stack[j++] = state->stack[i];
    
    //j++;
    state->front = j - 1;
    for(; j<state->len; ++j) state->stack[j] = -1;
}

void search_state_push(struct SearchState *state, int vertex) {
    search_state_polish(state, vertex);

    state->front = state->front + 1;
    
    state->stack[state->front] = vertex;
}

int search_state_pop(struct SearchState *state) {
    int val = NODE_UNDEF;

    while(val == NODE_UNDEF && state->front != -1) {
        val = state->stack[state->front];
        state->stack[state->front] = NODE_UNDEF;
        state->front = state->front - 1;
    }
    return val;
}

void search_state_enque(struct SearchState *state, int vertex){
    if(state->back == NODE_UNDEF) state->back = 0;
    
    state->front = state->front + 1;
    
    state->stack[state->front] = vertex;
}

int search_state_deque(struct SearchState *state){
    if(state->back > state->front) {
        state->back = NODE_UNDEF;
        state->front = NODE_UNDEF;
        return NODE_UNDEF;
    }
    int val = state->stack[state->back];
    
    state->stack[state->back] = NODE_UNDEF;
    state->back = state->back + 1;

    return val;
}

int search_state_empty(struct SearchState *state) {
    return state->front == -1;
}

void search_state_clean(struct SearchState *state){
    state->front = NODE_UNDEF;
    state->back  = NODE_UNDEF;
    state->top = NODE_UNDEF;
    for(int i=0; i<state->len; ++i)
        state->stack[i] = NODE_UNDEF;
}

// Function to get the search mode string
const char* search_state_get_mode_string(int mode) {
    switch (mode) {
        case DFS:
            return "Depth First Search";
        case BFS:
            return "Breadth First Search";
        // Add cases for other search modes
        default:
            return "Depth First Search Mode";
    }
}

// Function to print search mode
void print_search_mode(int mode) {
    printf("   - Mode : %s\n", search_state_get_mode_string(mode));
}

// Function to print other status information
void print_status_info(struct SearchState* state) {
    printf("   - Front: %02d\n", state->front);
    printf("   - Back : %02d\n", state->back);
    printf("   - TOP  : %02d (DFS)\n", state->top);
}

// Function to print stored values
void print_stored_values(struct SearchState* state) {
    printf("   - Stored Values:");
    for (int i = 0; i < state->len; ++i) {
        if (state->stack[i] == -1) continue;
        printf("  ([%02d]: %02d)", i, state->stack[i]);
    }
    printf("\n");
}

// Function to print the entire search state
void search_state_print(struct SearchState* state) {
    printf("SearchState status:\n");
    print_search_mode(state->mode);
    print_status_info(state);
    print_stored_values(state);
}

int dfs_step(struct Graph* g, struct SearchState *state) {
    if (search_state_empty(state)) return -1; // Search completed  

    int cur = search_state_pop(state);
    if(cur == NODE_UNDEF) return 0;

    if (g->arr_visited[cur] == NODE_VISITED) return 0; // Already visited, move to the next step
    
    g->arr_visited[cur] = NODE_VISITED;

    struct Node* neighbor = g->arr[cur].adj.head;
    while (neighbor != NULL) {
        if(g->arr_visited[neighbor->id] != NODE_VISITED){
            search_state_push(state, neighbor->id);
        }
        neighbor = neighbor->next;
    }

    return 1; // Move to the next step
}


int bfs_step(struct Graph *g, struct SearchState *state){
    if(search_state_empty(state)) return -1;

    int cur = search_state_deque(state);
    if(cur == NODE_UNDEF) return 0;

    if(g->arr_visited[cur] == NODE_VISITED) return 0;

    g->arr_visited[cur] = NODE_VISITED;

    struct Node *neighbor = g->arr[cur].adj.head;
    while(neighbor != NULL){
        if(
                (g->arr_visited[neighbor->id] != NODE_VISITED) & 
                (g->arr_visited[neighbor->id] != NODE_ENQUED )
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


void search_insert(struct SearchState *state, int src_node){
    if(state->mode == DFS) 
        search_state_push(state, src_node);
    else if(state->mode == BFS) 
        search_state_enque(state, src_node);
    else
        search_state_push(state, src_node);
}

