/*******************************************************************************
  Arduboy hardware test toy v1.1 Sep. 2017 - Nov.2021 by Mr.Blinky
*******************************************************************************/

#include <Arduboy2.h>
#include "globals.h"
#include "bitmaps.h"

Arduboy2 arduboy;
Sprites sprites;

////////////////////////////////////////////////////////////////////////////////

void setup() 
{
  arduboy.begin();
  arduboy.setRGBled(0,0,0);
  arduboy.setFrameRate(FRAMERATE);
  arduboy.audio.on; 
}

////////////////////////////////////////////////////////////////////////////////

void loop() 
{
  if (!(arduboy.nextFrame()))
    return;
     
  sprites.drawOverwrite(BACKGROUND_X,BACKGROUND_Y,background_gfx,0);
  #ifdef AB_DEVKIT
  sprites.drawOverwrite(NOSPEAKER_X,NOSPEAKER_Y,nospeaker_gfx,0);
  #endif
  
  //button input control
  if (arduboy.pressed(LEFT_BUTTON)) 
  {
    sprites.drawSelfMasked(BUTTON_LEFT_X,BUTTON_LEFT_Y,buttonleft_gfx,0);
    if (selectedOption > 4) selectedOption--;
    else if (selectedOption < 3) decreaseOptionValue();
    hardwareChange = true;
  }
  if (arduboy.pressed(RIGHT_BUTTON)) 
  {
    sprites.drawSelfMasked(BUTTON_RIGHT_X,BUTTON_RIGHT_Y,buttonright_gfx,0);
    if (selectedOption > 2)
    {
      if (selectedOption == 3) selectedOption += 2;
      else if (selectedOption < MAX_OPTIONS-1) selectedOption++;
    }
    else increaseOptionValue();
    hardwareChange = true;
  }
  if (arduboy.pressed(UP_BUTTON))
  {
    sprites.drawSelfMasked(BUTTON_UP_X,BUTTON_UP_Y,buttonup_gfx,0);
    if (selectedOption > 0) selectedOption--;
    hardwareChange = true;
  }
  if (arduboy.pressed(DOWN_BUTTON)) 
  {
    sprites.drawSelfMasked(BUTTON_DOWN_X,BUTTON_DOWN_Y,buttondown_gfx,0);
    if (selectedOption < MAX_OPTIONS-1) selectedOption++;
    hardwareChange = true;
  }
  if (arduboy.pressed(A_BUTTON)) 
  {
    sprites.drawSelfMasked(BUTTON_A_X,BUTTON_A_Y,button_gfx,0);
    decreaseOptionValue();
    hardwareChange = true;
  }
  if (arduboy.pressed(B_BUTTON)) 
  {
    sprites.drawSelfMasked(BUTTON_B_X,BUTTON_B_Y,button_gfx,0);
    increaseOptionValue();
    hardwareChange = true;
  }
  
  // draw focused option
  switch (selectedOption)
  {
    case 0: sprites.drawOverwrite(RED_X,RED_Y,red_focus_gfx,0);
            break;
    case 1: sprites.drawOverwrite(GREEN_X,GREEN_Y,green_focus_gfx,0);
            break;
    case 2: sprites.drawOverwrite(BLUE_X,BLUE_Y,blue_focus_gfx,0);
            break;
    case 3: sprites.drawOverwrite(RXLED_X,RXLED_Y,rxled_on_focus_gfx,0);
            break;
    case 4: sprites.drawOverwrite(TXLED_X,TXLED_Y,txled_on_focus_gfx,0);
            break;
    case 5: sprites.drawOverwrite(SPEAKER1_X,SPEAKER1_Y,speaker1_on_focus_gfx,0);
            break;
    case 6: sprites.drawOverwrite(SPEAKER2_X,SPEAKER2_Y,speaker2_on_focus_gfx,0);
            break;
  }
  
  //Draw option values
  sprites.drawOverwrite(SLIDER_X + SLIDER_X_STEP * rgbLed.red,SLIDER_Y,slider_gfx,0);
  sprites.drawOverwrite(SLIDER_X + SLIDER_X_STEP * rgbLed.green,SLIDER_Y + 1 * SLIDER_HEIGHT,slider_gfx,0);
  sprites.drawOverwrite(SLIDER_X + SLIDER_X_STEP * rgbLed.blue,SLIDER_Y + 2 * SLIDER_HEIGHT,slider_gfx,0);
  sprites.drawOverwrite(ON_GFX_X,ON_GFX_Y,rxled_on ? on_gfx : off_gfx,0);
  sprites.drawOverwrite(OFF_GFX_X,OFF_GFX_Y,txled_on ? on_gfx : off_gfx,0);
  if (speaker1_on) sprites.drawOverwrite(SOUND1_X,SOUND1_Y,sound_gfx,0);
  if (speaker2_on) sprites.drawOverwrite(SOUND2_X,SOUND2_Y,sound_gfx,0);
  //update complete display
  arduboy.display();
  
  //apply hardware changes
  if (hardwareChange)
  {
    arduboy.setRGBled(pgm_read_byte(rgbValues + rgbLed.red), pgm_read_byte(rgbValues + rgbLed.green), pgm_read_byte(rgbValues + rgbLed.blue));
    rxled_on ? RXLED1 : RXLED0;
    txled_on ? TXLED1 : TXLED0;
    SPEAKER_1_PORT &= ~_BV(SPEAKER_1_BIT);    
    SPEAKER_2_PORT |= _BV(SPEAKER_2_BIT);
    for (int i = 0; i < 50; i++) 
    {
      speaker1_on ? SPEAKER_1_PORT ^= _BV(SPEAKER_1_BIT) : SPEAKER_1_PORT &= ~_BV(SPEAKER_1_BIT);
      #ifndef AB_DEVKIT
      speaker2_on ? SPEAKER_2_PORT ^= _BV(SPEAKER_2_BIT) : SPEAKER_2_PORT |= _BV(SPEAKER_2_BIT);
      #endif
      delayMicroseconds(300);
    }
    SPEAKER_1_PORT &= ~_BV(SPEAKER_1_BIT);    
    SPEAKER_2_PORT &= ~_BV(SPEAKER_2_BIT);    
    hardwareChange = false;
  }
}

////////////////////////////////////////////////////////////////////////////////

void decreaseOptionValue(void) 
{
  switch (selectedOption)
  {
    case 0: if (rgbLed.red > 0) rgbLed.red --;
            break;
    case 1: if (rgbLed.green > 0) rgbLed.green --;
            break;
    case 2: if (rgbLed.blue > 0) rgbLed.blue --;
            break;
    case 3: rxled_on = !rxled_on;
            break;
    case 4: txled_on = !txled_on;
            break;
    case 5: speaker1_on = !speaker1_on;
            break;
    case 6: speaker2_on = !speaker2_on;
            break;
  }  
}

void increaseOptionValue(void)
{
  switch (selectedOption)
  {
    case 0: if (rgbLed.red < RGB_MAX_STEPS-1) rgbLed.red ++;
            break;
    case 1: if (rgbLed.green < RGB_MAX_STEPS-1) rgbLed.green ++;
            break;
    case 2: if (rgbLed.blue < RGB_MAX_STEPS-1) rgbLed.blue ++;
            break;
    case 3: rxled_on = !rxled_on;
            break;
    case 4: txled_on = !txled_on;
            break;
    case 5: speaker1_on = !speaker1_on;
            break;
    case 6: speaker2_on = !speaker2_on;
            break;
  }  
}

////////////////////////////////////////////////////////////////////////////////