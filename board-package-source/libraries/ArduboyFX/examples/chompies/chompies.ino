/* *****************************************************************************
 * FX drawBitmap test v1.31 by Mr.Blinky Apr 2019-Feb 2022 licenced under CC0
 * *****************************************************************************
 * 
 * The map and whale images used in this example were made by 2bitcrook and are
 * licenced under CC-BY-NC-SA licence.
  
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
 * This example uses a 816 by 368 pixel image as background and a 
 * 107 x 69 image for masked sprite. Both can be moved around using the button combos
 * below. This example also shows how you can make use of the different draw modes.
 * 
 * 
 * A            Invert the while sprite from black to white and vice versa
 * B            Show or hide the coordinates in the top left corner
 * D-PAD        move around the whale sprite
 * D-PAD + A    move around the whale sprite in single pixel steps
 * D-PAD + B    move around the background image
 * 
 ******************************************************************************/

#include <Arduboy2.h>       // required to build for Arduboy
#include <ArduboyFX.h>      // required to access the FX external flash
#include "fxdata/fxdata.h"  // this file contains all references to FX data

#define FRAME_RATE 120

Arduboy2 arduboy;
bool showposition = true;
uint8_t select,color;
int x [2];
int y [2];

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(FRAME_RATE);
  FX::begin(FX_DATA_PAGE);  // initialise FX chip
}

void loop() {
  if (!arduboy.nextFrame()) return;

  arduboy.pollButtons();
  if (arduboy.justPressed(B_BUTTON)) showposition = !showposition;
  if (arduboy.pressed(B_BUTTON)) select = 0;
  else select = 1;
  if (arduboy.justPressed(A_BUTTON)) color ^= dbmReverse;
  if (arduboy.pressed(A_BUTTON))
  {
  if (arduboy.justPressed(UP_BUTTON)) y[select]--;
  if (arduboy.justPressed(DOWN_BUTTON)) y[select]++;
  if (arduboy.justPressed(LEFT_BUTTON)) x[select]--;
  if (arduboy.justPressed(RIGHT_BUTTON)) x[select]++;
  }
  else
  {
  if (arduboy.pressed(UP_BUTTON)) y[select]--;
  if (arduboy.pressed(DOWN_BUTTON)) y[select]++;
  if (arduboy.pressed(LEFT_BUTTON)) x[select]--;
  if (arduboy.pressed(RIGHT_BUTTON)) x[select]++;
  }
  
  FX::drawBitmap(x[0],y[0],mapGfx,0,dbmNormal);
  FX::drawBitmap(x[1],y[1],whaleGfx,0,dbmMasked | color);        // comment this line and uncomment one below to test the drawing modes
  //FX::drawBitmap(x[1],y[1],whaleGfx,0,dbmMasked | dbmBlack);   // black pixels as drawn solid. White pixels are transparent. Mask is ignored.
  //FX::drawBitmap(x[1],y[1],whaleGfx,0,dbmMasked | dbmWhite);   // white pixels are drawn solid. Black pixels are transparent. Mask is ignored.
  //FX::drawBitmap(x[1],y[1],whaleGfx,0,dbmMasked | dbmInvert);  // white pixels are xored together with background pixels. Mask is ignored.
  //FX::drawBitmap(x[1],y[1],whaleGfx,0,dbmMasked | dbmReverse); // Inverts the image: white pixels are drawn as black, black pixels are drawn as white pixels. Mask is applied.
  if (showposition)
  {
    arduboy.setCursor(0,0);
    arduboy.print(x[select]);
    arduboy.setCursor(0,8);
    arduboy.print(y[select]);
  }
  FX::display(CLEAR_BUFFER); // Using CLEAR_BUFFER will clear the display buffer after it is displayed
}
