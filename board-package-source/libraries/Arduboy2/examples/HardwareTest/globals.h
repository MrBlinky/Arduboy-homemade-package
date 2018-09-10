#ifndef GLOBALS_H
#define GLOBALS_H

#define FRAMERATE  10
#define BACKGROUND_X 0
#define BACKGROUND_Y 0

#define SLIDER_X 20
#define SLIDER_Y -1
#define SLIDER_HEIGHT 14
#define SLIDER_X_STEP 6
#define RGB_MAX_STEPS 17


#define RED_X 0
#define RED_Y 0

#define GREEN_X 0
#define GREEN_Y 14

#define BLUE_X 0
#define BLUE_Y 28

#define RXLED_X 0
#define RXLED_Y 44

#define TXLED_X 0
#define TXLED_Y 54

#define ON_GFX_X 16
#define ON_GFX_Y 44

#define OFF_GFX_X 16
#define OFF_GFX_Y 54

#define SPEAKER1_X 34
#define SPEAKER1_Y 44

#define SOUND1_X 48
#define SOUND1_Y 46

#define SPEAKER2_X 56
#define SPEAKER2_Y 44

#define SOUND2_X 70
#define SOUND2_Y 46

#define NOSPEAKER_X 57
#define NOSPEAKER_Y 43

#define BUTTON_LEFT_X 80
#define BUTTON_LEFT_Y 49

#define BUTTON_RIGHT_X 91
#define BUTTON_RIGHT_Y 49

#define BUTTON_UP_X 86
#define BUTTON_UP_Y 43

#define BUTTON_DOWN_X 86
#define BUTTON_DOWN_Y 54

#define BUTTON_A_X 108
#define BUTTON_A_Y 55

#define BUTTON_B_X 118
#define BUTTON_B_Y 46

#ifdef AB_DEVKIT
  #define MAX_OPTIONS 6 //No 2nd speaker pin
#else  
  #define MAX_OPTIONS 7
#endif

boolean rxled_on;
boolean txled_on;
boolean speaker1_on;
boolean speaker2_on;
byte selectedOption;
boolean hardwareChange;

typedef struct rgbled_t
{
  byte red;  
  byte green;  
  byte blue;  
} rgbled_t;

rgbled_t rgbLed;

const unsigned char PROGMEM rgbValues[RGB_MAX_STEPS] = {
  0,1,4,9,16,25,36,49,64,81,100,111,144,169,196,225,255
  };

#endif
