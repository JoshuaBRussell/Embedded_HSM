#include "Ship.h"


#include "qpc.h"
#include "stdio.h"

//Screen
#include "OLED.h"
#include "mess_transf.h"


//Key Press
#include "BSP.h"

#define X_STEP_UPDATE 3
#define Y_STEP_UPDATE 3

static const uint8_t const Gear_arr[] = {
	0x00, 0x0c, 0xc0, 0x00, 0x00, 0x48, 0x9c, 0x00, 0x00, 0xea, 0x29, 0x80, 0x02, 0x5f, 0xfa, 0x80, 
	0x07, 0x7c, 0x1f, 0x00, 0x02, 0xe0, 0x07, 0x70, 0x13, 0x80, 0x01, 0x90, 0x17, 0x0f, 0x70, 0xe0, 
	0x16, 0x30, 0x04, 0x7c, 0x0c, 0x20, 0x04, 0x34, 0x7c, 0xd3, 0x4b, 0x38, 0x48, 0xae, 0x75, 0x1e, 
	0x39, 0x10, 0x08, 0x9b, 0x91, 0x11, 0x88, 0x8e, 0xd1, 0x30, 0x0c, 0xcc, 0xf1, 0xe4, 0x07, 0x8f, 
	0x31, 0x04, 0x00, 0x0d, 0x91, 0x30, 0x0c, 0xcc, 0xf1, 0x10, 0x08, 0x8e, 0x39, 0x10, 0x08, 0x9f, 
	0x78, 0x2e, 0x74, 0x1a, 0x5c, 0xd3, 0x4b, 0x38, 0x4c, 0x20, 0x04, 0x34, 0x0e, 0x30, 0x04, 0x7c, 
	0x17, 0x0f, 0x70, 0xe0, 0x1b, 0x80, 0x41, 0xd0, 0x02, 0xe0, 0x07, 0xf0, 0x07, 0xf8, 0x1e, 0x80, 
	0x02, 0x5f, 0xfa, 0x80, 0x00, 0x5f, 0xf1, 0x80, 0x00, 0x62, 0x1c, 0x00, 0x00, 0x0c, 0xc0, 0x00
};
static Bitmap Test_T = {Gear_arr, 4, 32};


enum {BSP_TICKS_PER_SEC = 10};


//Declare Ship Active Object
typedef struct{
    QActive super;

    QTimeEvt timeEvt;

    int x;
    int y;
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

    OLED_setup();

    me->x = 0;
    me->y = 0;

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
            printf("Ship-Time_Sig\n");
            
            //Detect Key Press
            BSP_update_KB_state();
            if(BSP_isUpKey_Pressed()){
                me->y-=Y_STEP_UPDATE;
            }
            if(BSP_isLeftKey_Pressed()){
                me->x-=X_STEP_UPDATE;
            }
            if(BSP_isDwnKey_Pressed()){
                me->y+=Y_STEP_UPDATE;
            }
            if(BSP_isRightKey_Pressed()){
                me->x+=X_STEP_UPDATE;
            }



            OLED_clear_frame_buffer();
            OLED_set_bitmap(me->x, me->y, &Test_T);

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