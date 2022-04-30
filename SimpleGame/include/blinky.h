#ifndef _BLINKY_H_
#define _BLINKY_H_

#include "qpc.h"

//Event Signal Identifiers
enum BlinkySignals{
    TIMEOUT_SIG = Q_USER_SIG,
    MAX_SIG
};

//Blinky Constructor
void Blinky_ctor(void);

//Opaque pointer for needed access
extern QActive * const AO_Blinky;

#endif