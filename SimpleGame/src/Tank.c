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

static const uint8_t Tank_arr[] = {
	0x00, 0x3c, 0x00, 0x01, 0xff, 0x80, 0x06, 0x66, 0x60, 0x06, 0x66, 0x60, 0x1e, 0x66, 0x78, 0xfe, 
    0x66, 0x7f, 0xfe, 0x66, 0x7f, 0xfe, 0x66, 0x7f, 0xfe, 0x66, 0x7f, 0x00, 0x00, 0x00
};
static Bitmap Tank_img = {Tank_arr, 3, 10};


enum {BSP_TICKS_PER_SEC = 10};


//Declare Tank Active Object
typedef struct{
    QActive super;

    QTimeEvt timeEvt;

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
    QTimeEvt_ctorX(&me->timeEvt, &me->super, TIME_SIG, 0u);
}

static QState Tank_initial(Tank * const me, void const * const par){
    (void)par;

    QTimeEvt_armX(&me->timeEvt, BSP_TICKS_PER_SEC/2, BSP_TICKS_PER_SEC/2);

    OLED_setup();

    me->x = 52;
    me->y = 50;

    return Q_TRAN(&Tank_Active);
}


static QState Tank_Active(Tank * const me, QEvt const * const e){
    QState status;

    switch(e->sig){
        case Q_ENTRY_SIG: {
            printf("Enter Active.\n");
            status = Q_HANDLED();
            break;
        }

        case TIME_SIG: {
            printf("Tank-Time_Sig\n");
            
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



            OLED_clear_frame_buffer();
            OLED_set_bitmap(me->x, me->y, &Tank_img);

            OLED_send_frame();

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