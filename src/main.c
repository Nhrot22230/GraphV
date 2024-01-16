#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <complex.h>
#include <math.h>

#include <raylib.h>

#include <dlfcn.h>

#include "quick_lib.h"

#define X(name, ...) name##_t *name = NULL;
LIST_FUNC
#undef X

const char *qlib_name = "quick_lib.so";
void *qlib = NULL;

int lib_reload(void){
    if(qlib != NULL) dlclose(qlib);
    qlib = dlopen(qlib_name, RTLD_NOW);

    if(qlib == NULL){
        fprintf(stderr, "ERROR: could not load %s: %s\n", qlib_name, dlerror());
        return -1;
    }
    
    #define X(name, ...) \
        name = dlsym(qlib, #name); \
        if(name == NULL) { \
            fprintf(stderr, "ERROR: could not find %s symbol in %s: %s\n", #name, qlib_name, dlerror()); \
            return -1; \
        } 
    LIST_FUNC
    #undef X

    return 1;
}


int main(void){

    lib_reload();

    size_t factor = 60;
    InitWindow(factor*16, factor*9, "MusiV");

    
    SetTargetFPS(60);
    
    state_init();
    while(!WindowShouldClose()){
        if(IsKeyPressed(KEY_Q)) break;

        if(IsKeyPressed(KEY_R)){
            void *state = state_pre_reload();
            if(lib_reload() == -1) return 1;
            state_post_reload(state);
        }

        state_update();
    }
    CloseWindow();

    return 0;
}
