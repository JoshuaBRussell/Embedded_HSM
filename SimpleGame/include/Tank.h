#ifndef _TANK_AO_
#define _TANK_AO_

#include "qpc.h"


//Tank Event Identifiers
enum TankSignals{
    TIME_SIG = Q_USER_SIG,
    MAX_SIG
};

//Tank Constructor
void Tank_ctor(void);

//Opaque Pointer
extern QActive * const AO_Tank;

#endif