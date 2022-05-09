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

#ifndef _MESS_TRANSF_H_
#define _MESS_TRANSF_H_


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

/*

Simple set of functions for sending and recieving message over byte streams
Currently expects the START_OF_MESS and END_OF_MESS bytes to be at the beginning
and at the end of the message buffer.  

If the message does not align correctly, this just assumes something is wrong and will
keep retrying hoping for the error to correct itself. 
*/


//Needs to be large enough to hold screen data AND the delimitting characters
#define MESS_BUFFER_SIZE 1030 //TODO: Make this dependant on the screen size

//This implementation uses pipes. The name of the FIFO
#define FIFO_LOC "/home/joshua/MyProjects/Embedded_HSM/Repo/Embedded_HSM/SimpleGame/build/screen_fifo" 

#define MESS_HEADER_LEN 3
#define MESS_TAIL_LEN   3

#define MESS_HEADER_BYTE  64 //'@' in ASCII
#define MESS_TAIL_BYTE    33 //'!' in ASCII


bool check_if_mess_form_is_correct(const void * const buffer_start, size_t buffer_size);


int setup_tx();
int setup_rx();

int send_tx(void *tx_buff, size_t num_of_bytes);
ssize_t receive_rx(void * payload_buff);




#endif