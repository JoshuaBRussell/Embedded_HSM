#include "Enemy.h"


#include "qpc.h"
#include "stdio.h"


//Declare Enemy Active Object
typedef struct{
    QActive super;

    int x;
    int y;
} Enemy;


//Define Enemy AO
static Enemy local_enemy;

//Set the oqaque pointer to point to the Enemy super
QActive * const AO_Enemy = &local_enemy.super;

static QState Enemy_initial(Enemy * const me, void const * const par);
static QState Enemy_Inactive(Enemy * const me, QEvt const * const e);



void Enemy_ctor(void){
    Enemy *me = (Enemy *)AO_Enemy;
    QActive_ctor(&me->super, Q_STATE_CAST(&Enemy_initial));
}

static QState Enemy_initial(Enemy * const me, void const * const par){
    (void) par;
    
    me->x = 0;
    me->y = 0;

    return Q_TRAN(&Enemy_Inactive);
}


static QState Enemy_Inactive(Enemy * const me, QEvt const * const e){
    QState status;

    switch(e->sig){
        case Q_ENTRY_SIG: {
            printf("Entry Seq.\n");
            status = Q_HANDLED();
            break;
        }

        default: {
            status = Q_SUPER(&QHsm_top);
        }
    }

    return status;
}