#include "Ship.h"


#include "qpc.h"
#include "stdio.h"




//Key Press
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#define NB_DISABLE 1
#define NB_ENABLE 0


int kbhit(){
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}

void nonblock(int state)
{
    struct termios ttystate;

    //get the terminal state
    tcgetattr(STDIN_FILENO, &ttystate);

    if (state==NB_ENABLE)
    {
        //turn off canonical mode
        ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
    }
    else if (state==NB_DISABLE)
    {
        //turn on canonical mode
        ttystate.c_lflag |= ICANON;
    }
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);

}

enum {BSP_TICKS_PER_SEC = 2};


//Declare Ship Active Object
typedef struct{
    QActive super;

    QTimeEvt timeEvt;
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

            char c;
            //Detect Key Press
            nonblock(NB_ENABLE);
            int i = kbhit();
            if(i != 0){
               c = fgetc(stdin);
            }
            nonblock(NB_DISABLE);

            if (c == 'w'){
                printf("Move Up\n");
            } 
            if (c == 'a'){
                printf("Move Left\n");
            }
            if (c == 's'){
                printf("Move Down\n");
            } 
            if (c == 'd'){
                printf("Move Right\n");
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