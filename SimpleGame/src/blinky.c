
#include "blinky.h"

#include "qpc.h"
#include "stdio.h"

enum { BSP_TICKS_PER_SEC = 100 };

// Declare Blinky Active Object
typedef struct {
    QActive super;

    QTimeEvt timeEvt;
} Blinky;

//Define Blinky AO
static Blinky local_blinky;

//Set the opaque pointer to point to the actual blinky object
QActive * const AO_Blinky = &local_blinky.super;


//AO State Function Declarations
static QState Blinky_initial(Blinky * const me, void const * const par);
static QState Blinky_off(Blinky * const me, QEvt const * const e);
static QState Blinky_on(Blinky * const me, QEvt const * const e);



void Blinky_ctor(void){
    Blinky *me = (Blinky *)AO_Blinky;
    QActive_ctor(&me->super, Q_STATE_CAST(&Blinky_initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, TIMEOUT_SIG, 0u);
}

static QState Blinky_initial(Blinky * const me, void const * const par){
    (void)par;

    QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SEC/2, BSP_TICKS_PER_SEC/2);

    return Q_TRAN(&Blinky_off);
}

static QState Blinky_off(Blinky * const me, QEvt const * const e){
    QState status;
    switch(e->sig){
        case Q_ENTRY_SIG: {
            printf("LED Off\n");
            status = Q_HANDLED();
            break;
        }

        case TIMEOUT_SIG: {
            status = Q_TRAN(&Blinky_on);
            break;
        }

        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }

    return status;
}

static QState Blinky_on(Blinky * const me, QEvt const * const e){
    QState status;
    switch(e->sig){
        case Q_ENTRY_SIG: {
            printf("LED On\n");
            status = Q_HANDLED();
            break;
        }

        case TIMEOUT_SIG: {
            status = Q_TRAN(&Blinky_off);
            break;
        }

        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }

    return status;
}