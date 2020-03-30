#ifndef TIMER_BU
#define TIMER_BU

#include "../Base/base.h"

int Timer_click();
int Timer_getMiliseconds();
int Timer_getSeconds();

static struct {
    clock_t begin;
    clock_t end;
    clock_t diff;
    int (*click)();
    int (*getMiliseconds)();
    int (*getSeconds)();
} Timer = {
    (clock_t)0,
    (clock_t)0,
    (clock_t)0,
    Timer_click,
    Timer_getMiliseconds,
    Timer_getSeconds
};

int Timer_click(){
    Timer.end = clock();
    Timer.diff = Timer.end-Timer.begin;
    Timer.begin = Timer.end;
    return(Timer.diff);
}

int Timer_getMiliseconds(){
    return((int)((double)(Timer.diff) * (double)1000/ (double)CLOCKS_PER_SEC ));
}

int Timer_getSeconds(){
    return(((double)(Timer.diff)/ (double)( CLOCKS_PER_SEC)));
}

#endif