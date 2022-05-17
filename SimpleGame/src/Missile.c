#include "Missile.h"

#include "qpc.h"

#include "stdio.h"

#include "OLED.h"

#define MISS_PIXEL_PER_TICK 3

static const uint8_t Miss_arr[] = {
	0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x78, 0x00, 0x00, 0x00
};
static Bitmap Miss_img = {Miss_arr, 1, 15};

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
            me->y -= MISS_PIXEL_PER_TICK;

            //Send updated position to World AO to be drawn
            BmpImageEvt *miss_evt = Q_NEW(BmpImageEvt, MISS_POS);
            miss_evt->x = me->x;
            miss_evt->y = me->y;
            miss_evt->bmp_img = &Miss_img;
            QF_PUBLISH((QEvt *)miss_evt, me);

            if(me->y <= -Miss_img.bitmap_height){
                status = Q_TRAN(&Missile_Inactive);
            } else
            {
                status = Q_HANDLED();    
            }
            
            break;
        }

        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }

    return status;
}