/*
 Copyright (C), 2022 Joshua B. Russell
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free 
 Software Foundation, either version 3 of the License, or (at your option) any
 later version.
 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 details.
 You should have received a copy of the GNU General Public License along with 
 this program. If not, see <https://www.gnu.org/licenses/>. 
*/


#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "OLED.h"
#include "mess_transf.h"

static const uint8_t const Gear_arr[] = {
	0x00, 0x0c, 0xc0, 0x00, 0x00, 0x48, 0x9c, 0x00, 0x00, 0xea, 0x29, 0x80, 0x02, 0x5f, 0xfa, 0x80, 
	0x07, 0x7c, 0x1f, 0x00, 0x02, 0xe0, 0x07, 0x70, 0x13, 0x80, 0x01, 0x90, 0x17, 0x0f, 0x70, 0xe0, 
	0x16, 0x30, 0x04, 0x7c, 0x0c, 0x20, 0x04, 0x34, 0x7c, 0xd3, 0x4b, 0x38, 0x48, 0xae, 0x75, 0x1e, 
	0x39, 0x10, 0x08, 0x9b, 0x91, 0x11, 0x88, 0x8e, 0xd1, 0x30, 0x0c, 0xcc, 0xf1, 0xe4, 0x07, 0x8f, 
	0x31, 0x04, 0x00, 0x0d, 0x91, 0x30, 0x0c, 0xcc, 0xf1, 0x10, 0x08, 0x8e, 0x39, 0x10, 0x08, 0x9f, 
	0x78, 0x2e, 0x74, 0x1a, 0x5c, 0xd3, 0x4b, 0x38, 0x4c, 0x20, 0x04, 0x34, 0x0e, 0x30, 0x04, 0x7c, 
	0x17, 0x0f, 0x70, 0xe0, 0x1b, 0x80, 0x41, 0xd0, 0x02, 0xe0, 0x07, 0xf0, 0x07, 0xf8, 0x1e, 0x80, 
	0x02, 0x5f, 0xfa, 0x80, 0x00, 0x5f, 0xf1, 0x80, 0x00, 0x62, 0x1c, 0x00, 0x00, 0x0c, 0xc0, 0x00
};
static Bitmap Test_T = {Gear_arr, 4, 32};

int main(){

    OLED_setup();

    int send_status;
    while (1){
        for (uint8_t i = 0; i < 120; i++){

            // OLED_set_bitmap(0, 0, &Test_T);
            OLED_set_bitmap(48, 16, &Test_T);
            // OLED_set_char('F', 0, 0);
            // OLED_set_char('o',12, 0);
            // OLED_set_char('x',24, 0);
            // OLED_set_char('y',36, 0);

            // OLED_set_char('D',60, 0);
            // OLED_set_char('u',72, 0);
            // OLED_set_char('d',84, 0);
            // OLED_set_char('e',96, 0);

            //OLED_set_pixel(2, 0, 1);
            
            send_status = OLED_send_frame();
            if(send_status < 0){
                printf("Send Failed. Error: %s\n", strerror(errno));
            }
            sleep(1);
        }
        
    
    }

    unlink(FIFO_LOC);

    return 0;
}