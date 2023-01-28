/* *****************************************************************************
 * FX basic example v1.02 by Mr.Blinky    May 2021 - Feb 2022 licenced under CC0
 * *****************************************************************************
 * 
 * This is a basic example that shows how you can draw a image from FX external
 * flash memory. It will draw the Arduboy FX logo and move it around the screen.
 * 
* Before this example sketch is uploaded and run on the Arduboy FX, make sure 
 * the fxdata this sketch has been build and uploaded to the Arduboy FX. 
 * 
 * If the Arduboy FX Arduino plugin has been installed you can simply choose the 
 * 'Build and upload Arduboy FX data' from the Arduino IDE Tools menu.
 * 
 * Alternatively the fxdata.txt script file can be build using the fxdata-build.py 
 * Phyton script and the fxdata.bin file can be uploaded using the uploader-gui.py, 
 * fxdata-upload.py or flash-writer.py Python script using the -d switch.
 * 
 ******************************************************************************/

#include <Arduboy2.h>       // required to build for Arduboy
#include <ArduboyFX.h>      // required to access the FX external flash
#include "fxdata/fxdata.h"  // this file contains all references to FX data

Arduboy2 arduboy;

//assign values;
int16_t x    = (WIDTH - FXlogoWidth) / 2;
int16_t y    = 25;
int8_t  xDir = 1;
int8_t  yDir = 1;

void setup() {
  arduboy.begin();            // normal initialisation with Arduboy startup logo
  //arduboy.setFrameRate(60); // Only needed when frameRate != 60
  FX::begin(FX_DATA_PAGE);    // initialise FX chip
}

void loop() {
  if (!arduboy.nextFrame()) return; // Do nothing until it's time for the next frame

  //program code
  FX::drawBitmap(x, y, FXlogo, 0, dbmNormal);
  x += xDir;
  y += yDir;
  if (x == 0 || x == WIDTH - FXlogoWidth) xDir = -xDir;
  if (y == 0 || y == HEIGHT - FXlogoHeight) yDir = -yDir;
  
  FX::display(CLEAR_BUFFER); // Using CLEAR_BUFFER will clear the display buffer after it is displayed
}
