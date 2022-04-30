



#include "qpc.h"
#include "blinky.h"
#include <stdlib.h> //For exit()

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
    QF_TICK_X(0, (void *)0);
}


int main(){

    // Statically allocate event queue
    static QEvt const *blinky_queue[10];

    QF_init();

    //Call AO Constructor
    Blinky_ctor();
    QACTIVE_START(AO_Blinky,
                  1,
                  blinky_queue, Q_DIM(blinky_queue),
                  (void *)0, 0,
                  (QEvt *)0);

    return QF_run();

}