/* *****************************************************************************
 * FX Hello world example v1.00 by Mr.Blinky         Mar.2022 licenced under CC0
 * *****************************************************************************
 * 
 * This is a example that shows how you can draw texts from the FX flash chip. 
 * It will draw some text, number, and a scrolling message using different 
 * fonts and drawing modes.
 * 
 * Before this example sketch is uploaded and run on the Arduboy FX, make sure 
 * the fxdata of this sketch has been build and uploaded to the Arduboy FX. 
 * 
 * If the Arduboy FX Arduino plugin has been installed you can simply choose the 
 * 'Build and upload Arduboy FX data' from the Arduino IDE Tools menu.
 * 
 * Alternatively the fxdata.txt script file can be build using the fxdata-build.py 
 * Phyton script and the fxdata.bin file can be uploaded using the uploader-gui.py, 
 * fxdata-upload.py or flash-writer.py Python script using the -d switch.
 * 
 ******************************************************************************/

#include <Arduboy2.h>
#include <ArduboyFX.h>
#include "fxdata/fxdata.h"

Arduboy2Base arduboy;

void setup()
{
  arduboy.begin();
  FX::begin(FX_DATA_PAGE);             // Initialize FX flash chip
  FX::setFont(arduboyFont, dcmNormal); // select default font
  FX::setCursorRange(0,32767);         // set cursor left and wrap positions
}

uint16_t frames = 0;
uint8_t speed = 1;      // scrolly speed
int16_t scroll_x = 128; // scrolly position

uint8_t fontMode = dcmNormal;
int8_t  leadingDigits = 5; // number of digits printed including leading zeros
uint8_t str[] = "FX Demo"; // demo text stored in ram.

void loop()
{
  if(!(arduboy.nextFrame())) return;
  arduboy.pollButtons();

  // draw FX demo string in top left corner
  FX::setCursor(0,0);         // set cursor to top left positon
  FX::setFontMode(dcmNormal); // only change the font mode to normal
  FX::drawString(str);        // draw string from a buffer (in this case the buffer holds 'FX Demo' text)

  // draw number of displayed frames in top right corner
  FX::setCursor(WIDTH - 5 * arduboyFontWidth, 0);    // position the cursor
  FX::drawNumber(frames, leadingDigits); // draw the frames number
  
  // draw a scrolly message using a masked font
  FX::setCursor(scroll_x, 24);                   // position the cursor
  FX::setFont(maskedFont, dcmMasked | fontMode); // Select the masked font
  FX::drawString(helloWorld);                    // 'print' the message
 
  //draw 'press any button' text as normal or inverse text
  FX::setCursor(13, HEIGHT - arduboyFontHeight);
  FX::setFont(arduboyFont, fontMode);
  FX::drawString(" Press any button "); // draw an immediate string (string is stored in RAM so it is not recommended to use this method)

  // copy display buffer to OLED display and clear the buffer
  FX::display(CLEAR_BUFFER); 
  
  // decrement x for scrolling effect
  scroll_x -= speed; 
  if (scroll_x < -1792) scroll_x = 128; 
  frames++;
  
  // handle button changes
  if (arduboy.justPressed(0xFF)) frames = 0;       // clear frames counter if any new button is pressed
  if (arduboy.justPressed(UP_BUTTON)) speed = 2;   // faster scrolling speed
  if (arduboy.justPressed(DOWN_BUTTON)) speed = 1; // slower scrolling speed
  if (arduboy.justPressed(LEFT_BUTTON)) leadingDigits = (leadingDigits == -5) ? 0 : -5; // use leading spaces
  if (arduboy.justPressed(RIGHT_BUTTON))leadingDigits = (leadingDigits ==  5) ? 0 :  5; // use leading zeros
  if (arduboy.justPressed(A_BUTTON)) fontMode =  dcmNormal; // Normal drawing mode
  if (arduboy.justPressed(B_BUTTON)) fontMode = dcmReverse; // Reverse drawing mode
}
