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


#include "stdio.h"
#include "stdint.h"
#include <string.h>
#include "raylib.h"
#include <unistd.h>
#include <errno.h>


#include "mess_transf.h"
#include "OLED.h"


#define FAKE_PIXEL_SIZE 1




void print_in_binary(void * const ptr , size_t num_of_bytes) {
    
    
    //Convert the void type pointer to one of an unsigned byte pointer
    uint8_t *byte_ptr = (uint8_t *)ptr;

    size_t byte_index = 0;
    int bit_shift = 0;
    uint8_t printable_byte = 0;

    for (byte_index = 0; byte_index < num_of_bytes; byte_index++){
        printf("Byte[%u]: ", byte_index);
        for(bit_shift = 7; bit_shift >= 0; bit_shift--){
            
            printable_byte = (byte_ptr[byte_index] >> bit_shift) & 0x01;
            printf("%u", printable_byte);    
        }
        
        printf("\n");
    }
    
    printf("\n");
}




int main(){

    const int block_size_in_pixels = FAKE_PIXEL_SIZE;

    const int screenWidth  = block_size_in_pixels * PIXEL_WIDTH;
    const int screenHeight = block_size_in_pixels * PIXEL_HEIGHT;
    InitWindow(screenWidth, screenHeight, "Simple Pixel Screen");
    SetTargetFPS(60);

    Rectangle block_pixels[PIXEL_HEIGHT][PIXEL_WIDTH];
    Color block_colors[PIXEL_HEIGHT][PIXEL_WIDTH];
    for (int j = 0; j < (PIXEL_HEIGHT); j++){
        for (int i = 0; i < (PIXEL_WIDTH); i++){
            block_pixels[j][i].width  = block_size_in_pixels;
            block_pixels[j][i].height = block_size_in_pixels;
            block_pixels[j][i].x = (i*block_size_in_pixels)%screenWidth;
            block_pixels[j][i].y = (j*block_size_in_pixels)%screenHeight;
            block_colors[j][i] = (Color){ GetRandomValue(200, 240), GetRandomValue(200, 240), GetRandomValue(200, 250), 255 };
        }
    }
    // frame_buffer to hold pixel information
    uint8_t frame_buffer1[SCREENWIDTH_IN_BYTES*PIXEL_HEIGHT];
    uint8_t frame_buffer2[SCREENWIDTH_IN_BYTES*PIXEL_HEIGHT];
    uint8_t update_status[SCREENWIDTH_IN_BYTES*PIXEL_HEIGHT];
    memset(frame_buffer1, 0, SCREENWIDTH_IN_BYTES*PIXEL_HEIGHT);
    memset(frame_buffer2, 0, SCREENWIDTH_IN_BYTES*PIXEL_HEIGHT);
    memset(update_status, 1, SCREENWIDTH_IN_BYTES*PIXEL_HEIGHT); //All blocks need to be initialized on startup


    BeginDrawing();
    ClearBackground(BLACK);
    //DrawText("Waiting on Input...", 0, 0, 8, RAYWHITE);
    EndDrawing();

    while(setup_rx() < 0) {
        

        //Is the RX channel just not open? Retry after some time.
        if (errno == ENOENT){
            sleep(1);
        }

        //Is there a different issue? Close the Window and exit. 
        else {
            printf("[Error] Setting up RX failed:  %s\n", strerror(errno));
            CloseWindow();
            return -1;
        }
        
    }

    bool update_screen = false;
    ssize_t payload_size = 0;
    
    //main game loop
    while(!WindowShouldClose()){
        
        payload_size = receive_rx(frame_buffer1);
        printf("Payload Size: %i\n ", payload_size);

        //Check to see if a full frame's worth of data was sent
        update_screen = (payload_size == SCREENWIDTH_IN_BYTES*PIXEL_HEIGHT);
        
        //Check to see if there was any actual change in the data
        if(update_screen){
            for (int i = 0; i < SCREENWIDTH_IN_BYTES*PIXEL_HEIGHT; i++){
                update_status[i] = frame_buffer1[i] ^ frame_buffer2[i];
            }
        }


        
        //Redraw the screen
        BeginDrawing();

        //ClearBackground(RAYWHITE);

        int bit_shift_array[] = {7, 6, 5, 4, 3, 2, 1, 0};

        bool update_pixel_value = false;
        bool pixel_value = false;
        if(update_screen){

        
            for (int row_index = 0; row_index < PIXEL_HEIGHT; row_index++){
                for(int col_index = 0; col_index < PIXEL_WIDTH; col_index++) {
                    
                    int byte_index = SCREENWIDTH_IN_BYTES*row_index + col_index/8;
                    uint8_t bit_shift = bit_shift_array[col_index%8];

                    update_pixel_value = (update_status[byte_index] >> bit_shift) & 0x01;

                    pixel_value = (frame_buffer1[byte_index] >> bit_shift) & 0x01;

                    if (update_pixel_value){
                        //printf("Updated Pixel: [%i][%i]\n", row_index, col_index);
                        if(pixel_value){
                            DrawRectangleRec(block_pixels[row_index][col_index], (Color){ 255, 255, 255, 255 });  
                        } else {
                            DrawRectangleRec(block_pixels[row_index][col_index], (Color){ 0, 0, 0, 255 });
                        }                   
                    }
                    
                }
            }


            //swap the data from frame_buffer1 into 2
            for (int i = 0; i < SCREENWIDTH_IN_BYTES*PIXEL_HEIGHT; i++){
                frame_buffer2[i] = frame_buffer1[i];
            }
        }
        

        EndDrawing();   
    }
    
    CloseWindow();

    return 0;
}
