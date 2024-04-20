#include "qlib.h"

Plug *plug = NULL;

const int N = 1000;
const float v_radius = 15.0f;

int id_node = -1;
size_t t = 0;

/* 
    ========================================================================================
                                        STACKID/LISTID FUNCTIONS
    ========================================================================================
*/

void StackId_init(struct StackId *stack){
    for(int i=0; i<10; i++) stack->st[i] = NODE_UNDEF;
    stack->top = 0;
}

void StackId_push(struct StackId *stack, int id){
    if(id < 0) return;
    stack->st[stack->top] = id;
    stack->top = stack->top + 1;
}

int StackId_pop(struct StackId *stack){
    if(stack->top <= 0) return -1;

    stack->top = stack->top - 1;
    int val = stack->st[stack->top];
    stack->st[stack->top] = -1;
    
    return val;
}

void StackId_update_graph(struct StackId *stack, struct Graph *g){
    if(stack->top < 2) return;
    int des_id = StackId_pop(stack);
    int src_id = StackId_pop(stack);
    
    if(list_hasId(&g->arr[src_id].adj, des_id) == -1) graph_addEdge(g, src_id, des_id);
    else graph_deleteEdge(g, src_id, des_id);
    PlaySound(plug->sfx[SOUND_EDGE]);
    g->arr[src_id].state = DEFAULT_STATE;
    g->arr[des_id].state = DEFAULT_STATE;
}

void StackId_print(struct StackId *stack){
    printf("StackId len: %d\n", stack->top);
    printf("Stacked Values for connect: ");
    for(int i=0; i<10; ++i){
        printf("  %02d", stack->st[i]);
    }
    printf("\n");
}

void StackId_clean(struct StackId *stack){
    for(int i=0; i<10; i++)
        stack->st[i] = NODE_UNDEF;
    stack->top =0;
}

void ListId_restart(struct ListNode* list, int len){
    list_free(list);
    
    for(int i=0; i<len; ++i) list_addNodeRight(list, i);
}

/* 
    ========================================================================================
                                        RENDER FUNCTIONS
    ========================================================================================
*/

