#include "quick_lib.h"

Plug *plug = NULL;

const int N = 100;
const float v_radius = 20.0f;
const float speed = 0.0f;
const float amp = 25.0f;

size_t id = 0;
int id_node = -1;
size_t t = 0;

float center_x = 0;
float center_y = 0;

struct StackId{
    int st[10];
    int len;
};

struct StackId node_stack;

void StackId_push(struct StackId *stack, int id){
    if(id < 0) return;

    stack->len = stack->len + 1;
    stack->st[stack->len] = id;
}

int StackId_pop(struct StackId *stack){
    if(stack->len == -1) return -1;

    int val = stack->st[stack->len];
    stack->st[stack->len] = 0;
    stack->len = stack->len - 1;
    
    return val;
}

void StackId_update(struct StackId *stack){
    if(stack->len < 2) return;

    int src_id = StackId_pop(stack);
    int des_id = StackId_pop(stack);
    
    if(list_hasId(plug->g.arr[src_id].adj, des_id) == -1)
        graph_addEdge(&plug->g, src_id, des_id);
    else
        graph_deleteEdge(&plug->g, src_id, des_id);

    plug->g.arr[src_id].state = DEFAULT_STATE;
    plug->g.arr[des_id].state = DEFAULT_STATE;
}

/************************************************************
 *                                                          *
 *                Start of Function Declarations            *
 *                                                          *
 ************************************************************/


int IsNodeHere(struct Graph *g, float posX, float posY, float radius){
    float max_Xrange = posX + radius;
    float min_Xrange = posX - radius;

    float max_Yrange = posY + radius;
    float min_Yrange = posY - radius;

    for(int id = 0; id<g->n_vertex; ++id){
        float x = g->arr[id].curX;
        float y = g->arr[id].curY;

        if( (min_Xrange <= x && x <= max_Xrange) && (min_Yrange <= y && y <= max_Yrange) ) 
            return id;
    }

    return -1;
}

void move_node(struct Graph *g, int id, float posX, float posY){
    g->arr[id].posX = posX;
    g->arr[id].posY = posY;

    g->arr[id].curX = posX;
    g->arr[id].curY = posY;
}

void alternate_node_state(struct Graph *g, int id){
    g->arr[id].state = (g->arr[id].state + 1) % (TOTAL_STATES + 1);
    
    if(g->arr[id].state == 0) g->arr[id].state = DEFAULT_STATE;
}

void update_graph_state(void){
    StackId_update(&node_stack);
}

void graph_draw(void){
    for(int i=0; i<N; ++i){
        if(plug->g.arr[i].id == NODE_UNDEF) continue;

        float posX = plug->g.arr[i].curX;
        float posY = plug->g.arr[i].curY;

        struct Node *cur = NULL;
        for(cur = plug->g.arr[i].adj->head; cur; cur=cur->next){
            int fPosX = plug->g.arr[cur->id].curX;
            int fPosY = plug->g.arr[cur->id].curY;
            DrawLine(posX, posY, fPosX, fPosY, PURPLE);
        }
    }

    for(int i=0; i<N; ++i){
        if(plug->g.arr[i].id == NODE_UNDEF) continue;

        float posX = plug->g.arr[i].curX;
        float posY = plug->g.arr[i].curY;

        char buffer[5];
        sprintf(buffer, "%d", i);
        
        Color c = GREEN;
        if(plug->g.arr[i].state == NODE_SELECTED) c = BLUE;
        else if(plug->g.arr[i].state == NODE_MOVING) c = WHITE;
        else if(plug->g.arr_visited[i] == NODE_VISITED) c = ORANGE;

        DrawCircle(posX, posY, v_radius, c);
        
        int font_size = 30;
        
        Vector2 txt_size = MeasureTextEx(GetFontDefault(), buffer, font_size, 1);
        int x = txt_size.x / 2;
        int y = txt_size.y / 2;
        
        DrawText(buffer, posX - x, posY - y, font_size, BLACK);
    }
}

void graph_clean(void){
    for(int i=0; i<N; ++i){
        if(plug->g.arr[i].id == NODE_UNDEF) continue;
        graph_deleteNode(&plug->g, i);
        plug->g.arr_visited[i] = NODE_NOT_VISITED;
        plug->g.arr[i].state = DEFAULT_STATE;
    }

    while(plug->state.top != -1) search_state_pop(&plug->state);
    
    search_state_push(&plug->state, 0);
}

/************************************************************
 *                                                          *
 *                Start of Function Declarations            *
 *                                                          *
 ************************************************************/


void state_init(void){
    plug = malloc(sizeof(*plug));
    memset(plug, 0, sizeof(*plug));

    graph_init(&plug->g, N);

    search_state_init(&plug->state, N);
    search_state_push(&plug->state, 0);
}

Plug* state_pre_reload(void){
    return plug;
}

void state_post_reload(Plug *p){
    plug = p;
}

void state_update(void){
    int w = GetRenderWidth();
    int h = GetRenderHeight();
    float posX, posY;

    center_x = w/2; center_y = h/2;

    BeginDrawing();

    ClearBackground(BLACK);
  
    if(IsKeyPressed(KEY_SPACE)){
        printf("Id_Node=%d\n", id_node);
        if(id_node != -1) printf("Id_Node_State=%d\n", plug->g.arr[id_node].state);
    }
    // Hacer click
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        if(id_node != -1 && plug->g.arr[id_node].state == NODE_MOVING){
            plug->g.arr[id_node].state = DEFAULT_STATE;
            id_node = -1;
            StackId_pop(&node_stack);
        }
        else{
            posX = GetMouseX();
            posY = GetMouseY();
            
            id_node = IsNodeHere(&plug->g, posX, posY, v_radius);
            if(id_node != -1) alternate_node_state(&plug->g, id_node);
            else{
                //printf("X: %5.2f, Y: %5.2f\n", posX, posY);
                graph_addVertex(&plug->g, id, posX, posY);
                id = (id + 1) % N;
            }

            if(plug->g.arr[id_node].state == NODE_SELECTED){
                StackId_push(&node_stack, id_node);
            }
        }
    }

    if(id_node != -1 & plug->g.arr[id_node].state == NODE_MOVING){
        posX = GetMouseX();
        posY = GetMouseY();

        move_node(&plug->g, id_node, posX, posY);
    }

    if(IsKeyPressed(KEY_P)){
        graph_print(&plug->g);
    }

    if(IsKeyPressed(KEY_C)){
        t = 0;
        id_node = -1;
        id = 0;
        graph_clean();
    }

    if(IsKeyPressed(KEY_D) && plug->g.arr[0].id != -1) {

        dfs_step(&plug->g, &plug->state);
        printf("Search: %d \n", plug->state.top);
    }

    graph_draw();

    EndDrawing();
    
    update_graph_state();
    t = (t+1)%10009;
}
