/* *****************************************************************************
 * FX basic example v1.0 by Mr.Blinky                May 2021 licenced under CC0
 * *****************************************************************************
 * 
 * This is a basic example that shows how you can draw a image from FX external
 * flash memory. It will draw the Arduboy FX logo and move it around the screen.
 * 
 * This test depend on the file fxdata.bin being uploaded to the external FX flash 
 * chip using the uploader-gui.py or flash-writer.py Python script in the
 * development area. When using the flash writer script. Use the following command:
 * 
 * python flash-writer.py -d fxdata.bin
 * 
 ******************************************************************************/

#include <Arduboy2.h>     // required to build for Arduboy
#include <ArduboyFX.h>    // required to access the FX external flash
#include "fxdata.h"       // this file contains all references to FX data

//constant values
constexpr uint8_t FXlogoWidth = 115; 
constexpr uint8_t FXlogoHeight = 16;

Arduboy2 arduboy;

//assign values;
int16_t x    = (WIDTH - FXlogoWidth) / 2;
int16_t y    = 25;
int8_t  xDir = 1;
int8_t  yDir = 1;

void setup() {
  arduboy.begin();            // normal initialisation with Arduboy startup logo
  //arduboy.setFrameRate(60); // Only needed when frameRate != 60
  FX::disableOLED();          // OLED must be disabled before external flash is accessed. OLED display should only be enabled prior updating the display.
  FX::begin(FX_DATA_PAGE);    // external flash chip may be in power down mode so wake it up (Cathy bootloader puts chip into powerdown mode)
}

void loop() {
  if (!arduboy.nextFrame()) return; // Do nothing until it's time for the next frame

  //program code
  FX::drawBitmap(x, y, FXlogo, 0, dbmNormal);
  x += xDir;
  y += yDir;
  if (x == 0 || x == WIDTH - FXlogoWidth) xDir = -xDir;
  if (y == 0 || y == HEIGHT - FXlogoHeight) yDir = -yDir;
  
  FX::enableOLED();              // only enable OLED for updating the display
  arduboy.display(CLEAR_BUFFER); // Using CLEAR_BUFFER will clear the display buffer after it is displayed
  FX::disableOLED();             // disable display again so external flash can be accessed at any time
}