void graph_edges_render(struct Graph *g, float radius){
    const int n = g->n_vertex;
    
    for(int i=0; i<n; ++i){
        if(g->arr[i].id == NODE_UNDEF) continue;

        float posX = g->arr[i].curX;
        float posY = g->arr[i].curY;

        struct Node *cur = NULL;
        for(cur = g->arr[i].adj.head; cur; cur=cur->next){
            float fPosX = g->arr[cur->id].curX;
            float fPosY = g->arr[cur->id].curY;

            DrawLine(posX, posY, fPosX, fPosY, RED);

            float rot = 0.0f;

            if(fabs(fPosX - posX) >= 0.001f) rot = atanf( (posY - fPosY)/(fPosX - posX) );
            else{
                if(fPosY > posY) rot = 3*PI/2;
                if(fPosY < posY) rot = PI/2;
            }
            if(fPosX < posX) rot += PI;
            if( (fPosX > posX) & (fPosY > posY) ) rot += 2*PI;

            float x0, y0, x1, y1, x2, y2;
            float A_H = radius/2;

            x0 = fPosX - radius * cosf(rot); y0 = fPosY + radius * sinf(rot);
            x1 = x0 - A_H * sinf(rot + PI/4); y1 = y0 + A_H * sinf(rot - PI/4);
            x2 = x0 + A_H * sinf(rot - PI/4); y2 = y0 + A_H * sinf(rot + PI/4);
            
            DrawTriangle(
                (Vector2) {x0, y0}, 
                (Vector2) {x1, y1}, 
                (Vector2) {x2, y2},
                PINK
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

        char buffer[20];
        sprintf(buffer, "%d", i);
        
        Color c = GREEN;
        if(g->arr[i].state == NODE_SELECTED) c = SKYBLUE;
        else if(g->arr[i].state == NODE_MOVING) c = RAYWHITE;
        else if(g->arr_visited[i] == NODE_VISITED) c = GOLD;

        DrawCircle(posX, posY, radius, c);
        
        int font_size = 20;
        
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

void search_state_draw(struct SearchState *state, int src_node){
    DrawText(search_state_get_mode_string(state->mode), 0, 10, 20, RAYWHITE);
    char buffer[30];
    sprintf(buffer, "Source: %d", src_node);
    DrawText(buffer, 0, 30, 20, RAYWHITE);
}


/* 
    ========================================================================================
                                        PLUG FUNCTIONS
    ========================================================================================
*/

void state_loadSounds(Plug *p){
    p->sfx[SOUND_NODE] = LoadSound("./resources/audio/node_2.wav");
    p->sfx[SOUND_EDGE] = LoadSound("./resources/audio/add_edge.wav");
    p->sfx[SOUND_MOVE] = LoadSound("./resources/audio/node_move_1.wav");
    p->sfx[SOUND_CLEAN] = LoadSound("./resources/audio/clean_everything.wav");
    p->sfx[SOUND_SEARCH_STEP] = LoadSound("./resources/audio/search_step_2.wav");
    p->sfx[SOUND_SEARCH_SWITCH] = LoadSound("./resources/audio/search_switch.wav");

    p->nav_cur = 0;
    p->nav_len = 30;
    p->notas[NOTE_G4] = LoadSound("./resources/notes/G4.wav");
    p->notas[NOTE_A4] = LoadSound("./resources/notes/A4.wav");
    p->notas[NOTE_B4] = LoadSound("./resources/notes/B4.wav");
    p->notas[NOTE_C5] = LoadSound("./resources/notes/C5.wav");
    p->notas[NOTE_D5] = LoadSound("./resources/notes/D5.wav");
    p->notas[NOTE_E5] = LoadSound("./resources/notes/E5.wav");
    p->notas[NOTE_F5] = LoadSound("./resources/notes/B5.wav");

    p->navidad[0] = p->notas[NOTE_G4];
    p->navidad[1] = p->notas[NOTE_C5];
    p->navidad[2] = p->notas[NOTE_C5];
    p->navidad[3] = p->notas[NOTE_D5];
    p->navidad[4] = p->notas[NOTE_C5];
    p->navidad[5] = p->notas[NOTE_B4];
    p->navidad[6] = p->notas[NOTE_A4];
    p->navidad[7] = p->notas[NOTE_A4];
    p->navidad[8] = p->notas[NOTE_A4];
    
    p->navidad[9] = p->notas[NOTE_D5];
    p->navidad[10] = p->notas[NOTE_D5];
    p->navidad[11] = p->notas[NOTE_E5];
    p->navidad[12] = p->notas[NOTE_D5];
    p->navidad[13] = p->notas[NOTE_C5];
    p->navidad[14] = p->notas[NOTE_B4];
    p->navidad[15] = p->notas[NOTE_G4];
    p->navidad[16] = p->notas[NOTE_G4];

    p->navidad[17] = p->notas[NOTE_E5];
    p->navidad[18] = p->notas[NOTE_E5];
    p->navidad[19] = p->notas[NOTE_F5];
    p->navidad[20] = p->notas[NOTE_E5];
    p->navidad[21] = p->notas[NOTE_D5];
    p->navidad[22] = p->notas[NOTE_C5];
    p->navidad[23] = p->notas[NOTE_A4];
    p->navidad[24] = p->notas[NOTE_G4];
    p->navidad[25] = p->notas[NOTE_G4];

    p->navidad[26] = p->notas[NOTE_A4];
    p->navidad[27] = p->notas[NOTE_D5];
    p->navidad[28] = p->notas[NOTE_B4];
    p->navidad[29] = p->notas[NOTE_C5];
}

void state_init(void){
    plug = malloc(sizeof(*plug));
    memset(plug, 0, sizeof(*plug));
    plug->src_node = 0;
    
    graph_init(&plug->g, N);
    
    ListId_restart(&plug->list_id, N); 
    search_state_init(&plug->state, N);
    search_state_push(&plug->state, plug->src_node);
    StackId_init(&plug->node_stack);

    state_loadSounds(plug);
    InitializeSnowfall(plug->snowfall, GetRenderWidth(), GetRenderHeight());
}

Plug* state_pre_reload(void){
    return plug;
}

void state_post_reload(Plug *p){
    plug = p;
}

void state_logic(void){
    /* 
    ========================================================================================
                                    MOUSE INPUT
    ========================================================================================
    */
    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)){
        state_delNode(&plug->g, &plug->node_stack, &plug->list_id);
    }

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        if(id_node != NODE_UNDEF && plug->g.arr[id_node].state == NODE_MOVING){
            plug->g.arr[id_node].state = DEFAULT_STATE;
            id_node = NODE_UNDEF;
            StackId_pop(&plug->node_stack);
        }
        else{
            float posX = GetMouseX();
            float posY = GetMouseY();
            
            id_node = IsNodeHere(&plug->g, posX, posY, v_radius);
            if(id_node != NODE_UNDEF) 
                node_alt_state(&plug->g, id_node);
            else{
                PlaySound(plug->sfx[SOUND_NODE]);
                state_addNode(&plug->g, &plug->list_id);
            }

            if(id_node != NODE_UNDEF && plug->g.arr[id_node].state == NODE_SELECTED){
                StackId_push(&plug->node_stack, id_node);
            }
        }
    }

    /* 
    ========================================================================================
                                    KEYBINNDINGS
    ========================================================================================
    */
        
    if(IsKeyPressed(KEY_D) && plug->g.arr[plug->src_node].id != NODE_UNDEF) {
        if(search_state_empty(&plug->state) == 0){
            PlaySound(plug->navidad[plug->nav_cur]);
            plug->nav_cur = (plug->nav_cur + 1)%plug->nav_len;
            search_step(&plug->g, &plug->state);
        }
    }
    
    if(IsKeyPressed(KEY_P)){
        state_print(plug);
    }

    if(IsKeyPressed(KEY_C)){
        PlaySound(plug->sfx[SOUND_CLEAN]);
        state_restart(plug);
    }

    if(IsKeyPressed(KEY_W)){
        state_restart_search(plug);
        search_insert(&plug->state, plug->src_node);
    }

    if(IsKeyPressed(KEY_S)){
        PlaySound(plug->sfx[SOUND_SEARCH_SWITCH]);
        search_state_switch_mode(&plug->state);
        state_restart_search(plug);
        search_insert(&plug->state, plug->src_node);
    }
}

void state_update(void){
    if( (id_node != -1) & (plug->g.arr[id_node].state == NODE_MOVING) ){
        float posX = GetMouseX();
        float posY = GetMouseY();
        PlaySound(plug->sfx[SOUND_MOVE]);
        node_move(&plug->g, id_node, posX, posY);
    }

    StackId_update_graph(&plug->node_stack, &plug->g);

    UpdateSnowfall(plug->snowfall, GetRenderWidth(), GetRenderHeight());
    t = (t+1)%10009;
}

void state_render(void){
    BeginDrawing();
    Color c = {0x18, 0x18, 0x18, 255};
    ClearBackground(c);

    graph_draw(&plug->g);
    search_state_draw(&plug->state, plug->src_node);
    DrawSnowfall(plug->snowfall);

    EndDrawing();
}

/* 
    ========================================================================================
                                    ADDITIONAL PLUG FUNCTIONS
    ========================================================================================
*/

void state_delNode(struct Graph *g, struct StackId *stack, struct ListNode *list){
    float posX = GetMouseX();
    float posY = GetMouseY();

    int temp_id = IsNodeHere(g, posX, posY, v_radius);
    if(temp_id != NODE_UNDEF) {
        if(g->arr[temp_id].state == NODE_SELECTED){
            g->arr[id_node].state = DEFAULT_STATE;
            id_node = NODE_UNDEF;
            StackId_pop(stack);
        }
        graph_deleteNode(g, temp_id);
        list_addNodeLeft(list, temp_id);
    }
}

void state_addNode(struct Graph *g, struct ListNode *list){
    float posX = GetMouseX();
    float posY = GetMouseY();

    int new_id = list_popLeft(list);
    graph_addVertex(g, new_id, posX, posY);
}

void state_restart(Plug *p){
    graph_clean(&p->g);

    ListId_restart(&plug->list_id, N);
    StackId_clean(&p->node_stack); 

    state_restart_search(p);
    search_insert(&p->state, p->src_node);
}

void state_restart_search(Plug *p){
    graph_clean_visited(&p->g);
    search_state_clean(&p->state);
}

void state_print(Plug *p){
    search_state_print(&p->state);
    graph_print(&p->g);
    StackId_print(&p->node_stack);
    printf("Id_Node=%d\n", id_node);
    if(id_node != NODE_UNDEF) printf("Id_Node_State=%d\n", p->g.arr[id_node].state);
}


void InitializeSnowfall(Snowflake snowfall[], int screenWidth, int screenHeight) {
    for (int i = 0; i < 200; i++) {
        snowfall[i].position.x = GetRandomValue(0, screenWidth);
        snowfall[i].position.y = GetRandomValue(0, screenHeight);
        snowfall[i].color = WHITE;
        snowfall[i].speed = GetRandomValue(1, 10) / 10.0f;
    }
}

void UpdateSnowfall(Snowflake snowfall[], int screenWidth, int screenHeight) {
    for (int i = 0; i < 200; i++) {
        snowfall[i].position.y += snowfall[i].speed;
        snowfall[i].position.x -= GetRandomValue(1, 20) / 10.0f;

        // Reset snowflake position if it goes below the screen
        if (snowfall[i].position.y > screenHeight) {
            snowfall[i].position.y = 0;
            snowfall[i].position.x = GetRandomValue(0, screenWidth);
        }
        if(snowfall[i].position.x < 0){
            snowfall[i].position.x = screenWidth;
        }

    }
}

void DrawSnowfall(Snowflake snowfall[]) {
    for (int i = 0; i < 200; i++) {
        DrawCircleV(snowfall[i].position, 2.5f, snowfall[i].color);
    }
}
