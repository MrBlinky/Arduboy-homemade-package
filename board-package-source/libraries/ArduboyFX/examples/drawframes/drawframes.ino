/* *****************************************************************************
 * FX drawDrame example v1.00 by Mr.Blinky    Jan 2023 licenced under CC0
 * *****************************************************************************
 * 
 * This is a example that shows how you can draw animations from the FX external
 * flash memory using the drawframes function
 * 
* Before this example sketch is uploaded and run on the Arduboy FX, make sure 
 * the fxdata used by this sketch has been build and uploaded to the Arduboy FX. 
 * 
 * If the Arduboy FX Arduino plugin has been installed you can simply choose the 
 * 'Build and upload Arduboy FX data' from the Arduino IDE Tools menu.
 * 
 * Alternatively the fxdata.txt script file can be build using the fxdata-build.py 
 * Python script. using the command:
 *
 * fxdata-upload.py fxdata.txt

 The created fxdata.bin file can be uploaded to your Arduboy by using the 
 *uploader-gui.py Python script using the command:
 
 * fxdata-upload.py fxdata.bin
 *
 * On some OS you may need to type python3 infront of the script name and supply
 * the full path to the script and fxdata files.
 * 
 ******************************************************************************
 *
 * There are 3 functions to make drawing animations of multiple bitmap images
 * easier:
 *
 * - uint24_t FX::drawFrame(uint24_t address)
 * -          FX::setFrame(uint24_t frame, uint8_t repeat);
 * - uint8_t  FX::drawFrame();
 *
 * FX::drawFrame(uint24_t address) 
 * -------------------------------
 * drawFrame is a bit like FX::drawBitmap but instead of passing the parameters
 * along with the function directly, they are stored in FX memory at the given
 * address. In addition to that parameters of multiple bitmaps can be stored at
 * the address to draw multiple bitmaps by this function.
 
 * the list of bitmap parameters is called a frame. The parameters are the same
 * as used with FX::drawBitmap except the mode parameter. An extra flag value 
 * can be added to mark the end of a frame or to mark the frame as the last in
 * the list of frames.
 *
 * this functions return the address of the next frame unless the frame is
 * flagged as a last frame. In that case the function returns 0
 * 
 * using this function has no effect on using the other frame functions.
 *
 * FX::setFrame(uint24_t address, uint8_t repeat)
 * --------------------------------------------
 * setFrame sets the current frame address and repeat value for the drawFrame 
 * function below.
 *
 * - address is the address of a frame list in FX memory.
 * - repeat is number of times each frame should be repeated.
 *   Each frame is drawn repeat + 1 times.
 *
 * uint8_t  FX::drawFrame()
 * ------------------------
 *
 * drawFrame without parameters draws the current frame in the frame list set
 * by the setFrame function above. When the last frame of the framelist is
 * drawn and is also the last repeated frame, the current frame pointer is
 * reset to start of the frame list. This effectively loops the frame list.
 * the function also returns 0 in that case. This can be usedto detect the 
 * end of the animation.
 *
 ******************************************************************************/

#include <Arduboy2.h>       // required to build for Arduboy
#include <ArduboyFX.h>      // required to access the FX external flash
#include "fxdata/fxdata.h"  // this file contains all references to FX data

Arduboy2 arduboy;

enum class State : uint8_t {
    init,
    logo,
    wait,
    main,
};

State state;
uint8_t timer;

void setup() {
  arduboy.boot();                // boot is used instead of begin as we don't want the Arduboy startup logo
  arduboy.display(CLEAR_BUFFER); // another way to clear the display
  arduboy.systemButtons();       // also do system buttons as it's normally used by begin
  //arduboy.setFrameRate(60);    // Only needed for frameRates other than 60
  FX::begin(FX_DATA_PAGE);       // initialise FX chip
  state = State::init;
}

void loop() {
  if (!arduboy.nextFrame()) return; // Do nothing until it's time for the next display frame
  arduboy.pollButtons();            // required here for arduboy.justPressed() below

  switch (state) {
      
    //initialise animation using setFrame  
    case State::init:
      {
        FX::setFrame(ArduboyLogo_Frame, 0); // set frame list for drawFrame() and 0 repeats
        state = State::logo;                // changing the 0 into an higher value will
      }                                     // slow down the animation
      break;
      
    // do logo animation  
    case State::logo:
      {                          // draw a frame from the frame list and test if it was the last frame
        if(!FX::drawFrame()) {   // without this test the animation would loop continiously
            state = State::wait; // it was the last frame go for wait state
            timer = 60;          // wait for 60 frames
          }
      }
      break;

    //wait and display a single frame
    case State::wait:
      {
        FX::drawFrame(ArduboyLogo_LastFrame); 
        if (--timer == 0) state = State::main;
      }
      break;

    case State::main:
      {
        // No special code here so the screen will turn black after the wait period
        
        // pressing A or B button will restart the animation
        if (arduboy.justPressed(A_BUTTON | B_BUTTON)) state = State::init; 
      }
      break;
  }

  FX::display(CLEAR_BUFFER); // Using CLEAR_BUFFER will clear the display buffer after it is displayed
}
