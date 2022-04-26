#  Copyright (C), 2022 Joshua B. Russell
#  This program is free software: you can redistribute it and/or modify it under
#  the terms of the GNU General Public License as published by the Free 
#  Software Foundation, either version 3 of the License, or (at your option) any
#  later version.
#  This program is distributed in the hope that it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
#  FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
#  details.
#  You should have received a copy of the GNU General Public License along with 
#  this program. If not, see <https://www.gnu.org/licenses/>. 



# PROJECT := simple_screen

# INLCUDES := -I./include/

SRC_DIR := ./src
BIN_DIR := ./build/bin

SCREEN_LIBS += -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

CC   := gcc
LINK := gcc

C_FLAGS := $(INLCUDES)

screen:
	$(CC) $(SRC_DIR)/screen_main.c $(SRC_DIR)/mess_transf.c $(SCREEN_LIBS) -I./include/ -o $(BIN_DIR)/screen_main

writer: 
	$(CC) $(SRC_DIR)/writer.c $(SRC_DIR)/mess_transf.c $(SRC_DIR)/OLED.c -I./include/ -o $(BIN_DIR)/writer
