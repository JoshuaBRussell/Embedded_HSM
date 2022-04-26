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


#include "mess_transf.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

#define MAX_MESS_LEN (MESS_HEADER_LEN + MESS_TAIL_LEN + MESS_BUFFER_SIZE)

// Holds the location of the memory before to pull in raw data.
static uint8_t rx_buffer[MESS_BUFFER_SIZE];
static uint8_t tx_buffer[MESS_BUFFER_SIZE];

static int rx_file_descriptor;
static int tx_file_descriptor;




int setup_tx(){

    int tx_setup_status = 0;

    int fifo_creation_status = mkfifo(FIFO_LOC, 0600);
    if(fifo_creation_status < 0){
        int errsv = errno; //Save this before another error has a chance to erase it.
        // The FIFO already existing is not an issue. If the error occurs,
        // ignore it. However, if it is anything else, set the error status
        if (errsv != EEXIST){
            tx_setup_status = -1;
        }        
    }  

    if (!(tx_setup_status < 0))
        tx_file_descriptor = open(FIFO_LOC, O_WRONLY);
        if(tx_file_descriptor < 0){
            int errsv = errno;
            printf("FIFO could not be opended: %s\n", strerror(errsv));
            tx_setup_status = -1;
    }

    return tx_setup_status;
}

int setup_rx(){
    
    //Open a fifo specified by FIFO_LOC.
    rx_file_descriptor = open(FIFO_LOC, O_RDONLY, 0666);
    
    return rx_file_descriptor;
}

int send_tx(void *tx_buff, size_t num_of_bytes){

    int send_status = 0;
    //Check the total number of bytes needed in a buffer to hold the
    //start byte sequence, payload, and end byte sequence
    size_t total_mess_len = (num_of_bytes + MESS_HEADER_LEN + MESS_TAIL_LEN);
    if(total_mess_len > MAX_MESS_LEN){
        errno = EMSGSIZE;
       send_status = -1;
    } else{
        //---Create Message---//
    
        //Set Starter Bytes
        tx_buffer[0] = MESS_HEADER_BYTE;
        tx_buffer[1] = MESS_HEADER_BYTE;
        tx_buffer[2] = MESS_HEADER_BYTE;

        //Copy Contents of Payload into TX Buffer
        for(size_t i = 0; i < num_of_bytes; i++){
            tx_buffer[MESS_HEADER_LEN + i] = *(uint8_t *)(tx_buff+i);
        }

        //Set Tail Bytes
        tx_buffer[MESS_HEADER_LEN + num_of_bytes  ] = MESS_TAIL_BYTE;
        tx_buffer[MESS_HEADER_LEN + num_of_bytes+1] = MESS_TAIL_BYTE;
        tx_buffer[MESS_HEADER_LEN + num_of_bytes+2] = MESS_TAIL_BYTE;
        
        ssize_t bytes_written = write(tx_file_descriptor, tx_buffer, total_mess_len);

        if(bytes_written != total_mess_len){
            send_status = -1;
            errno = EIO;
        }
    }

    

    return send_status;
}


ssize_t receive_rx(void * payload_buff){
    
    int return_status = -1;
    //Check for any new info from pipe
    int num_of_bytes_read =  read(rx_file_descriptor, rx_buffer, MESS_BUFFER_SIZE);
    
    //if message format is correct, then copy the data to a 'frame buffer'
    if(check_if_mess_form_is_correct(rx_buffer, MESS_BUFFER_SIZE)){
        printf("Message Format is Correct.\n");
        printf("Copying over contents...\n");
        memcpy((uint8_t*)payload_buff, (rx_buffer+MESS_HEADER_LEN), MESS_BUFFER_SIZE - MESS_HEADER_LEN - MESS_TAIL_LEN);

        return_status = MESS_BUFFER_SIZE - MESS_HEADER_LEN - MESS_TAIL_LEN;
    }

    //On success, it returns the size of the payload.
    //On failure, it returns a -1
    return return_status;
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
