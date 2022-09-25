#include "Tank.h"


#include "qpc.h"
#include "stdio.h"

//Screen
#include "OLED.h"
#include "mess_transf.h"


//Key Press
#include "BSP.h"

#define X_STEP_UPDATE 3
#define Y_STEP_UPDATE 3

#define MISS_CUSTOM_OFFSET_X 9

static const uint8_t Tank_arr[] = {
	0x00, 0x3c, 0x00, 0x01, 0xff, 0x80, 0x06, 0x66, 0x60, 0x06, 0x66, 0x60, 0x1e, 0x66, 0x78, 0xfe, 
    0x66, 0x7f, 0xfe, 0x66, 0x7f, 0xfe, 0x66, 0x7f, 0xfe, 0x66, 0x7f, 0x00, 0x00, 0x00
};
static Bitmap Tank_img = {Tank_arr, 3, 10};


enum {BSP_TICKS_PER_SEC = 10};


//Declare Tank Active Object
typedef struct{
    QActive super;

    int x;
    int y;
} Tank;

//Define Tank AO
static Tank local_tank;

//Set the opaque pointer to point the correct
QActive * const AO_Tank = &local_tank.super;


static QState Tank_initial(Tank * const me, void const * const par);
static QState Tank_Active(Tank * const me, QEvt const * const e);



void Tank_ctor(void){
    Tank *me = (Tank *)AO_Tank;
    QActive_ctor(&me->super, Q_STATE_CAST(&Tank_initial));
}

static QState Tank_initial(Tank * const me, void const * const par){
    (void)par;

    QActive_subscribe(&me->super, TIME_SIG);

    OLED_setup();
    BSP_setup();

    me->x = 52;
    me->y = 50;

    return Q_TRAN(&Tank_Active);
}


static QState Tank_Active(Tank * const me, QEvt const * const e){
    QState status;

    switch(e->sig){
        case Q_ENTRY_SIG: {
            status = Q_HANDLED();
            break;
        }

        case TIME_SIG: {
            
            //Detect Key Press
            BSP_update_KB_state();
            // if(BSP_isUpKey_Pressed()){
            //     me->y-=Y_STEP_UPDATE;
            // }
            if(BSP_isLeftKey_Pressed()){
                me->x-=X_STEP_UPDATE;
            }
            // if(BSP_isDwnKey_Pressed()){
            //     me->y+=Y_STEP_UPDATE;
            // }
            if(BSP_isRightKey_Pressed()){
                me->x+=X_STEP_UPDATE;
            }

            BmpImageEvt *ship_evt = Q_NEW(BmpImageEvt, SHIP_POS);
            ship_evt->x = me->x;
            ship_evt->y = me->y;
            ship_evt->bmp_img = &Tank_img;
            QF_PUBLISH((QEvt *)ship_evt, me);

            if(BSP_isActionButtonPressed()){
                BmpImageEvt *miss_fire_evt = Q_NEW(BmpImageEvt, MISS_FIRE);
                miss_fire_evt->x = me->x+MISS_CUSTOM_OFFSET_X;
                miss_fire_evt->y = me->y;
                miss_fire_evt->bmp_img = &Tank_img;
                QF_PUBLISH((QEvt *)miss_fire_evt, me);
            }

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