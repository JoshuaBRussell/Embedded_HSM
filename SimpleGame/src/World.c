#include "World.h"

#include "qpc.h"

#include "stdio.h"


//Screen Includes
#include "OLED.h"
#include "mess_transf.h"


//Declare World Active Object
typedef struct{
    QActive super;
} World;


//Define Tank AO
static World local_world;

//Set the opaque pointer to point to the World AO
QActive * const AO_World = &local_world.super;

static QState World_initial(World * const me, void const * const par);
static QState World_Active(World * const me, QEvt const * const e);


void World_ctor(void){
    World *me = (World *)AO_World;
    QActive_ctor(&me->super, Q_STATE_CAST(&World_initial));
}

static QState World_initial(World * const me, void const * const par){
    (void)par;

    QActive_subscribe(&me->super, TIME_SIG);
    QActive_subscribe(&me->super, SHIP_POS);

    return Q_TRAN(&World_Active);
}

static QState World_Active(World * const me, QEvt const * const e){
    QState status;

    switch(e->sig){
        case Q_ENTRY_SIG: {
            printf("Enter World-Active.\n");
            status = Q_HANDLED();
            break;
        }

        case TIME_SIG: {
            printf("World-TIME_SIG.\n");
            status = Q_HANDLED();
            break;
        }

        case SHIP_POS: {

            printf("Got Ship Position.\n");
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