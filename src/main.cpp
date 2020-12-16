/**************************************************************************
 * M5Stack IMU Snow-Globe
 * 
 * A simple program that turns an M5Stack Fire into a digital snow globe.
 * Note: The PSRAM is needed to fit the fullscreen sprite in 16bit.
 * The PSRAM must be activated in the compiler!
 * 
 * Hague Nusseck @ electricidea
 * v0.01 16.December.2020
 * https://github.com/electricidea/M5Stack-Snow-Globe
 * 
 * 
 * Distributed as-is; no warranty is given.
**************************************************************************/

#include <Arduino.h>

// This definition must be placed before the #include <M5Stack.h>
// #define M5STACK_MPU6886 
#define M5STACK_MPU9250 
// #define M5STACK_MPU6050
// #define M5STACK_200Q
#include <M5Stack.h>
// install the library:
// pio lib install "M5Stack"

// Free Fonts for nice looking fonts on the screen
#include "Free_Fonts.h"

// logo with 150x150 pixel size in XBM format
// check the file header for more information
#include "electric-idea_logo.h"

// Stuff for the Graphical output
// The M5Stack screen pixel is 320x240, with the top left corner of the screen as the origin (0,0)
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

float accX = 0.0F;
float accY = 0.0F;
float accZ = 0.0F;

/*

Direction of acceleration:

      . down = +z
  - - - - -
  |       |
  |       | --> -x
  | O O O |
  - - - - - 
      |
      V
     +y 
*/


// bitmap of one snowflake (XBM Format)
#include "snowflake.h"

// background image as uint16_t RGB565 array
#include "background_image.h"

// structure for thr position of every snow flake
const int flake_max = 250;
struct flakeObject
{
  int32_t x;
  int32_t y;
  float speed;
};

flakeObject flakeArray[flake_max];

// Sprite object "img" with pointer to "M5.Lcd" object
// the pointer is used by pushSprite() to push it onto the LCD
TFT_eSprite img = TFT_eSprite(&M5.Lcd);  

void setup(void) {
  M5.begin();
  M5.Power.begin();
  M5.IMU.Init();
  // int the starting position of all snowflakes
  for(int i=0; i < flake_max; i++){
    // horizontally distributed
    flakeArray[i].x = random(SCREEN_WIDTH);
    // at the bottom of the screen
    flakeArray[i].y = SCREEN_HEIGHT-random(20);
    // individual speed for each snowflake
    flakeArray[i].speed = (random(80)+20)/100.0;
  }
  // electric-idea logo
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.drawXBitmap((int)(320-logoWidth)/2, (int)(240-logoHeight)/2, logo, logoWidth, logoHeight, TFT_WHITE);
  delay(1500);
  // Create a 16 bit sprite
  img.setColorDepth(16);
  img.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
  // welcome text
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.fillScreen(TFT_BLACK);
  // configure centered String output
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.setFreeFont(FF2);
  M5.Lcd.drawString("IMU Snow Globe", (int)(M5.Lcd.width()/2), (int)(M5.Lcd.height()/2), 1);
  M5.Lcd.setFreeFont(FF1);
  M5.Lcd.drawString("Version 1.01 | 16.12.2020", (int)(M5.Lcd.width()/2), M5.Lcd.height()-20, 1);
  delay(1500);
}

void loop() {
  M5.update();
  // push the background image to the sprite
  img.pushImage(0, 0, 320, 240, (uint16_t *)background_image);
  // get the acceleration data
  // values are in g (9.81 m/s2)
  M5.IMU.getAccelData(&accX,&accY,&accZ);
  // Draw the snowflakes
  for (int i=0; i < flake_max; i++)
  {
    // detect shaking
    if(fabs(accX) > 2 || fabs(accY) > 2){
      flakeArray[i].x = random(SCREEN_WIDTH);
      flakeArray[i].y = random(SCREEN_HEIGHT);
    }
    else {
      // use gravity vector for movement
      float dx = (accX*-10.0) + (round(accX)*random(5)) + (round(accY)*(random(10)-5));
      float dy = (accY*10.0) +  (round(accX)*random(5)) + (round(accY)*(random(10)-5));
      flakeArray[i].x = flakeArray[i].x + round(dx*flakeArray[i].speed);
      flakeArray[i].y = flakeArray[i].y + round(dy*flakeArray[i].speed);
    }
    // keep the snowflakes on the screen
    if(flakeArray[i].x < 0)
      flakeArray[i].x = 0;
    if(flakeArray[i].x > SCREEN_WIDTH)
      flakeArray[i].x = SCREEN_WIDTH;
    if(flakeArray[i].y < 0)
      flakeArray[i].y = 0;
    if(flakeArray[i].y > SCREEN_HEIGHT)
      flakeArray[i].y = SCREEN_HEIGHT;
    // push the snowflake to the sprite on top of the background image
    img.drawXBitmap((int)(flakeArray[i].x-flakeWidth), 
                       (int)(flakeArray[i].y-flakeHeight), 
                       snowflake, flakeWidth, flakeHeight, TFT_WHITE);
  }
  // After all snowflakes are drawn, pus the sprite
  // to TFT screen CGRAM at coordinate x,y (top left corner)
  // Specify what colour is to be treated as transparent.
  img.pushSprite(0, 0, TFT_TRANSPARENT);
  delay(20);

}
