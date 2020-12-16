// See:
//  https://github.com/m5stack/M5Stack/tree/master/examples/Advanced/Display/drawXBitmap
// 
// Images can be converted to XBM format by using the online converter here:
// https://www.online-utility.org/image/convert/to/XBM

// The output must be pasted in a header file, renamed and adjusted to appear
// as as a const unsigned char array in PROGMEM (FLASH program memory).

// The xbm format adds padding to pixel rows so they are a whole number of bytes
// For example: 50 pixel width means 56 bits = 7 bytes
// the 50 height then means array uses 50 x 7 = 350 bytes of FLASH
// The library ignores the padding bits when drawing the image on the display.

#include <pgmspace.h>  // PROGMEM support header

// snowflake image 13 x 13 pixel array in XBM format
#define flakeWidth  13
#define flakeHeight 13

// Image is stored in this array
PROGMEM const unsigned char snowflake[] = {
  0x08, 0x02, 0x18, 0x03, 0xB8, 0x03, 0xB7, 0x1D, 0xAE, 0x0E, 0x1C, 0x07, 
  0x40, 0x00, 0x1C, 0x07, 0xAE, 0x0E, 0xB7, 0x1D, 0xB8, 0x03, 0x18, 0x03, 
  0x08, 0x02};