#include "World.h"

#include "qpc.h"

#include "stdio.h"


//Screen Includes
#include "OLED.h"
#include "mess_transf.h"


#define WINNING_SCORE 3

//Declare World Active Object
typedef struct{
    QActive super;

    int score;
} World;


//Define Tank AO
static World local_world;

//Set the opaque pointer to point to the World AO
QActive * const AO_World = &local_world.super;

static void dispaly_score(int score){
    OLED_set_char(' ', 0, 0);
    OLED_set_char(' ', 12, 0);
    if(score >= 0){
        OLED_set_char((char)(score+'0'), 0, 0);
    } else {
        OLED_set_char('~', 0, 0);
        OLED_set_char((char)(-1*score+'0'), 12, 0);
    }
    
}

static QState World_initial(World * const me, void const * const par);
static QState World_Active(World * const me, QEvt const * const e);
static QState World_Won(World * const me, QEvt const * const e);


void World_ctor(void){
    World *me = (World *)AO_World;
    QActive_ctor(&me->super, Q_STATE_CAST(&World_initial));
}

static QState World_initial(World * const me, void const * const par){
    (void)par;

    QActive_subscribe(&me->super, TIME_SIG);
    QActive_subscribe(&me->super, SHIP_POS);
    QActive_subscribe(&me->super, MISS_POS);
    QActive_subscribe(&me->super, ENEMY_POS);
    QActive_subscribe(&me->super, INC_SCORE);
    QActive_subscribe(&me->super, FIRE_POS);

    OLED_setup();

    me->score = 0;

    return Q_TRAN(&World_Active);
}

static QState World_Active(World * const me, QEvt const * const e){
    QState status;

    switch(e->sig){
        case Q_ENTRY_SIG: {
            status = Q_HANDLED();
            break;
        }

        case TIME_SIG: {
            dispaly_score(-2);
            
            OLED_send_frame();
            OLED_clear_frame_buffer();
            status = Q_HANDLED();
            break;
        }

        case SHIP_POS: {
            //Update OLED Frame Buffer with the ship's current location
            OLED_set_bitmap(Q_EVT_CAST(BmpImageEvt)->x, 
                            Q_EVT_CAST(BmpImageEvt)->y, 
                            Q_EVT_CAST(BmpImageEvt)->bmp_img);
            status = Q_HANDLED();
            break;
        }

        case MISS_POS: {
            //Update OLED Frame Buffer with the missile's current location
            OLED_set_bitmap(Q_EVT_CAST(BmpImageEvt)->x,
                            Q_EVT_CAST(BmpImageEvt)->y,
                            Q_EVT_CAST(BmpImageEvt)->bmp_img);
            status = Q_HANDLED();
            break;
        }

        case ENEMY_POS: {
            //Update OLED Frame Buffer with the missile's current location
            OLED_set_bitmap(Q_EVT_CAST(BmpImageEvt)->x,
                            Q_EVT_CAST(BmpImageEvt)->y,
                            Q_EVT_CAST(BmpImageEvt)->bmp_img);
            status = Q_HANDLED();
            break;
        }

        case INC_SCORE: {
            
            me->score+=1;

            if (me->score < WINNING_SCORE){
                status = Q_HANDLED();
            } else {
                status = Q_TRAN(&World_Won);
            }
            
            break;
        }

        case FIRE_POS: {
            //Update OLED Frame Buffer with the ship's current location
            OLED_set_bitmap(Q_EVT_CAST(BmpImageEvt)->x, 
                            Q_EVT_CAST(BmpImageEvt)->y, 
                            Q_EVT_CAST(BmpImageEvt)->bmp_img);
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

static QState World_Won(World * const me, QEvt const * const e){
    QState status;

    switch(e->sig){
        case Q_ENTRY_SIG: {
            status = Q_HANDLED();
            break;
        }

        case TIME_SIG: {
            
            OLED_set_char('Y', 12, 24);
            OLED_set_char('o', 24, 24);
            OLED_set_char('u', 36, 24);
            OLED_set_char(' ', 48, 24);
            OLED_set_char('W', 60, 24);
            OLED_set_char('o', 72, 24);
            OLED_set_char('n', 84, 24);
            OLED_set_char('!', 96, 24);
            
            OLED_send_frame();
            OLED_clear_frame_buffer();
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