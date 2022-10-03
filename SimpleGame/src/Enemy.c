#include "Enemy.h"


#include "qpc.h"
#include "stdio.h"
#include "stdbool.h"

#define DYING_TIMER_COUNT 10

static int const x_enemy_pos[] = {0, 1, 2, 5, 8, 13, 18, 24, 31, 38, 46, 54, 62, 70, 78, 
                                  86, 94, 101, 107, 113, 118, 122, 125, 127, 128, 128, 127, 
                                  125, 122, 118, 113, 107, 101, 94, 86, 78, 70, 62, 54, 46, 
                                  38, 31, 24, 18, 13, 8, 5, 2, 1, 0};
static int const y_enemy_pos[] = {14, 11, 8, 5, 3, 2, 1, 2, 3, 5, 7, 10, 13, 17, 20, 23, 
                                  25, 26, 27, 27, 26, 24, 22, 19, 16, 13, 10, 7, 4, 2, 1, 
                                  1, 2, 4, 6, 9, 12, 15, 18, 21, 24, 26, 27, 27, 27, 25, 23, 
                                  20, 17, 14};


static const uint8_t Enemy_arr[] = {
	0x1f, 0x80, 0x3f, 0xc0, 0x7f, 0xe0, 0x7f, 0xe0, 0xff, 0xf0, 0xff, 0xf0, 0xbf, 0xd0, 0xf9, 0xf0, 
	0xff, 0xf0, 0x00, 0x00, 0x46, 0x20, 0x49, 0x20, 0x70, 0xe0, 0x69, 0x60, 0x0f, 0x00, 0x26, 0x40, 
	0x20, 0x40, 0x06, 0x00, 0x1f, 0x80, 0x0f, 0x00
};
static Bitmap Enemy_img = {Enemy_arr, 2, 20};

static const uint8_t Enemy_Dying1_arr[] = {
	0x1f, 0x80, 0x3f, 0xc0, 0x3f, 0xe0, 0x5f, 0xc0, 0xef, 0xf0, 0xff, 0xf0, 0xbf, 0xd0, 0xd9, 0xf0, 
	0xff, 0xf0, 0x00, 0x00, 0x46, 0x20, 0x49, 0x20, 0x70, 0xe0, 0x69, 0x60, 0x0f, 0x00, 0x26, 0x40, 
	0x20, 0x40, 0x02, 0x00, 0x1f, 0x80, 0x0f, 0x00
};
static Bitmap Enemy_Dying1_img = {Enemy_Dying1_arr, 2, 25};

static const uint8_t Enemy_Dying2_arr[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x08, 0x3a, 0x08, 0x32, 0x1c, 0x76, 0x0a, 0x76, 0x06, 
	0x79, 0x16, 0x5c, 0x1a, 0x5d, 0x14, 0x21, 0x04, 0x31, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 
	0x1c, 0x38, 0x18, 0x58, 0x03, 0xc0, 0x09, 0x90, 0x08, 0x10, 0x01, 0x80, 0x07, 0xe0, 0x00, 0x00, 
	0x00, 0x00
};
static Bitmap Enemy_Dying2_img = {Enemy_Dying2_arr, 2, 25};


