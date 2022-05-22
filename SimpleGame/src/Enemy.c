#include "Enemy.h"


#include "qpc.h"
#include "stdio.h"


static const uint8_t Enemy_arr[] = {
	0x00, 0x3c, 0x00, 0x01, 0xff, 0x80, 0x06, 0x66, 0x60, 0x06, 0x66, 0x60, 0x1e, 0x66, 0x78, 0xfe, 
    0x66, 0x7f, 0xfe, 0x66, 0x7f, 0xfe, 0x66, 0x7f, 0xfe, 0x66, 0x7f, 0x00, 0x00, 0x00
};
static Bitmap Enemy_img = {Enemy_arr, 3, 10};


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
static QState Enemy_Active(Enemy * const me, QEvt const * const e);


void Enemy_ctor(void){
    Enemy *me = (Enemy *)AO_Enemy;
    QActive_ctor(&me->super, Q_STATE_CAST(&Enemy_initial));
}

static QState Enemy_initial(Enemy * const me, void const * const par){
    (void) par;

    QActive_subscribe(&me->super, TIME_SIG);
    
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

        case TIME_SIG: {
            status = Q_TRAN(&Enemy_Active);
            break;
        }

        default: {
            status = Q_SUPER(&QHsm_top);
        }
    }

    return status;
}

static QState Enemy_Active(Enemy * const me, QEvt const * const e){
    QState status;

    switch(e->sig){
        case Q_ENTRY_SIG: {
            printf("Entry Seq.\n");
            status = Q_HANDLED();
            break;
        }

        case TIME_SIG: {
            BmpImageEvt *enemy_evt = Q_NEW(BmpImageEvt, ENEMY_POS);
            enemy_evt->x = me->x;
            enemy_evt->y = me->y;
            enemy_evt->bmp_img = &Enemy_img;
            QF_PUBLISH((QEvt *)enemy_evt, me);

            status = Q_HANDLED();
            break;
        }

        default: {
            status = Q_SUPER(&QHsm_top);
        }
    }

    return status;
}