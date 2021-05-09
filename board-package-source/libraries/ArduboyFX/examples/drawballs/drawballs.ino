/* *****************************************************************************
 * FX draw balls test v1.15 by Mr.Blinky May 2019 May2021 licenced under CC0
 * *****************************************************************************
 * 
 * This test depend on the file fxdata.bin being uploaded to the external FX flash 
 * chip using the uploader-gui.py or flash-writer.py Python script in the
 * development area. When using the flash writer script. Use the following command:
 * 
 * python flash-writer.py -d fxdata.bin
 * 
 * This demo draws a moving background tilemap with a bunch of bouncing ball sprites around
 * 
 * Button controls:
 * 
 * A - increase the number of bounching balls up to MAX_BALLS
 * B - decrease the number of balls down to zero
 * 
 * D-Pad - scroll the background
 * 
 */

#include <Arduboy2.h>
#include <ArduboyFX.h>                // Required library for accessing the FX flash chip
#include "fxdata.h"                   // this file contains all the references to FX data
                                      // Check out fxdata.txt to see how this is done.
#define FRAME_RATE 60

#define MAX_BALLS 55                   // 55 Balls possible at 60fps 155 at 30fps
#define CIRCLE_POINTS 84               
#define VISABLE_TILES_PER_COLUMN 5     // the maximum number of tiles visable vertically
#define VISABLE_TILES_PER_ROW 9        // the maximum number of tiles visable horizontally

//datafile offsets
constexpr uint8_t ballWidth = 16;
constexpr uint8_t ballHeight = 16;
constexpr uint8_t tilemapWidth = 16;   // number of tiles in a tilemap row
constexpr uint8_t tileWidth  = 16;
constexpr uint8_t tileHeight = 16;

Arduboy2 arduboy;

Point circlePoints[CIRCLE_POINTS] = // all the points of a circle with radius 15 used for the circling background effect
{
  {-15,0},  {-15,1},   {-15,2},   {-15,3},  {-15,4},  {-14,5},  {-14,6},  {-13,7},  {-13,8},  {-12,9},   {-11,10},  {-10,11}, {-9,12},  {-8,13},  {-7,13},  {-6,14},
  {-5,14},  {-4,14},   {-3,15},   {-2,15},  {-1,15},  {0,15},   {1,15},   {2,15},   {3,15},   {4,14},    {5,14},    {6,14},   {7,13},   {8,13},   {9,12},   {10,11},
  {11,10},  {12,9},    {12,8},    {13,7},   {13,6},   {14,5},   {14,4},   {14,3},   {14,2},   {14,1},    {15,0},    {15,-1},  {15,-2},  {15,-3},  {15,-4},  {14,-5},
  {14,-6},  {13,-7},   {13,-8},   {12,-9},  {11,-10}, {10,-11}, {9,-12},  {8,-13},  {7,-13},  {6,-14},   {5,-14},   {4,-14},  {3,-15},  {2,-15},  {1,-15},  {0,-15},
  {-1,-15}, {-2,-15},  {-3,-15},  {-4,-14}, {-5,-14}, {-6,-14}, {-7,-13}, {-8,-13}, {-9,-12}, {-10,-11}, {-11,-10}, {-12,-9}, {-12,-8}, {-13,-7}, {-13,-6}, {-14,-5},
  {-14,-4}, {-14,-3},  {-14,-2},  {-14,-1}
};

Point camera;
Point mapLocation = {16,16};

struct Ball 
{
  int8_t x;
  int8_t y;
  int8_t xspeed;
  int8_t yspeed;  
};

Ball ball[MAX_BALLS];
uint8_t ballsVisible = MAX_BALLS;

uint8_t pos;

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(FRAME_RATE);
  FX::disableOLED(); // OLED must be disabled before cart can be used. OLED display should only be enabled prior updating the display.
  FX::begin(FX_DATA_PAGE); // wakeup external flash chip, initialize datapage, detect presence of external flash chip
  
  for (uint8_t i=0; i < MAX_BALLS; i++) // initialize ball sprites
  {
   ball[i].x = random(113);
   ball[i].y = random(49);
   ball[i].xspeed = 1;//random(1,3);
   if (random(100) > 49) ball[i].xspeed = -ball[i].xspeed;
   ball[i].yspeed = 1; //random(1,3);
   if (random(100) > 49) ball[i].yspeed = -ball[i].yspeed;
  }                                     
}

uint8_t tilemapBuffer[VISABLE_TILES_PER_ROW]; // a small buffer to store one horizontal row of tiles from the tilemap

