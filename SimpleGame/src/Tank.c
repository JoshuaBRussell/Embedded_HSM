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

#define DYING_TIMER_COUNT 10

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

    int dying_counter;
} Tank;

//Define Tank AO
static Tank local_tank;

//Set the opaque pointer to point the correct
QActive * const AO_Tank = &local_tank.super;

static int max(int x, int y){
    return ((x > y) ? x : y);
}

static int min(int x, int y){
    return ((x < y) ? x : y);
}


static bool doBmpImagesOverlap(int Ax1, int Ay1, int Ax2, int Ay2, 
                               int Bx1, int By1, int Bx2, int By2){
    return (Bx1 < Ax2) && (Bx2 > Ax1) && (By1 < Ay2) && (By2 > Ay1);
}


static QState Tank_initial(Tank * const me, void const * const par);
static QState Tank_Active(Tank * const me, QEvt const * const e);
static QState Tank_Dying(Tank * const me, QEvt const * const e);



void Tank_ctor(void){
    Tank *me = (Tank *)AO_Tank;
    QActive_ctor(&me->super, Q_STATE_CAST(&Tank_initial));
}

static QState Tank_initial(Tank * const me, void const * const par){
    (void)par;

    QActive_subscribe(&me->super, TIME_SIG);
    QActive_subscribe(&me->super, FIRE_POS);

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

            BmpImageEvt *tank_evt = Q_NEW(BmpImageEvt, SHIP_POS);
            tank_evt->x = me->x;
            tank_evt->y = me->y;
            tank_evt->bmp_img = &Tank_img;
            QF_PUBLISH((QEvt *)tank_evt, me);

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

        case FIRE_POS: {

            int fireball_x_pos = Q_EVT_CAST(BmpImageEvt)->x;
            int fireball_y_pos = Q_EVT_CAST(BmpImageEvt)->y;
            int fireball_bmp_width = (Q_EVT_CAST(BmpImageEvt)->bmp_img)->bitmap_width;
            int fireball_bmp_height = (Q_EVT_CAST(BmpImageEvt)->bmp_img)->bitmap_height;

            //Check for overlap between the fireball and tank BMP images.
            bool is_hit = false;
            if (doBmpImagesOverlap(me->x, me->y, me->x + Tank_img.bitmap_width*8, me->y + Tank_img.bitmap_height, 
                                   fireball_x_pos, fireball_y_pos, fireball_x_pos + fireball_bmp_width*8, fireball_y_pos + fireball_bmp_height)){
                
                printf("Overlap Exist!\n");

                //Lowest - Upper Coord
                int overlap_y1 = max(me->y, fireball_y_pos);

                //Highest - Lower Coord
                int overlap_y2 = min(me->y+Tank_img.bitmap_height - 1, fireball_y_pos+fireball_bmp_height - 1);

                //Right Most - Left Coord
                int overlap_x1 = max(me->x, fireball_x_pos);

                //Left Most - Right Coord
                int overlap_x2 = min(me->x + Tank_img.bitmap_width*8 - 1, fireball_x_pos + fireball_bmp_width*8 - 1);


                printf("[1](%d,%d)\n",overlap_x1, overlap_y1);
                printf("[2](%d,%d)\n",overlap_x2, overlap_y2);

                int const MSb_bit_shift[] = {7, 6, 5, 4, 3, 2, 1, 0};

                //The overall method is to look at each absolute pixel value location in the overlap window.
                //The absolute pixel value is converted to a relative pixel value in the respective BMP image.
                //Then the pixel value at the BMP images relative location is found (for both BMP images)
                //Finally, the two are compared to determine if there is an overlap. 

                
                for(int y_index = overlap_y1; y_index <= overlap_y2; y_index++){
                    for(int x_index = overlap_x1; x_index <= overlap_x2; x_index++){

                        //Get Tank Pixel Value
                        int tank_rel_x_coord = (x_index - me->x);
                        int tank_rel_y_coord = (y_index - me->y);

                        int tank_byte_index = (tank_rel_y_coord * Tank_img.bitmap_width) + tank_rel_x_coord/8;
                        int tank_bit_index = MSb_bit_shift[tank_rel_x_coord%8]; 

                        uint8_t row_byte = Tank_arr[tank_byte_index];
                        bool tank_bit = (row_byte >> tank_bit_index) & 0x01;

                        
                        //Get Missile Pixel Value
                        int fireball_rel_x_coord = (x_index - me->x);
                        int fireball_rel_y_coord = (y_index - me->y);

                        int fireball_byte_index = (fireball_rel_y_coord * (Q_EVT_CAST(BmpImageEvt)->bmp_img)->bitmap_width) + fireball_rel_x_coord/8;
                        int fireball_bit_index = MSb_bit_shift[fireball_rel_x_coord%8]; 

                        row_byte = *((Q_EVT_CAST(BmpImageEvt)->bmp_img)->byte_array+ fireball_byte_index);
                        bool fireball_bit = (row_byte >> fireball_bit_index) & 0x01;
                        
                        
                        if(fireball_bit && tank_bit){
                            is_hit = true;
                        } 

                    }
                }

            }
            
            if(is_hit){
                printf("Fireball HIT the Tank! Oh no!");
                QEvt *score_evt = Q_NEW(QEvt, DEC_SCORE);
                QF_PUBLISH(score_evt, me);

                status = Q_TRAN(&Tank_Dying);
           
            } else {
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

static QState Tank_Dying(Tank * const me, QEvt const * const e){
    QState status;

    switch(e->sig){
        case Q_ENTRY_SIG: {

            me->dying_counter = DYING_TIMER_COUNT;
            status = Q_HANDLED();
            break;
        }

        case TIME_SIG: {
 
            me->dying_counter--;

            BmpImageEvt *tank_evt = Q_NEW(BmpImageEvt, SHIP_POS);
            tank_evt->x = me->x;
            tank_evt->y = me->y;
            tank_evt->bmp_img = &Tank_img;
            QF_PUBLISH((QEvt *)tank_evt, me);

            if (me->dying_counter == 0){
                status = Q_TRAN(&Tank_Active);
            } else {
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