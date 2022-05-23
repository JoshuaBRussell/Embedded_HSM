#include "Enemy.h"


#include "qpc.h"
#include "stdio.h"
#include "stdbool.h"


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
    QActive_subscribe(&me->super, MISS_POS);
    
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

        case MISS_POS: {

            int miss_x_pos = Q_EVT_CAST(BmpImageEvt)->x;
            int miss_y_pos = Q_EVT_CAST(BmpImageEvt)->y;
            int miss_bmp_width = (Q_EVT_CAST(BmpImageEvt)->bmp_img)->bitmap_width;
            int miss_bmp_height = (Q_EVT_CAST(BmpImageEvt)->bmp_img)->bitmap_height;
            //Check for overlap between the missile and enemy BMP images.
            if (doBmpImagesOverlap(me->x, me->y, me->x + Enemy_img.bitmap_width*8, me->y + Enemy_img.bitmap_height, 
                                   miss_x_pos, miss_y_pos, miss_x_pos + miss_bmp_width*8, miss_y_pos + miss_bmp_height)){
                
                printf("Overlap Exist!\n");

                //Lowest - Upper Coord
                int overlap_y1 = max(me->y, miss_y_pos);

                //Highest - Lower Coord
                int overlap_y2 = min(me->y+Enemy_img.bitmap_height, miss_y_pos+miss_bmp_height);

                //Right Most - Left Coord
                int overlap_x1 = max(me->x, miss_x_pos);

                //Left Most - Right Coord
                int overlap_x2 = min(me->x + Enemy_img.bitmap_width*8, miss_x_pos + miss_bmp_width*8);


                printf("[1](%d,%d)\n",overlap_x1, overlap_y1);
                printf("[2](%d,%d)\n",overlap_x2, overlap_y2);

            }


            status = Q_HANDLED();
            break;
        }

        default: {
            status = Q_SUPER(&QHsm_top);
        }
    }

    return status;
}