void loop() {
  if (!arduboy.nextFrame()) return; // return until it's time to draw a new frame

  arduboy.pollButtons();                                                           // pollButtons required for the justPressed() function
  if ((arduboy.justPressed(A_BUTTON) && ballsVisible < MAX_BALLS)) ballsVisible++; // Pressing A button increases the number of visible balls until the maximum has been reached
  if ((arduboy.justPressed(B_BUTTON) && ballsVisible > 0)) ballsVisible--;         // Pressing B reduces the number of visible balls until none are visible
  if (arduboy.pressed(UP_BUTTON) && mapLocation.y > 16) mapLocation.y--;           // Pressing directional buttons will scroll the tilemap
  if (arduboy.pressed(DOWN_BUTTON) && mapLocation.y < 176) mapLocation.y++; 
  if (arduboy.pressed(LEFT_BUTTON) && mapLocation.x > 16) mapLocation.x--;
  if (arduboy.pressed(RIGHT_BUTTON) && mapLocation.x < 112) mapLocation.x++; 
  
  camera.x = mapLocation.x + circlePoints[pos].x; // circle around a fixed point
  camera.y = mapLocation.y + circlePoints[pos].y;
  
  //draw tilemap
  for (int8_t y = 0; y < VISABLE_TILES_PER_COLUMN; y++)
  {
    FX::readDataArray(FX_DATA_TILEMAP,           // read the visible tiles on a horizontal row from the tilemap in external flash
                      y + camera.y / tileHeight, // the tilemap row
                      camera.x / tileWidth,      // the column within tilemap row
                      tilemapWidth,              // use the width of tilemap as array element size
                      tilemapBuffer,             // reading tiles into a small buffer is faster then reading each tile individually
                      VISABLE_TILES_PER_ROW);

    for (uint8_t x = 0; x < VISABLE_TILES_PER_ROW; x++)
    {
      FX::drawBitmap(x * tileWidth - camera.x % tileWidth,   // we're substracting the tile width and height modulus for scrolling effect
                     y * tileHeight - camera.y % tileHeight, //
                     FX_DATA_TILES,                          // the tilesheet bitmap offset in external flash
                     tilemapBuffer[x],                       // tile index
                     dbmNormal);                             // draw a row of normal tiles
    }
  }
  if (arduboy.notPressed(UP_BUTTON | DOWN_BUTTON | LEFT_BUTTON | RIGHT_BUTTON)) pos = ++pos % CIRCLE_POINTS; //only circle around when no directional buttons are pressed
  
  //draw balls
  for (uint8_t i=0; i < ballsVisible; i++)
    FX::drawBitmap(ball[i].x,                      // although this function is called drawBitmap it can also draw masked sprites
                   ball[i].y, 
                   FX_DATA_BALLS,                  // the ball sprites masked bitmap offset in external flash memory
                   0,                              // the fxdata was build using the single ball sprite.png image so there's only frame 0
                   //i % 16,                       // comment above and uncomment this one if the fxdata is rebuild using the ball_16x16.png image
                   dbmMasked /* | dbmReverse */ ); // remove the '/*' and '/*' to reverse the balls into white balls

   //when uploading the drawballs-singe-datafile.bin into the development area,
   //you can replace the "0" value in the drawBitmap function above with "i % 16" without the quotes to display 16 different balls
                     
  //update ball movements
  for (uint8_t i=0; i < ballsVisible; i++)
  {
    if (ball[i].xspeed > 0) // Moving right
    {
      ball[i].x += ball[i].xspeed;
      if (ball[i].x > WIDTH - ballWidth) //off the right
      {
        ball[i].x = WIDTH - ballWidth;
        ball[i].xspeed = - ball[i].xspeed;
      }
    }
    else // moving left
    {
      ball[i].x += ball[i].xspeed;
      if (ball[i].x < 0) // off the left
      {
        ball[i].x = 0;
        ball[i].xspeed = - ball[i].xspeed;
      }
    }
    if (ball[i].yspeed > 0) // moving down
    {
      ball[i].y += ball[i].yspeed;
      if (ball[i].y > HEIGHT - tileHeight) // off the bottom
      {
        ball[i].y = HEIGHT - tileHeight;
        ball[i].yspeed = - ball[i].yspeed;
      }
    }
    else // moving up
    {
      ball[i].y += ball[i].yspeed;
      if (ball[i].y < 0) // off the top
      {
        ball[i].y = 0;
        ball[i].yspeed = - ball[i].yspeed;
      }
    }
  }
      
  FX::enableOLED();// only enable OLED for updating the display
  arduboy.display(CLEAR_BUFFER); // Using CLEAR_BUFFER will clear the display buffer after it is displayed
  FX::disableOLED();// disable display again so external flash can be accessed at any time
}
