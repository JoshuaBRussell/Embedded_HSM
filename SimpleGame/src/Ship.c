#include "Ship.h"


#include "qpc.h"
#include "stdio.h"


enum {BSP_TICKS_PER_SEC = 100};


//Declare Ship Active Object
typedef struct{
    QActive super;

    QTimeEvt timeEvt;
} Ship;

//Define Ship AO
static Ship local_ship;

//Set the opaque pointer to point the correct
QActive * const AO_Ship = &local_ship.super;


static QState Ship_initial(Ship * const me, void const * const par);
static QState Ship_Active(Ship * const me, QEvt const * const e);



void Ship_ctor(void){
    Ship *me = (Ship *)AO_Ship;
    QActive_ctor(&me->super, Q_STATE_CAST(&Ship_initial));
    QTimeEvt_ctorX(&me->timeEvt, &me->super, TIME_SIG, 0u);
}

static QState Ship_initial(Ship * const me, void const * const par){
    (void)par;

    QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SEC/2, BSP_TICKS_PER_SEC/2);

    return Q_TRAN(&Ship_Active);
}


static QState Ship_Active(Ship * const me, QEvt const * const e){
    QState status;

    switch(e->sig){
        case Q_ENTRY_SIG: {
            printf("Enter Active.\n");
            status = Q_HANDLED();
            break;
        }

        case TIME_SIG: {
            printf("Timer Event...\n");
            status = Q_HANDLED();
            break;
        }

        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }

    return status;
}