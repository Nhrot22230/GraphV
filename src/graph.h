// graph.h

#ifndef GRAPH_H
#define GRAPH_H

enum NodeState {
    NODE_UNDEF = -1,
    NODE_NOT_VISITED = 0,
    NODE_VISITED = 1,
    NODE_ENQUED = 10,
    NODE_NOT_SELECTED = 2,
    NODE_SELECTED = 3,
    NODE_MOVING = 4,
    TOTAL_STATES = 4,
    DEFAULT_STATE = 2
};

// Define the structure for a Node
struct Node {
    int id;
    int w;
    struct Node* next;
};

// Define the structure for a ListNode (contains a pointer to the head of the list)
struct ListNode {
    struct Node* head;
    int length;
};

// Define the structure for a GraphNode
struct GraphNode {
    int id;
    int state;

    float posX;
    float posY;
    
    float curX;
    float curY;
    struct ListNode* adj;
};

// Define the structure for a Graph
struct Graph {
    struct GraphNode* arr;
    int n_vertex;
    int* arr_visited; // Added for DFS/BFS support
    float *distances;
};

// Define the structure for SearchState
enum{
    DFS = 1,
    BFS = 2,
    DEFAULT_SEARCH_MODE = 1,
    TOTAL_MODES = 2
};

struct SearchState {
    int* stack;
    int top;
    int mode;
};

// Function declarations
struct Node* node_create(int id);
int IsNodeHere(struct Graph *g, float posX, float posY, float radius);
void node_move(struct Graph *g, int id, float posX, float posY); 
void node_alt_state(struct Graph *g, int id);

struct ListNode* list_init();
int list_hasId(struct ListNode *list, int id);
void list_addNode(struct ListNode *list, int id);
void list_deleteNode(struct ListNode *list, int id);
void list_free(struct ListNode *list);
int list_empty(struct ListNode *list);

struct GraphNode* graphNode_create(int id, float x, float y);

void graph_create(struct Graph *g);
void graph_init(struct Graph *g, int n);
void graph_addVertex(struct Graph *g, int id, float x, float y);
void graph_addEdge(struct Graph *g, int id_src, int id_dest);
void graph_connect_2_all(struct Graph *g, int id);
void graph_deleteEdge(struct Graph *g, int id_src, int id_dest);
void graph_deleteNode(struct Graph *g, int id);
void graph_print(struct Graph *g);
void graph_clean(struct Graph *g);
void graph_clean_visited(struct Graph *g);

void graph_free(struct Graph* g);
void graph_dfs(struct Graph *g, int id);

void search_state_init(struct SearchState *state, int n_vertex);
void search_state_restart(struct SearchState *state, int vertex);
void search_state_switch_mode(struct SearchState *state);

void search_state_push(struct SearchState *state, int vertex);
void search_state_enque(struct SearchState *state, int vertex);
int search_state_deque(struct SearchState *state);
int search_state_pop(struct SearchState *state);
int search_state_empty(struct SearchState *state);
void search_state_clean(struct SearchState *state);
void search_state_print(struct SearchState *state);

int dfs_step(struct Graph *g, struct SearchState *state);
int bfs_step(struct Graph *g, struct SearchState *state);
int search_step(struct Graph *g, struct SearchState *state);
void stepped_dfs(struct Graph *g, int start_vertex, struct SearchState *state);

#endif // GRAPH_H

