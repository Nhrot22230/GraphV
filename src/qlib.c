#include "qlib.h"

Plug *plug = NULL;

const int N = 100;
const float v_radius = 15.0f;
const float speed = 0.0f;
const float amp = 25.0f;

size_t id = 0;
int id_node = -1;
size_t t = 0;

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

void StackId_update_graph(struct StackId *stack, struct Graph *g){
    if(stack->len < 2) return;

    int des_id = StackId_pop(stack);
    int src_id = StackId_pop(stack);
    
    if(list_hasId(g->arr[src_id].adj, des_id) == -1)
        graph_addEdge(g, src_id, des_id);
    else
        graph_deleteEdge(g, src_id, des_id);

    g->arr[src_id].state = DEFAULT_STATE;
    g->arr[des_id].state = DEFAULT_STATE;
}


/************************************************************
 *                                                          *
 *                Start of Function Declarations            *
 *                                                          *
 ************************************************************/

void graph_edges_render(struct Graph *g, float radius){
    const int n = g->n_vertex;
    
    for(int i=0; i<n; ++i){
        if(g->arr[i].id == NODE_UNDEF) continue;

        float posX = g->arr[i].curX;
        float posY = g->arr[i].curY;

        struct Node *cur = NULL;
        for(cur = g->arr[i].adj->head; cur; cur=cur->next){
            int fPosX = g->arr[cur->id].curX;
            int fPosY = g->arr[cur->id].curY;

            DrawLine(posX, posY, fPosX, fPosY, PURPLE);

            float rot = 0.0f;

            if(fabs(fPosX - posX) >= 0.001f) rot = atanf( (posY - fPosY)/(fPosX - posX) );

            if(fPosX < posX) rot += M_PI;
            if(fPosX > posX & fPosY > posY) rot += 2*M_PI;

            float x0, y0, x1, y1, x2, y2;
            float A_H = radius;

            x0 = fPosX - radius * cosf(rot); y0 = fPosY + radius * sinf(rot);
            x1 = x0 - A_H * sinf(rot + M_PI/4); y1 = y0 + A_H * sinf(rot - M_PI/4);
            x2 = x0 + A_H * sinf(rot - M_PI/4); y2 = y0 + A_H * sinf(rot + M_PI/4);
            /* 
            char buffer [30];
            sprintf(buffer, "%5.2f deg", rot*(180.f/M_PI));
            DrawText(buffer, 0, 0, 20, WHITE);
            
            sprintf(buffer, "x0: %5.2f", x0);
            DrawText(buffer, 0, 20.0f, 20, WHITE);
            sprintf(buffer, "y0: %5.2f", y0);
            DrawText(buffer, 400.0f, 20.0f, 20, WHITE);
            
            sprintf(buffer, "x1: %5.2f", x1);
            DrawText(buffer, 0, 40.0f, 20, WHITE);
            sprintf(buffer, "y1: %5.2f", y1);
            DrawText(buffer, 400.0f, 40.0f, 20, WHITE);
            
            sprintf(buffer, "x2: %5.2f", x2);
            DrawText(buffer, 0, 60.0f, 20, WHITE);
            sprintf(buffer, "y2: %5.2f", y2);
            DrawText(buffer, 400.0f, 60.0f, 20, WHITE);
            */
            DrawTriangle(
                    (Vector2) {x0, y0},
                    (Vector2) {x1, y1},
                    (Vector2) {x2, y2},
                    PURPLE
                    );
        }
    }
}

void graph_extra_render(struct Graph *g){
}

void graph_nodes_render(struct Graph *g, float radius){
    const int n = g->n_vertex;
    for(int i=0; i<n; ++i){
        if(g->arr[i].id == NODE_UNDEF) continue;

        float posX = g->arr[i].curX;
        float posY = g->arr[i].curY;

        char buffer[5];
        sprintf(buffer, "%d", i);
        
        Color c = GREEN;
        if(g->arr[i].state == NODE_SELECTED) c = BLUE;
        else if(g->arr[i].state == NODE_MOVING) c = WHITE;
        else if(g->arr_visited[i] == NODE_VISITED) c = ORANGE;

        DrawCircle(posX, posY, radius, c);
        
        int font_size = 30;
        
        Vector2 txt_size = MeasureTextEx(GetFontDefault(), buffer, font_size, 1);
        int x = txt_size.x / 2;
        int y = txt_size.y / 2;
        
        DrawText(buffer, posX - x, posY - y, font_size, BLACK);
    }

}

void graph_draw(struct Graph *g){
    graph_edges_render(g, v_radius);
    
    graph_extra_render(g);

    graph_nodes_render(g, v_radius);
}

void StackId_clean(struct StackId *stack){
    while(stack->len != -1) StackId_pop(stack);
    StackId_push(stack, 0);
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
    if(id_node != -1 & plug->g.arr[id_node].state == NODE_MOVING){
        float posX = GetMouseX();
        float posY = GetMouseY();

        node_move(&plug->g, id_node, posX, posY);
    }

    StackId_update_graph(&node_stack, &plug->g);

    t = (t+1)%10009;
}

void state_render(void){
    BeginDrawing();
    Color c = {0x18, 0x18, 0x18, 255};
    ClearBackground(c);

    graph_draw(&plug->g);

    EndDrawing();
}

void state_logic(void){
    if(IsKeyPressed(KEY_SPACE)){
        printf("Id_Node=%d\n", id_node);
        if(id_node != -1) printf("Id_Node_State=%d\n", plug->g.arr[id_node].state);
    }
    // Hacer click
    // Derecho
    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
        float posX = GetMouseX();
        float posY = GetMouseY();
            
        int temp_id = IsNodeHere(&plug->g, posX, posY, v_radius);
        if(temp_id != NODE_UNDEF) graph_deleteNode(&plug->g, temp_id);

        if(id_node == temp_id) id_node = -1;
    }

    // Izquierdo
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        if(id_node != -1 && plug->g.arr[id_node].state == NODE_MOVING){
            plug->g.arr[id_node].state = DEFAULT_STATE;
            id_node = -1;
            StackId_pop(&node_stack);
        }
        else{
            float posX = GetMouseX();
            float posY = GetMouseY();
            
            id_node = IsNodeHere(&plug->g, posX, posY, v_radius);
            if(id_node != -1) node_alt_state(&plug->g, id_node);
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

    if(IsKeyPressed(KEY_P)){
        graph_print(&plug->g);
    }

    if(IsKeyPressed(KEY_C)){
        t = 0;
        id_node = -1;
        id = 0;
        graph_clean(&plug->g);
        StackId_clean(&node_stack);
        search_state_clean(&plug->state);
        search_state_restart(&plug->state, 0);
    }

    if(IsKeyPressed(KEY_W)){
        graph_clean_visited(&plug->g);
        search_state_clean(&plug->state);
        search_state_restart(&plug->state, 0);
    }

    if(IsKeyPressed(KEY_S)){
        search_state_switch_mode(&plug->state);
        graph_clean_visited(&plug->g);
        search_state_clean(&plug->state);
        search_state_restart(&plug->state, 0);
        search_state_print(&plug->state);
    }

    if(IsKeyPressed(KEY_A)) {
        search_state_print(&plug->state);
    }

    if(IsKeyPressed(KEY_D) && plug->g.arr[0].id != -1) {
        search_step(&plug->g, &plug->state);
    }
}
