/* *****************************************************************************
 * FX drawBitmap test v1.3 by Mr.Blinky Apr 2019-May 2021 licenced under CC0
 * *****************************************************************************
 * 
 * The map and whale images used in this example were made by 2bitcrook and are
 * licenced under CC-BY-NC-SA licence.
  
 * This test depend on the file fxdata.bin being uploaded to the external FX flash 
 * chip using the uploader-gui.py or flash-writer.py Python script in the
 * development area. When using the flash writer script. Use the following command:
 * 
 * python flash-writer.py -d fxdata.bin
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

#include <Arduboy2.h>     // required to build for Arduboy
#include <ArduboyFX.h>    // required to access the FX external flash
#include "fxdata.h"       // this file contains all references to FX data

#define FRAME_RATE 120

Arduboy2 arduboy;
bool showposition = true;
uint8_t select,color;
int x [2];
int y [2];

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(FRAME_RATE);
  FX::disableOLED();        // OLED must be disabled before external flash is accessed. OLED display should only be enabled prior updating the display.
  FX::begin(FX_DATA_PAGE);  //external flash chip may be in power down mode so wake it up (Cathy bootloader puts chip into powerdown mode)
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
  FX::enableOLED();              // only enable OLED for updating the display
  arduboy.display(CLEAR_BUFFER); // Using CLEAR_BUFFER will clear the display buffer after it is displayed
  FX::disableOLED();             // disable display again so external flash can be accessed at any time
}