//Declare Enemy Active Object
typedef struct{
    QActive super;

    int x;
    int y;

    uint8_t dying_counter;
    uint8_t timing_counter;
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
static QState Enemy_Dying1(Enemy * const me, QEvt const * const e);
static QState Enemy_Dying2(Enemy * const me, QEvt const * const e);


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
            me->dying_counter = 90; //Use the dying counter to delay the (re)spawn of the enemy.
            status = Q_HANDLED();
            break;
        }

        case TIME_SIG: {
            me->dying_counter--;
            if(me->dying_counter == 0){
                status = Q_TRAN(&Enemy_Active);
            } else {
                status = Q_HANDLED();
            }
            
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
            me->timing_counter = 0;
            status = Q_HANDLED();
            break;
        }

        case TIME_SIG: {

            me->x = x_enemy_pos[me->timing_counter%50];
            me->y = y_enemy_pos[me->timing_counter%50];
            me->timing_counter++;

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
            bool is_hit = false;
            if (doBmpImagesOverlap(me->x, me->y, me->x + Enemy_img.bitmap_width*8, me->y + Enemy_img.bitmap_height, 
                                   miss_x_pos, miss_y_pos, miss_x_pos + miss_bmp_width*8, miss_y_pos + miss_bmp_height)){
                
                printf("Overlap Exist!\n");

                //Lowest - Upper Coord
                int overlap_y1 = max(me->y, miss_y_pos);

                //Highest - Lower Coord
                int overlap_y2 = min(me->y+Enemy_img.bitmap_height - 1, miss_y_pos+miss_bmp_height - 1);

                //Right Most - Left Coord
                int overlap_x1 = max(me->x, miss_x_pos);

                //Left Most - Right Coord
                int overlap_x2 = min(me->x + Enemy_img.bitmap_width*8 - 1, miss_x_pos + miss_bmp_width*8 - 1);


                printf("[1](%d,%d)\n",overlap_x1, overlap_y1);
                printf("[2](%d,%d)\n",overlap_x2, overlap_y2);

                int const MSb_bit_shift[] = {7, 6, 5, 4, 3, 2, 1, 0};

                //The overall method is to look at each absolute pixel value location in the overlap window.
                //The absolute pixel value is converted to a relative pixel value in the respective BMP image.
                //Then the pixel value at the BMP images relative location is found (for both BMP images)
                //Finally, the two are compared to determine if there is an overlap. 

                
                for(int y_index = overlap_y1; y_index <= overlap_y2; y_index++){
                    for(int x_index = overlap_x1; x_index <= overlap_x2; x_index++){

                        //Get Enemy Pixel Value
                        int enemy_rel_x_coord = (x_index - me->x);
                        int enemy_rel_y_coord = (y_index - me->y);

                        int enemy_byte_index = (enemy_rel_y_coord * Enemy_img.bitmap_width) + enemy_rel_x_coord/8;
                        int enemy_bit_index = MSb_bit_shift[enemy_rel_x_coord%8]; 

                        uint8_t row_byte = Enemy_arr[enemy_byte_index];
                        bool enemy_bit = (row_byte >> enemy_bit_index) & 0x01;

                        
                        //Get Missile Pixel Value
                        int miss_rel_x_coord = (x_index - me->x);
                        int miss_rel_y_coord = (y_index - me->y);

                        int miss_byte_index = (miss_rel_y_coord * (Q_EVT_CAST(BmpImageEvt)->bmp_img)->bitmap_width) + miss_rel_x_coord/8;
                        int miss_bit_index = MSb_bit_shift[miss_rel_x_coord%8]; 

                        row_byte = *((Q_EVT_CAST(BmpImageEvt)->bmp_img)->byte_array+ miss_byte_index);
                        bool miss_bit = (row_byte >> miss_bit_index) & 0x01;
                        
                        
                        if(miss_bit && enemy_bit){
                            is_hit = true;
                        } 

                    }
                }

            }

            if(is_hit){
                
                QEvt *score_evt = Q_NEW(QEvt, INC_SCORE);
                QF_PUBLISH(score_evt, me);

                status = Q_TRAN(&Enemy_Dying1);
            } else {
                    status = Q_HANDLED();
            }

            break;
        }

        default: {
            status = Q_SUPER(&QHsm_top);
        }
    }

    return status;
}


static QState Enemy_Dying1(Enemy * const me, QEvt const * const e){
    QState status;

    switch (e->sig){
        case Q_ENTRY_SIG: {
            printf("Enter Dying 1\n");
            me->dying_counter = DYING_TIMER_COUNT;
            status = Q_HANDLED();
            break;
        }

        case TIME_SIG: {

            me->dying_counter--;

            BmpImageEvt *enemy_evt = Q_NEW(BmpImageEvt, ENEMY_POS);
            enemy_evt->x = me->x;
            enemy_evt->y = me->y;
            enemy_evt->bmp_img = &Enemy_Dying1_img;
            QF_PUBLISH((QEvt *)enemy_evt, me);

            if(me->dying_counter == 0){
                printf("Done Dying\n");
                status = Q_TRAN(&Enemy_Dying2);
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

static QState Enemy_Dying2(Enemy * const me, QEvt const * const e){
    QState status;

    switch (e->sig){
        case Q_ENTRY_SIG: {
            me->dying_counter = DYING_TIMER_COUNT;
            status = Q_HANDLED();
            break;
        }

        case TIME_SIG: {
            
            me->dying_counter--;

            BmpImageEvt *enemy_evt = Q_NEW(BmpImageEvt, ENEMY_POS);
            enemy_evt->x = me->x;
            enemy_evt->y = me->y;
            enemy_evt->bmp_img = &Enemy_Dying2_img;
            QF_PUBLISH((QEvt *)enemy_evt, me);

            if(me->dying_counter == 0){
                status = Q_TRAN(&Enemy_Inactive);
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
