#ifndef _SHIP_AO_
#define _SHIP_AO_

#include "qpc.h"


//Ship Event Identifiers
enum BlinkySignals{
    TIME_SIG = Q_USER_SIG,
    MAX_SIG
};

//Ship Constructor
void Ship_ctor(void);

//Opaque Pointer
extern QActive * const AO_Ship;

#endif