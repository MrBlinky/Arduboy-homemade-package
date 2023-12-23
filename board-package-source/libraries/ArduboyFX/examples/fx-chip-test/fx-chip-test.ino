/* *****************************************************************************
 * FX chip test example v1.0 by Mr.Blinky          Sep 2023 licenced under CC0
 * *****************************************************************************
 * 
 * This test program can be used for testing and troubleshooting the external 
 * flash memory (FX) chip.
 * 
 * It will show which chip select configuration is selected for compiling and
 * it reads out the FX chips JEDEC ID and status register information. It also
 * checks if the FX chip is loaded with a flash image file.
 * 
 * Pressing the A-button will reset the status registers and disable any write
 * protected areas of the FX chip (unless the status register itself is protected)
 * 
 ******************************************************************************/

#include <Arduboy2.h>       // required to build for Arduboy
#include <ArduboyFX.h>      // required to access the FX external flash

Arduboy2 arduboy;

void setup() {
  arduboy.begin();            // normal initialisation with Arduboy startup logo
  FX::begin(0);               // initialise FX chip
}

JedecID  jedecID;

void printHexByte(uint8_t b)
{
 if (b <16) arduboy.print(0);
 arduboy.print(b,HEX);
}

void printStatusReg(uint8_t cmd)

{
  FX::enable();
  FX::writeByte(cmd);
  printHexByte(FX::readByte());
  FX::disable();
}

void loop() {
  if (!arduboy.nextFrame()) return; // Do nothing until it's time for the next frame
  arduboy.pollButtons();            // required for arduboy.justPressed

  // Show which chip select this sketch is compiled for.
 #if defined(CART_CS_RX)  
  arduboy.setCursor(7,0);
  arduboy.print(F("Chip select: PD2/RX"));
 #elif defined(CART_CS_HWB)
  arduboy.setCursor(4,0);
  arduboy.print(F("Chip select: PE2/HWB"));
 #else
  arduboy.setCursor(4,0);
  arduboy.print(F("Chip select: PD1/SDA"));
 #endif

  // Show JEDEC ID
  FX::enable();
  FX::writeByte(SFC_JEDEC_ID);
  jedecID.manufacturer = FX::readByte();
  jedecID.device = FX::readByte();
  jedecID.size = FX::readByte();
  FX::disable();
  arduboy.setCursor(16,12);
  arduboy.print(F("JEDEC ID: "));
  printHexByte(jedecID.manufacturer);
  printHexByte(jedecID.device );
  printHexByte(jedecID.size);  

  // Show flash status register
  arduboy.setCursor(22,24);
  arduboy.print(F("Status: "));
  if (jedecID.size == 0x18) printStatusReg(SFC_READSTATUS3);
  printStatusReg(SFC_READSTATUS2);
  printStatusReg(SFC_READSTATUS1);

  // Show state of the the flash chips image
  uint8_t cartSignature[7];
  arduboy.setCursor(0,36);
  arduboy.print(F("State: "));
  FX::readDataBytes(0, cartSignature,sizeof(cartSignature));
  if (strncmp(cartSignature,"ARDUBOY",sizeof(cartSignature)))
  {
     arduboy.print(F("No image loaded")); 
  }
  else
  {
     arduboy.print(F("Image loaded")); 
  }
  
  arduboy.setCursor(13,48);
  arduboy.print(F("Press A-button to"));
  arduboy.setCursor(1,56);
  arduboy.print(F("reset Status register"));
  if (arduboy.justPressed(A_BUTTON))
  {
    FX::writeEnable();
    FX::enable();
    FX::writeByte(0x01);  
    FX::writeByte(0);
    if (jedecID.size != 0x18) FX::writeByte(0);
    FX::disable();

    if (jedecID.size == 0x18)
    {
      FX::writeEnable();
      FX::enable();
      FX::writeByte(0x31);  
      FX::writeByte(0);
      FX::disable();
      
      FX::writeEnable();
      FX::enable();
      FX::writeByte(0x11);  
      FX::writeByte(0);
      FX::disable();
    }
  }
  
  FX::display(CLEAR_BUFFER); // Using CLEAR_BUFFER will clear the display buffer after it is displayed
}
