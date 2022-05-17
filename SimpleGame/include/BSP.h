#ifndef _BSP_HEADER_
#define _BSP_HEADER_

#include <stdbool.h>

void BSP_update_KB_state();

bool BSP_isUpKey_Pressed();

bool BSP_isDwnKey_Pressed();

bool BSP_isRightKey_Pressed();

bool BSP_isLeftKey_Pressed();

bool BSP_isActionButtonPressed();




#endif