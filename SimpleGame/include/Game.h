#ifndef _GAME_H_
#define _GAME_H_

#include "qpc.h"
#include "OLED.h"

enum GAME_Signals{
    TIME_SIG = Q_USER_SIG,
    SHIP_POS,
    MISS_POS,
    ENEMY_POS,
    MISS_FIRE,
    MAX_SIG
};


//Game Events
typedef struct{
    QEvt super;

    int x;
    int y;
    Bitmap *bmp_img;
} BmpImageEvt;



#endif