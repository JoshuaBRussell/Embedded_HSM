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
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define SCREENWIDTH  24                    //Must be evenly divisible by 8
#define SCREENWIDTH_IN_BYTES SCREENWIDTH/8
#define SCREENHEIGHT 8

//Needs to be large enough to hold screen data AND the delimitting characters
#define MESS_BUFFER_SIZE 29 //TODO: Make this dependant on the screen size

#define MESS_HEADER_LEN 3
#define MESS_TAIL_LEN   3

#define MESS_HEADER_BYTE  64 //'@' in ASCII
#define MESS_TAIL_BYTE    33 //'!' in ASCII



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


bool check_if_mess_form_is_correct(const void * const buffer_start, size_t buffer_size){
        
        //Currently, this function is making the assumption that a  
        //complete message fills the buffer and the two are the same size

        //Just make sure that a full frame's worth of data shows up in the message.
        //If not, then return false. 
        if (buffer_size != MESS_BUFFER_SIZE){
            return false;
        }

        //Convert this to a uint8_t pointer, so I can think in terms of bytes, regardless of what data type the buffer is.
        const uint8_t * const buf = buffer_start;

        bool message_start_found  = false;
        bool delims_are_correct = false;
        bool is_end_seq_correct   = false;

        
        //Check the start bytes...this assumes they are at the start of the buffer
        for (uint8_t start_seq_index = 0; start_seq_index < MESS_HEADER_LEN; start_seq_index++){
            //If at any point an incorrect byte is found, break out of the loop
            //leaving the message_start_found flag as false...
            if (buf[start_seq_index] != MESS_HEADER_BYTE){
                break;
            }
            //With the previous condition, if the following condition passes,
            //the message start is assumed to be found
            if (start_seq_index == (MESS_HEADER_LEN - 1)){
                message_start_found = true;
                printf("Message Start Found Successfuly\n");
            }
        }
        
        
        //check for the end sequence
        if (message_start_found){
            size_t end_seq_start_index = (buffer_size - MESS_TAIL_LEN);
            for (uint8_t end_seq_index = 0; end_seq_index < MESS_TAIL_LEN; end_seq_index++){
                
                //If at any point an incorrect byte is found, break out of the loop
                //leaving the message_start_found flag as false...
                if (buf[end_seq_start_index + end_seq_index] != MESS_TAIL_BYTE){
                    break;
                }
                //With the previous condition, if the following condition passes,
                //the message start is assumed to be found
                if (end_seq_index == (MESS_TAIL_LEN - 1)){
                    is_end_seq_correct = true;
                    printf("End Seq\n");
                }
            }

        }
             

    return (message_start_found && is_end_seq_correct);

}


int main(){

    int fd;
    char * myfifo = "./screen_fifo";
    char message_buf[MESS_BUFFER_SIZE];
    char frame_buffer[SCREENWIDTH_IN_BYTES*SCREENHEIGHT];
        
    size_t num_of_bytes_read = 0;

    fd = open(myfifo, O_RDONLY);
    while(1){
        num_of_bytes_read = read(fd, message_buf, MESS_BUFFER_SIZE);

        //Need to check if bytes were actually received...
        if (num_of_bytes_read == -1){
            printf("Error Reading Data from Pipe.\n");
        }
        if(num_of_bytes_read == 0){
            printf("No Bytes read from pipe.\n");
        }
        if(num_of_bytes_read == MESS_BUFFER_SIZE){
            printf("Number of bytes equals that of buffer.\n");
        }
        if(num_of_bytes_read < MESS_BUFFER_SIZE){
            printf("Less bytes recieved than in buffer");
        }
       
        //if message format is correct, then copy the data to a 'frame buffer'
        if(check_if_mess_form_is_correct(message_buf, MESS_BUFFER_SIZE)){
            printf("Message Format is Correct.\n");
            printf("Copying over contents...\n");
            memcpy(frame_buffer, (message_buf+MESS_HEADER_LEN), MESS_BUFFER_SIZE - MESS_HEADER_LEN - MESS_TAIL_LEN);

            //display the data in the buffer
            print_in_binary(frame_buffer, MESS_BUFFER_SIZE - MESS_HEADER_LEN - MESS_TAIL_LEN);

        }

        



        //If not, make no updates to the screen
        printf("Received: %s\n\n\n", message_buf);
        //
    }
    
    
    close(fd);



    return 0;
}



//Switch from thinking in terms of char/str to uint8_t (i.e. bytes)