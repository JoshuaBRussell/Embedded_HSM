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


#ifndef _OLED_H_
#define _OLED_H_


#include <stdint.h>
#include <stdbool.h>


/*
Generic Mockup for an OLED interface. 
*/



//PIXEL_WIDTH and PIXEL_HEIGHT are assumed to be divisible by 8
#define PIXEL_WIDTH  128
#define PIXEL_HEIGHT 64
#define SCREENWIDTH_IN_BYTES PIXEL_WIDTH/8

/*
Bitmap typedef. User can specifiy the image, and then pass it to the OLED 
interface to drawn to the screen. 

bitmap_width  - specifies the width of the image. Must be rounded up to bytes. 
               This means that the byte array may need to be padded to round 
               up.

bitmap_height - specifies the height of the bitmap image. 
*/
typedef struct{
    const uint8_t * const byte_array;
    const uint8_t bitmap_width;
    const uint8_t bitmap_height;
} Bitmap;



//-----Interface----//

/*

Performs any needed setup for the OLED

*/

void OLED_setup();



/*

(x,y) - Screen Coords
1 - Set Pixel High
0 - Clear Pixel

*/

void OLED_set_pixel(const int x, const int y, const bool pixel_status);

/*
- Characters are assumed to be 16x12
- If a screen coord is specified that causes a portion of the character to be 
  past the screen boundary, the portion of the character outside of the screen
  boundary will NOT be drawn (i.e. it will not start on a newline)
- When characters are written, they will overwrite anything in the 16x12
  area designated for the character 
*/
void OLED_set_char(const char char_val, const int x, const int y);


/*

*/
void OLED_set_bitmap(const int x, const int y, const Bitmap * const bitmap_image);

void OLED_clear_frame_buffer();

/*
Once the frame buffer has been adjusted to the user's liking, 
this sets teh screen's output.
*/
int OLED_send_frame();


#endif