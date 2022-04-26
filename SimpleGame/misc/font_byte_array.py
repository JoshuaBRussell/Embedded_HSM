

# Copyright (C), 2022 Joshua B. Russell

# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free 
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.

# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
# details.

# You should have received a copy of the GNU General Public License along with 
# this program. If not, see <https://www.gnu.org/licenses/>. 

import numpy as np
from PIL import Image, ImageFont, ImageDraw


def make_char_array(char, font, threshold, img_size):

    # Used an RGB (and only took the RED pixel data) b/c the PIL default for
    # did not allow for the font size to be changed for bitmaps
    image = Image.new("RGB", img_size, color=0)
    draw = ImageDraw.Draw(image)

    draw.text((0,0), char, font=font, fill=(255, 255, 255))

    image_arr = np.array(image)
    bin_image_arr = image_arr[:, :, 1] > threshold

    pixel_arr = bin_image_arr.astype('uint8')


    char_array_str = '{'
    for row_index in range(img_size[1]):
        
        row = pixel_arr[row_index]

        byte_1 = bytearray(b'\x00')
        byte_2 = bytearray(b'\x00')


        byte_1 |= (row[0] << 7)
        byte_1 |= (row[1] << 6)
        byte_1 |= (row[2] << 5)
        byte_1 |= (row[3] << 4)
        byte_1 |= (row[4] << 3)
        byte_1 |= (row[5] << 2)
        byte_1 |= (row[6] << 1)
        byte_1 |= (row[7] << 0)
        
        byte_2 |= (row[8] << 7)
        byte_2 |= (row[9] << 6)
        byte_2 |= (row[10] << 5)
        byte_2 |= (row[11] << 4)


        char_array_str += format(byte_1[0], '#04X') + ',' + format(byte_2[0], '#04X')
        if (row_index != (img_size[1]-1)):
            char_array_str += ', '


    char_array_str += '}'
    

    char_array_str = char_array_str.replace('X', 'x') # replace the capital 'X' in the HEX prefix
                                        # with a lowercase 'x' - needed since the 
                                        # format specifier to make the alphabetic 
                                        # portion of the HEX value ALSO capitalizes 
                                        # the prefix

    return char_array_str






def main():
    IMG_SIZE = (12, 16) #(WxH)
    INTENSITY_THRESHOLD = 55
    ASCII_SPACE_DEC = 32
    ASCII_TILDE_DEC = 126

    BYTE_ARRAY_NAME = "static uint8_t const font16x12[][] = {"

    font = ImageFont.truetype(font = "/usr/share/fonts/TTF/DejaVuSansMono.ttf", size=16)

    SAVE_FILE = open(r"./../misc/font_byte_array.h", "w")

    SAVE_FILE.write(BYTE_ARRAY_NAME + '\n')

    for i in range(ASCII_SPACE_DEC, ASCII_TILDE_DEC+1):
        char_array_str = make_char_array(chr(i), font, INTENSITY_THRESHOLD, IMG_SIZE)
        
        if (i != ASCII_TILDE_DEC):
            char_array_str += ','                       #Add the comma since this will be part of a 2D array
            char_array_str += ' /* ' + chr(i) + ' */'   #Add a comment stating what the character is
        
        else:
            char_array_str += '  /* ' + chr(i) + ' */'   #Add a comment stating what the character is
        
        print(char_array_str)
        SAVE_FILE.write(4*' ' + char_array_str + '\n')

    SAVE_FILE.write('};')
    SAVE_FILE.close()

if __name__ == "__main__":
    main()

