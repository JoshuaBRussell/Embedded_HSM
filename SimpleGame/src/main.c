



#include "qpc.h"
#include "Tank.h"
#include "Game.h"
#include <stdlib.h> //For exit()
#include <stdio.h>

#define FRAMES_PER_SEC 30


Q_DEFINE_THIS_FILE

//Functions that are "supplied" to QF
void Q_onAssert(char const * const module, int loc){
    (void)module;
    (void)loc;

    exit(-1);
}

void QF_onStartup(void){}
void QF_onCleanup(void){}
void QF_onClockTick(void) {
    //Allows each AO to process their own time events
    QF_TICK_X(0, (void *)0);

    //Creates a system wide timer event 
    static QEvt const tickEvt = {TIME_SIG, 0, 0};
    QF_PUBLISH(&tickEvt, 0);

}


int main(){

    //Statically allocate event queue
    static QEvt const *blinky_queue[10];

    //Static storage for list of subscribers
    static QSubscrList subscr_storage[10];
    
    
    QF_init();
    QF_setTickRate(FRAMES_PER_SEC, 1); //Set the system clock tick rate
    
    //Initialize QF Publish-Subscribe faculties
    QF_psInit(subscr_storage, Q_DIM(subscr_storage));

    //Call AO Constructor
    Tank_ctor();
    QACTIVE_START(AO_Tank,
                  1,
                  blinky_queue, Q_DIM(blinky_queue),
                  (void *)0, 0,
                  (QEvt *)0);

    return QF_run();

}