


#include "BSP.h"

#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <unistd.h>

#define test_bit(yalv, abs_b) ((((char *)abs_b)[yalv/8] & (1<<yalv%8)) > 0)


static uint8_t key_b[KEY_MAX/8 + 1]; //Keyboard Status Memory

void BSP_update_KB_state(){
    int fd;
    fd = open("/dev/input/by-path/pci-0000:00:12.0-usb-0:1:1.0-event-kbd", O_RDONLY);

    memset(key_b, 0, sizeof(key_b));
    ioctl(fd, EVIOCGKEY(sizeof(key_b)), key_b);
    close(fd);

}

bool BSP_isUpKey_Pressed(){

    return test_bit(KEY_W, key_b);
}

bool BSP_isDwnKey_Pressed(){

    return test_bit(KEY_S, key_b);
}

bool BSP_isRightKey_Pressed(){

    return test_bit(KEY_D, key_b);
}

bool BSP_isLeftKey_Pressed(){

    return test_bit(KEY_A, key_b);
}