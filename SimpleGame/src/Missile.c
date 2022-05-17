#include "Missile.h"

#include "qpc.h"

#include "stdio.h"

#include "OLED.h"

static const uint8_t Miss_arr[] = {
	0x00, 0x3c, 0x00, 0x01, 0xff, 0x80, 0x06, 0x66, 0x60, 0x06, 0x66, 0x60, 0x1e, 0x66, 0x78, 0xfe, 
    0x66, 0x7f, 0xfe, 0x66, 0x7f, 0xfe, 0x66, 0x7f, 0xfe, 0x66, 0x7f, 0x00, 0x00, 0x00
};
static Bitmap Miss_img = {Miss_arr, 3, 10};

//Declare Missile Active Object
typedef struct{
    QActive super;

    int x;
    int y;
} Missile;


//Define Missile AO
static Missile local_missile;

//Set the opaque pointer
QActive * const AO_Missile = &local_missile.super;

static QState Missile_initial(Missile * const me, void const * const par);
static QState Missile_Inactive(Missile * const me, QEvt const * const e);
static QState Missile_Active(Missile * const me, QEvt const * const e);


void Missile_ctor(void){
    Missile *me = (Missile *)AO_Missile;
    QActive_ctor(&me->super, Q_STATE_CAST(&Missile_initial));
}

static QState Missile_initial(Missile * const me, void const * const par){
    (void)par;

    QActive_subscribe(&me->super, TIME_SIG);
    QActive_subscribe(&me->super, MISS_FIRE);

    me->x = 52;
    me->y = 50;

    return Q_TRAN(&Missile_Inactive);
}


static QState Missile_Inactive(Missile * const me, QEvt const * const e){
    QState status;

    switch(e->sig){
        case Q_ENTRY_SIG: {
            printf("Enter Missile-Inactive.\n");
            status = Q_HANDLED();
            break;
        }

        case MISS_FIRE: {
            printf("Missile Fired!\n");
            me->x = Q_EVT_CAST(BmpImageEvt)->x;
            me->y = Q_EVT_CAST(BmpImageEvt)->y;
            status = Q_TRAN(&Missile_Active);
            break;
        }

        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }

    return status;
}

static QState Missile_Active(Missile * const me, QEvt const * const e){
    QState status;

    switch(e->sig){
        case Q_ENTRY_SIG: {
            printf("Enter Missile-Active.\n");
            status = Q_HANDLED();
            break;
        }

        case TIME_SIG: {
            
            //Update missile position
            me->y -= 1;

            //Send updated position to World AO to be drawn
            BmpImageEvt *miss_evt = Q_NEW(BmpImageEvt, MISS_POS);
            miss_evt->x = me->x;
            miss_evt->y = me->y;
            miss_evt->bmp_img = &Miss_img;
            QF_PUBLISH((QEvt *)miss_evt, me);


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