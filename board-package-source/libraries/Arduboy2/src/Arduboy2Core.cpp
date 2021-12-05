/**
 * @file Arduboy2Core.cpp
 * \brief
 * The Arduboy2Core class for Arduboy hardware initilization and control.
 */

#include "Arduboy2Core.h"

#include <avr/wdt.h>

#ifndef OLED_CONTRAST
# define OLED_CONTRAST 0xCF
#endif

//========================================
//========== class Arduboy2Core ==========
//========================================

// Commands sent to the OLED display to initialize it
const PROGMEM uint8_t Arduboy2Core::lcdBootProgram[] = {
  // boot defaults are commented out but left here in case they
  // might prove useful for reference
  //
  // Further reading: https://www.adafruit.com/datasheets/SSD1306.pdf
  
#if defined(GU12864_800B)
  0x24, 0x40,                   // enable Layer 0, graphic display area on
  0x47,                         // set brightness
  0x64, 0x00,                   // set x position 0
  0x84,                         // address mode set: X increment
#elif defined(OLED_SH1106) || (OLED_SH1106_I2C)
  0x8D, 0x14,                   // Charge Pump Setting v = enable (0x14)
  0xA1,                         // Set Segment Re-map
  0xC8,                         // Set COM Output Scan Direction
  0x81, OLED_CONTRAST,          // Set Contrast v = 0xCF
  0xD9, 0xF1,                   // Set Precharge = 0xF1
  OLED_SET_COLUMN_ADDRESS_LO,   //Set column address for left most pixel
  0xAF                          // Display On
#elif defined(LCD_ST7565)
  0xC8,                         //SET_COM_REVERSE
  0x28 | 0x7,                   //SET_POWER_CONTROL  | 0x7
  0x20 | 0x5,                   //SET_RESISTOR_RATIO | 0x5
  0x81,                         //SET_VOLUME_FIRST
  0x13,                         //SET_VOLUME_SECOND
  0xAF                          //DISPLAY_ON
#elif defined(OLED_96X96) || defined(OLED_128X96) || defined(OLED_128X128) || defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128) || defined(OLED_64X128_ON_128X128)
 #if defined(OLED_96X96) || defined(OLED_128X64_ON_96X96)
  0x15, 0x10, 0x3f, //left most 32 pixels are invisible
 #elif defined(OLED_96X96_ON_128X128)
  0x15, 0x08, 0x37, //center 96 pixels horizontally
 #elif defined(OLED_64X128_ON_128X128)
  0x15, 0x10, 0x2f, //center 64 pixels horizontally
 #else
  0x15, 0x00, 0x3f, //Set column start and end address
 #endif
 #if defined (OLED_96X96) 
  0x75, 0x20, 0x7f, //Set row start and end address
 #elif defined (OLED_128X64_ON_96X96) 
  0x75, 0x30, 0x6f, //Set row start and end address
 #elif defined (OLED_128X96)
  0x75, 0x00, 0x5f, //Set row start and end address
 #elif defined(OLED_128X64_ON_128X96)
  0x75, 0x10, 0x4f, //Set row start and end address
 #elif defined(OLED_96X96_ON_128X128) || defined(OLED_128X96_ON_128X128)
  0x75, 0x10, 0x6f, //Set row start and end address to centered 96 lines
 #elif defined(OLED_128X64_ON_128X128)
  0x75, 0x20, 0x5f, //Set row start and end address to centered 64 lines
 #else
  0x75, 0x00, 0x7F, //Set row start and end address to use all 128 lines
 #endif
 #if defined(OLED_64X128_ON_128X128)
  0xA0, 0x51,       //set re-map: split odd-even COM signals|COM remap|column address remap
 #else
  0xA0, 0x55,       //set re-map: split odd-even COM signals|COM remap|vertical address increment|column address remap
 #endif
  0xA1, 0x00,       //set display start line
  0xA2, 0x00,       //set display offset
  //0xA4,           //Normal display
  0xA8, 0x7F,       //Set MUX ratio 128MUX
  //0xB2, 0x23,
  //0xB3, 0xF0,     //set devider clock | oscillator frequency
  0x81, OLED_CONTRAST, //Set contrast
  //0xBC, 0x1F,     //set precharge voltage
  //0x82, 0xFE,     //set second Precharge speed
  0xB1, 0x21,       //reset and 1st precharge phase length  phase 2:2 DCLKs, Phase 1: 1 DCLKs
  //0xBB, 0x0F,     //set 2nd precharge period: 15 DCLKs
  //0xbe, 0x1F,     //output level high voltage com signal
  //0xB8, 0x04, 0x06, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, //set gray scale table
  0xAF              //Display on
#else
  // for SSD1306 and SSD1309 displays
  //
  // Display Off
  // 0xAE,

  // Set Display Clock Divisor v = 0xF0
  // default is 0x80
  0xD5, 0xF0,

  // Set Multiplex Ratio v = 0x3F
  // 0xA8, 0x3F,

  // Set Display Offset v = 0
  // 0xD3, 0x00,

  // Set Start Line (0)
  // 0x40,
 #if defined OLED_SSD1309
  //Charge Pump command not supported, use two NOPs instead to keep same size and easy patchability
  0xE3, 0xE3,
 #else  
  // Charge Pump Setting v = enable (0x14)
  // default is disabled
  0x8D, 0x14,
 #endif

  // Set Segment Re-map (A0) | (b0001)
  // default is (b0000)
  0xA1,

  // Set COM Output Scan Direction
  0xC8,

  // Set COM Pins v
  // 0xDA, 0x12,

  // Set Contrast v = 0xCF
  0x81, OLED_CONTRAST,

  // Set Precharge = 0xF1
  0xD9, 0xF1,

  // Set VCom Detect
  // 0xDB, 0x40,

  // Entire Display ON
  // 0xA4,

  // Set normal/inverse display
  // 0xA6,

  // Display On
  0xAF,

  // set display mode = horizontal addressing mode (0x00)
  0x20, 0x00,
 #if defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX)
  // set col address range
  0x21, 0x00, COLUMN_ADDRESS_END,

  // set page address range
  0x22, 0x00, PAGE_ADDRESS_END
 #endif
#endif
};

void Arduboy2Core::boot()
{
  #ifdef ARDUBOY_SET_CPU_8MHZ
  // ARDUBOY_SET_CPU_8MHZ will be set by the IDE using boards.txt
  setCPUSpeed8MHz();
  #endif

  // Select the ADC input here so a delay isn't required in generateRandomSeed()
  ADMUX = RAND_SEED_IN_ADMUX;

  bootPins();
  bootSPI();
  bootOLED();
  bootPowerSaving();
}

#ifdef ARDUBOY_SET_CPU_8MHZ
// If we're compiling for 8MHz we need to slow the CPU down because the
// hardware clock on the Arduboy is 16MHz.
// We also need to readjust the PLL prescaler because the Arduino USB code
// likely will have incorrectly set it for an 8MHz hardware clock.
void Arduboy2Core::setCPUSpeed8MHz()
{
  uint8_t oldSREG = SREG;
  cli();                // suspend interrupts
  PLLCSR = _BV(PINDIV); // dissable the PLL and set prescale for 16MHz)
  CLKPR = _BV(CLKPCE);  // allow reprogramming clock
  CLKPR = 1;            // set clock divisor to 2 (0b0001)
  PLLCSR = _BV(PLLE) | _BV(PINDIV); // enable the PLL (with 16MHz prescale)
  SREG = oldSREG;       // restore interrupts
}
#endif

// Pins are set to the proper modes and levels for the specific hardware.
// This routine must be modified if any pins are moved to a different port
void Arduboy2Core::bootPins()
{
#ifdef ARDUBOY_10

  // Port B INPUT_PULLUP or HIGH
  PORTB = (_BV(RED_LED_BIT) | _BV(BLUE_LED_BIT) | //RGB LED off
         #ifndef AB_ALTERNATE_WIRING
          _BV(GREEN_LED_BIT) | 
         #endif
         #ifndef ARDUINO_AVR_MICRO
          _BV(RX_LED_BIT) | //RX LED off for Arduboy and non Micro based Arduino
         #endif          
          _BV(B_BUTTON_BIT)) &
  // Port B INPUT or LOW                    
          ~(_BV(SPI_MISO_BIT) | _BV(SPI_MOSI_BIT) | _BV(SPI_SCK_BIT));

  // Port B outputs
  DDRB = (_BV(RED_LED_BIT)   | _BV(BLUE_LED_BIT) |
        #ifndef AB_ALTERNATE_WIRING
         _BV(GREEN_LED_BIT) | 
        #endif
         _BV(SPI_MOSI_BIT) | _BV(SPI_SCK_BIT)  | _BV(RX_LED_BIT)) &
  // Port B inputs
         ~(_BV(B_BUTTON_BIT) | _BV(SPI_MISO_BIT));

  // Port C
  // Speaker: Not set here. Controlled by audio class
  // Port D INPUT_PULLUP or HIGH
  PORTD = (
         #if defined(AB_ALTERNATE_WIRING)
          _BV(GREEN_LED_BIT) |
         #endif
         #if !(defined(ARDUINO_AVR_MICRO))
          _BV(TX_LED_BIT) | //TX LED off for Arduboy and non Micro based Arduino
         #endif          
          _BV(CART_BIT) | 
         #if !(defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C))
          _BV(DC_BIT) |
         #endif
          0) & ~( // Port D INPUTs or LOW outputs
         #if !(defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C))
          _BV(CS_BIT) |  // oled display enabled
          _BV(RST_BIT) | // reset active
         #endif
         #if defined(AB_ALTERNATE_WIRING)
          _BV(SPEAKER_2_BIT) |
         #endif
         #if defined(LCD_ST7565)
          _BV(POWER_LED_BIT) |
         #endif
         #if defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C)
          _BV(I2C_SCL) |
          _BV(I2C_SDA) |
         #endif
          0);

  // Port D outputs
  DDRD = (
        #if !(defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C))
         _BV(DC_BIT) | 
        #endif
        #if !(defined(AB_ALTERNATE_WIRING) && (CART_CS_SDA))
         _BV(RST_BIT) | 
         _BV(CS_BIT) |
        #endif
        #if defined(AB_ALTERNATE_WIRING)
         _BV(GREEN_LED_BIT) |
        #endif
        #if defined(LCD_ST7565)
         _BV(POWER_LED_BIT) |
        #endif
         _BV(CART_BIT) |
         _BV(TX_LED_BIT) |
         0) & ~(// Port D inputs
         #if defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C)
          _BV(I2C_SCL) | // SDA and SCL as inputs without pullups
          _BV(I2C_SDA) | // (both externally pulled up)
         #endif
         0);

  // Port E INPUT_PULLUP or HIGH
  PORTE |= _BV(A_BUTTON_BIT);
  // Port E INPUT or LOW (none)
  // Port E inputs
  DDRE &= ~(_BV(A_BUTTON_BIT));
  // Port E outputs (none)

  // Port F INPUT_PULLUP or HIGH
  PORTF = (_BV(LEFT_BUTTON_BIT) | _BV(RIGHT_BUTTON_BIT) |
          _BV(UP_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT)) &
  // Port F INPUT or LOW
          ~(_BV(RAND_SEED_IN_BIT));
  
  // Port F outputs (none)
  DDRF = 0 &
  // Port F inputs
         ~(_BV(LEFT_BUTTON_BIT) | _BV(RIGHT_BUTTON_BIT) |
         _BV(UP_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT) |
         _BV(RAND_SEED_IN_BIT));

#elif defined(AB_DEVKIT)

  // Port B INPUT_PULLUP or HIGH
  PORTB |= _BV(LEFT_BUTTON_BIT) | _BV(UP_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT) |
           _BV(BLUE_LED_BIT);
  // Port B INPUT or LOW (none)
  // Port B inputs
  DDRB &= ~(_BV(LEFT_BUTTON_BIT) | _BV(UP_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT) |
            _BV(SPI_MISO_BIT));
  // Port B outputs
  DDRB |= _BV(SPI_MOSI_BIT) | _BV(SPI_SCK_BIT) | _BV(BLUE_LED_BIT);

  // Port C INPUT_PULLUP or HIGH
  PORTC |= _BV(RIGHT_BUTTON_BIT);
  // Port C INPUT or LOW (none)
  // Port C inputs
  DDRC &= ~(_BV(RIGHT_BUTTON_BIT));
  // Port C outputs (none)

  // Port D INPUT_PULLUP or HIGH
  PORTD |= _BV(CS_BIT);
  // Port D INPUT or LOW
  PORTD &= ~(_BV(RST_BIT));
  // Port D inputs (none)
  // Port D outputs
  DDRD |= _BV(RST_BIT) | _BV(CS_BIT) | _BV(DC_BIT);

  // Port E (none)

  // Port F INPUT_PULLUP or HIGH
  PORTF |= _BV(A_BUTTON_BIT) | _BV(B_BUTTON_BIT);
  // Port F INPUT or LOW
  PORTF &= ~(_BV(RAND_SEED_IN_BIT));
  // Port F inputs
  DDRF &= ~(_BV(A_BUTTON_BIT) | _BV(B_BUTTON_BIT) | _BV(RAND_SEED_IN_BIT));
  // Port F outputs (none)
  // Speaker: Not set here. Controlled by audio class

#endif
}

void Arduboy2Core::bootOLED()
{
#if defined(GU12864_800B)
  bitSet(RST_PORT,RST_BIT);
  delayByte(10);
  displayEnable();
  for (uint8_t i = 0; i < sizeof(lcdBootProgram) + 8; i++)
  {
    if (i < 8)    
    {
      displayWrite(0x62); // set display area
      displayWrite(i);    // display area address
      LCDDataMode();     
      displayWrite(0xFF); // Graphic display
      LCDCommandMode();
    }
    else 
      displayWrite(pgm_read_byte(lcdBootProgram + i - 8));
  }
  displayDisable();
#elif defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C)
  i2c_start(SSD1306_I2C_CMD);
  for (uint8_t i = 0; i < sizeof(lcdBootProgram); i++)
    i2c_sendByte(pgm_read_byte(lcdBootProgram + i));
  i2c_stop();
#else
  // reset the display
  uint8_t cmd;
  const uint8_t* ptr = lcdBootProgram;
  delayByte(5);                          //for a short active low reset pulse
 #if !(defined(AB_ALTERNATE_WIRING) && defined(CART_CS_SDA))
  bitSet(RST_PORT, RST_BIT);             //deactivate reset
 #endif
  delayByte(5);
 #if defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128) || defined(OLED_64X128_ON_128X128)
  for (uint16_t i = 0; i < 8192; i++) SPItransfer(0); //make sure all display ram is cleared
 #endif
  //bitClear(CS_PORT, CS_BIT);               // select the display as default SPI device, already cleared by boot pins)
  LCDCommandMode();
 #if defined __AVR_ARCH__  
  asm volatile
  (
    "3:  lpm  %[cmd], Z+             \n" 
    : [ptr] "+z" (ptr),
      [cmd] "=r" (cmd)
    : 
    :
  );    
  SPItransfer(cmd);                      
  asm volatile(
    "    cpi  r30, lo8(%[lbp_end])   \n" // check only LSB cause size < 256
    "    brne 3b                     \n"
    : 
    : [lbp_end] "" (lcdBootProgram + sizeof(lcdBootProgram))
    :
  );
  LCDDataMode();
 #else
   for (uint8_t i = 0; i < sizeof(lcdBootProgram); i++) 
   {
     cmd = pgm_read_byte(lcdBootProgram + i));       
     SPItransfer(cmd);                      
   }
 #endif
#endif  
}

// Initialize the SPI interface for the display
void Arduboy2Core::bootSPI()
{
// master, mode 0, MSB first, CPU clock / 2 (8MHz)
  SPCR = _BV(SPE) | _BV(MSTR);
  SPSR = _BV(SPI2X);
}

// Write to the SPI bus (MOSI pin)
void Arduboy2Core::SPItransfer(uint8_t data)
{
  SPDR = data;
  /*
   * The following NOP introduces a small delay that can prevent the wait
   * loop from iterating when running at the maximum speed. This gives
   * about 10% more speed, even if it seems counter-intuitive. At lower
   * speeds it is unnoticed.
   */
  asm volatile("nop");
  while (!(SPSR & _BV(SPIF))) { } // wait
}

#if defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C)
void Arduboy2Core::i2c_start(uint8_t mode)
{
  I2C_SDA_LOW();       // disable posible internal pullup, ensure SDA low on enabling output
  I2C_SDA_AS_OUTPUT(); // SDA low before SCL for start condition
  I2C_SCL_LOW();
  I2C_SCL_AS_OUTPUT();  
  i2c_sendByte(SSD1306_I2C_ADDR << 1);
  i2c_sendByte(mode);
}

void Arduboy2Core::i2c_sendByte(uint8_t byte)
{
  uint8_t sda_clr = I2C_PORT & ~((1 << I2C_SDA) | (1 << I2C_SCL));
  uint8_t scl = 1 << I2C_SCL;
  uint8_t sda = 1 << I2C_SDA;
  uint8_t scl_bit = I2C_SCL;  
  asm volatile (    
    "    sec                    \n" // set carry for 8 shift counts
    "    rol  %[byte]           \n" // shift a bit out and count at the same time
    "1:                         \n"
    "    out  %[port], %[sda0]  \n" // preemtively clear SDA
    "    brcc 2f                \n" // skip if dealing with 0 bit
    "    out  %[pin], %[sda]    \n" 
    "2:                         \n" 
    "    out  %[pin], %[scl]    \n" // toggle SCL on
    "    lsl  %[byte]           \n" // next bit to carry (moved here for 1 extra cycle delay)
    "    out  %[pin], %[scl]    \n" // toggle SCL off
    "    brne 1b                \n" // initial set carry will be shifted out after 8 loops setting Z flag
    "                           \n" 
    "    out  %[port], %[sda0]  \n" // clear SDA for ACK
    "    nop                    \n" // extra delay
    "    sbi  %[port], %[sclb]  \n" // set SCL (extends ACK bit by 1 cycle)
    "    cbi  %[port], %[sclb]  \n" // clear SCL (extends SCL high by 1 cycle)
    :[byte] "+r" (byte)
    :[port] "i" (_SFR_IO_ADDR(I2C_PORT)),
     [pin]  "i" (_SFR_IO_ADDR(I2C_PIN)),
     [sda0] "r" (sda_clr),
     [scl]  "r" (scl),
     [sda]  "r" (sda),
     [sclb] "i" (scl_bit)
  );
}
#endif

void Arduboy2Core::safeMode()
{
  if (buttonsState() == UP_BUTTON)
  {
    setRGBledRedOn();

#ifndef ARDUBOY_CORE // for Arduboy core timer 0 should remain enabled
    // prevent the bootloader magic number from being overwritten by timer 0
    // when a timer variable overlaps the magic number location
    power_timer0_disable();
#endif

    while (true) { }
  }
}


/* Power Management */

void Arduboy2Core::idle()
{
  SMCR = _BV(SE); // select idle mode and enable sleeping
  sleep_cpu();
  SMCR = 0; // disable sleeping
}

void Arduboy2Core::bootPowerSaving()
{
  // disable Two Wire Interface (I2C) and the ADC
  // All other bits will be written with 0 so will be enabled
  PRR0 = _BV(PRTWI) | _BV(PRADC);
  // disable USART1
  PRR1 = _BV(PRUSART1);
}

#if defined(GU12864_800B)
void Arduboy2Core::displayEnable()
{
  bitSet(CS_PORT,CS_BIT);
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA);
  //bitClear(CS_PORT,CS_BIT);
  LCDCommandMode();
}

void Arduboy2Core::displayDisable()
{
  //bitSet(CS_PORT,CS_BIT);
  SPCR = _BV(SPE) | _BV(MSTR);
}

void Arduboy2Core::displayWrite(uint8_t data)
{
  bitClear(CS_PORT,CS_BIT);
  SPItransfer(data);
  bitSet(CS_PORT,CS_BIT);
}
#endif

// Shut down the display
void Arduboy2Core::displayOff()
{
#if defined(GU12864_800B)
  displayEnable();
  displayWrite(0x20);
  displayWrite(0x00);
  displayDisable();
#elif defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C)
  i2c_start(SSD1306_I2C_CMD);    
  i2c_sendByte(0xAE); // display off
  i2c_sendByte(0x8D); // charge pump:
  i2c_sendByte(0x10); //   disable
  i2c_stop();
#else
  LCDCommandMode();
  SPItransfer(0xAE); // display off
  SPItransfer(0x8D); // charge pump:
  SPItransfer(0x10); //   disable
#endif  
}

// Restart the display after a displayOff()
void Arduboy2Core::displayOn()
{
  bootOLED();
}


/* Drawing */

void Arduboy2Core::paint8Pixels(uint8_t pixels)
{
#if defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C)
  i2c_start(SSD1306_I2C_DATA);
  i2c_sendByte(pixels);
  i2c_stop();
#else  
  SPItransfer(pixels);
#endif
}

void Arduboy2Core::paintScreen(const uint8_t *image)
{
#if defined(GU12864_800B) 
  displayEnable();
  for (uint8_t r = 0; r < (HEIGHT/8); r++)
  {
    LCDCommandMode();
    displayWrite(0x60);
    displayWrite(r);
    LCDDataMode();
    for (uint8_t c = 0; c < (WIDTH); c++)
    {
      bitClear(CS_PORT,CS_BIT);
      SPDR = pgm_read_byte(image++);
      while (!(SPSR & _BV(SPIF)));
      bitSet(CS_PORT,CS_BIT);
    }
  }
  displayDisable();
#elif defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX)
  i2c_start(SSD1306_I2C_DATA);
  for (int i = 0; i < (HEIGHT * WIDTH) / 8; i++)
    i2c_sendByte(pgm_read_byte(image+i));
  i2c_stop();
#elif  defined (OLED_SH1106_I2C)
  for (int page = 0; page < HEIGHT/8; page++)
  {
    i2c_start(SSD1306_I2C_CMD);
    i2c_sendByte(OLED_SET_PAGE_ADDRESS + page); // set page
    i2c_sendByte(OLED_SET_COLUMN_ADDRESS_HI);   // only reset hi nibble to zero
    i2c_stop();
    const uint8_t *line = image + page*WIDTH;
    i2c_start(SSD1306_I2C_DATA);
    for (int i = 0; i < WIDTH; i++)
      i2c_sendByte(pgm_read_byte(line+i));
    i2c_stop();
  }
#elif defined(OLED_SH1106) || defined(LCD_ST7565)
  for (uint8_t i = 0; i < HEIGHT / 8; i++)
  {
    LCDCommandMode();
    SPDR = (OLED_SET_PAGE_ADDRESS + i);
    while (!(SPSR & _BV(SPIF)));
    SPDR = (OLED_SET_COLUMN_ADDRESS_HI); // only reset hi nibble to zero
    while (!(SPSR & _BV(SPIF)));
    LCDDataMode();
    for (uint8_t j = WIDTH; j > 0; j--)
      {
        SPDR = pgm_read_byte(image++);
        while (!(SPSR & _BV(SPIF)));
      }
  }
#elif defined(OLED_96X96) || defined(OLED_128X96) || defined(OLED_128X128) || defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128) || defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128)
 #if defined(OLED_128X64_ON_96X96)
  uint16_t i = 16;
  for (uint8_t col = 0; col < 96 / 2; col++)
 #else     
  uint16_t i = 0;
  for (uint8_t col = 0; col < WIDTH / 2; col++)
 #endif     
  {
    for (uint8_t row = 0; row < HEIGHT / 8; row++)
    {
      uint8_t b1 = pgm_read_byte(image + i);
      uint8_t b2 = pgm_read_byte(image + i + 1);
      for (uint8_t shift = 0; shift < 8; shift++)
      {
        uint8_t c = 0xFF;
        if ((b1 & 1) == 0) c &= 0x0F;
        if ((b2 & 1) == 0) c &= 0xF0;
        SPDR = c;
        b1 = b1 >> 1;
        b2 = b2 >> 1;
        while (!(SPSR & _BV(SPIF)));
      }
      i += WIDTH;
    }
    i -= HEIGHT / 8 * WIDTH - 2;
  }
#elif defined(OLED_64X128_ON_128X128)
  uint16_t i = WIDTH-1;
  for (uint8_t col = 0; col < WIDTH ; col++)
  {
    for (uint8_t row = 0; row < HEIGHT / 8; row++)
    {
      uint8_t b = pgm_read_byte(image + i);
      for (uint8_t shift = 0; shift < 4; shift++)
      {
        uint8_t c = 0xFF;
        if ((b & _BV(0)) == 0) c &= 0x0F;
        if ((b & _BV(1)) == 0) c &= 0xF0;
        SPDR = c;
        b = b >> 2;
        while (!(SPSR & _BV(SPIF)));
      }
      i += WIDTH;
    }
    i -= HEIGHT / 8 * WIDTH  + 1;
  }
#else 
  //OLED SSD1306 and compatibles
  for (int i = 0; i < (HEIGHT*WIDTH)/8; i++)
  {
    SPItransfer(pgm_read_byte(image + i));
  }
#endif
}

// paint from a memory buffer, this should be FAST as it's likely what
// will be used by any buffer based subclass
void Arduboy2Core::paintScreen(uint8_t image[], bool clear)
{
#if defined(GU12864_800B) 
  displayEnable();
  for (uint8_t r = 0; r < (HEIGHT/8); r++)
  {
    LCDCommandMode();
    displayWrite(0x60);
    displayWrite(r);
    LCDDataMode();
    for (uint8_t c = 0; c < (WIDTH); c++)
    {
      bitClear(CS_PORT,CS_BIT);
      if (clear)
      {
        SPDR = *image; // set the first SPI data byte to get things started
        *(image++) = 0;  // clear the first image byte
      }
      else
        SPDR = *(image++);
      while (!(SPSR & _BV(SPIF)));
      bitSet(CS_PORT,CS_BIT);
    }
  }
  displayDisable();
#elif defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX)
  uint16_t length = WIDTH * HEIGHT / 8;
  uint8_t sda_clr = I2C_PORT & ~((1 << I2C_SDA) | (1 << I2C_SCL));
  uint8_t scl = 1 << I2C_SCL;
  uint8_t sda = 1 << I2C_SDA;
  uint8_t scl_bit = I2C_SCL;
  i2c_start(SSD1306_I2C_DATA);
 #if defined (OLED_SSD1306_I2C)
  //bitbanging I2C ~2Mbps (8 cycles per bit / 78 cycles per byte)
  asm volatile (    
    "    dec  %[clear]          \n" //  get clear mask 0:0xFF, 1:0x00
    "    ld   r24, %a[ptr]      \n" // fetch display byte from buffer
    "1:                         \n"
    "    mov  r0, r24           \n" // move to shift register
    "    and  r24, %[clear]     \n" // apply clear mask
    "    st   %a[ptr]+, r24     \n" // update buffer
    "                           \n" 
    "    sec                    \n" // set carry for 8 shift counts
    "    rol  r0                \n" // shift a bit out and count at the same time
    "2:                         \n"
    "    out  %[port], %[sda0]  \n" // preemtively clear SDA
    "    brcc 3f                \n" // skip if dealing with 0 bit
    "    out  %[pin], %[sda]    \n" // toggle SDA on
    "3:                         \n" 
    "    out  %[pin], %[scl]    \n" // toggle SCL on
    "    lsl  r0                \n" // next bit to carry (moved here for 1 extra cycle delay)
    "    out  %[pin], %[scl]    \n" // toggle SCL off
    "    brne 2b                \n" // initial set carry will be shifted out after 8 loops setting Z flag
    "                           \n" 
    "    out  %[port], %[sda0]  \n" // clear SDA for ACK
    "    subi %A[len], 1        \n" // len-- part1 (moved here for 1 cycle delay)
    "    ld   r24, %a[ptr]      \n" // fetch display byte from buffer (and delay)
    "    out  %[pin], %[scl]    \n" // set SCL (2 cycles required)
    "    sbci %B[len], 0        \n" // len-- part2 (moved here for 1 cycle delay)
    "    out  %[pin], %[scl]    \n" // clear SCL (2 cycles required)
    "    brne 1b                \n"
    :[ptr]   "+e" (image),
     [len]   "+d" (length),
     [clear] "+r" (clear)
    :[port]  "i" (_SFR_IO_ADDR(I2C_PORT)),
     [pin]   "i" (_SFR_IO_ADDR(I2C_PIN)),
     [sda0]  "r" (sda_clr),
     [scl]   "r" (scl),
     [sda]   "r" (sda)
    :"r24"
  );
 #else
  //bitbanging I2C @ 2.66Mbps (6 cycles per bit / 56 cycles per byte)
  asm volatile (    
    "    dec  %[clear]          \n" //  get clear mask 0:0xFF, 1:0x00
    "    ld   r0, %a[ptr]       \n" // fetch display byte from buffer
    "1:                         \n"
    "    sbrc r0, 7             \n" // MSB first comes first
    "    out  %[pin], %[sda]    \n" // toggle SDA on for 1-bit
    "    out  %[pin], %[scl]    \n" // toggle SCL high
    "    mov  r24, r0           \n" // duplicate byte (also serves as extra clock cycle delay)
    "    out  %[pin], %[scl]    \n" // toggle SCL low
    "    out  %[port], %[sda0]  \n" // preemptively clear SDA for next bit
    "                           \n"    
    "    sbrc r0, 6             \n" // repeat of above but for bit 6
    "    out  %[pin], %[sda]    \n" //    
    "    out  %[pin], %[scl]    \n" //    
    "    and  r24, %[clear]     \n" // apply clear mask (also serves as extra clock cycle delay)
    "    out  %[pin], %[scl]    \n" //    
    "    out  %[port], %[sda0]  \n" //    
    
    "    sbrc r0, 5             \n" // 
    "    out  %[pin], %[sda]    \n" //    
    "    out  %[pin], %[scl]    \n" //    
    "    st   %a[ptr]+, r24     \n" // new buffer contents (also serves as extra clock cycle delay)
    "    out  %[pin], %[scl]    \n" //    
    "    out  %[port], %[sda0]  \n" //    

    "    sbrc r0, 4             \n" // 
    "    out  %[pin], %[sda]    \n" //    
    "    out  %[pin], %[scl]    \n" // 
    "    cbi  %[port], %[sclb]  \n" // using cbi for extra extra clock cycle delay
    "    out  %[port], %[sda0]  \n" // 

    "    sbrc r0, 3             \n" // 
    "    out  %[pin], %[sda]    \n" //    
    "    out  %[pin], %[scl]    \n" // 
    "    cbi  %[port], %[sclb]  \n" // using cbi for extra extra clock cycle delay
    "    out  %[port], %[sda0]  \n" // 
    
    "    sbrc r0, 2             \n" // 
    "    out  %[pin], %[sda]    \n" //    
    "    out  %[pin], %[scl]    \n" // 
    "    cbi  %[port], %[sclb]  \n" // using cbi for extra extra clock cycle delay
    "    out  %[port], %[sda0]  \n" // 
    
    "    sbrc r0, 1             \n" // 
    "    out  %[pin], %[sda]    \n" //    
    "    out  %[pin], %[scl]    \n" // 
    "    cbi  %[port], %[sclb]  \n" // using cbi for extra extra clock cycle delay
    "    out  %[port], %[sda0]  \n" // 
    
    "    sbrc r0, 0             \n" // 
    "    out  %[pin], %[sda]    \n" //    
    "    out  %[pin], %[scl]    \n" //    
    "    subi %A[len], 1        \n" // length-- part 1 (also serves as extra clock cycle delay)
    "    out  %[pin], %[scl]    \n" //    
    "    out  %[port], %[sda0]  \n" // SDA low for ACK   
    
    "    sbci %B[len], 0        \n" // length-- part 2 (also serves as extra clock cycle delay)
    "    out  %[pin], %[scl]    \n" // // clock ACK bit
    "    ld   r0, %a[ptr]       \n" // fetch next buffer byte (also serves as clock delay)
    "    out  %[pin], %[scl]    \n" // 
    "    brne 1b                \n" // length != 0 do next byte
    :[ptr]   "+e" (image),
     [len]   "+d" (length),
     [clear] "+r" (clear)
    :[port]  "i" (_SFR_IO_ADDR(I2C_PORT)),
     [pin]   "i" (_SFR_IO_ADDR(I2C_PIN)),
     [sda0]  "r" (sda_clr),
     [scl]   "r" (scl),
     [sda]   "r" (sda),
     [sclb]  "i" (scl_bit)
    :"r24"
  );
 #endif
  i2c_stop();
#elif  defined (OLED_SH1106_I2C)
  for (int page = 0; page < HEIGHT/8; page++)
  {
    i2c_start(SSD1306_I2C_CMD);
    i2c_sendByte(OLED_SET_PAGE_ADDRESS + page); // set page
    i2c_sendByte(OLED_SET_COLUMN_ADDRESS_HI);
    i2c_stop();
    i2c_start(SSD1306_I2C_DATA);
    if (clear)
    {
      for (int i = 0; i < WIDTH; i++)
      {
        i2c_sendByte(*image);
        *(image++) = 0;
      }
    } else
    {
      for (int i = 0; i < WIDTH; i++)
        i2c_sendByte(*(image++));
    }
    i2c_stop();
  }
#elif defined(OLED_SH1106) || defined(LCD_ST7565)
  //Assembly optimized page mode display code with clear support.
  //Each byte transfer takes 18 cycles
  asm volatile (
    "     ldi  r19, %[page_cmd]                     \n\t"
    "1:                                             \n\t"
    "     ldi  r18, %[col_cmd]        ;1            \n\t"
    "     ldi  r20, 6                 ;1            \n\t"
    "     cbi  %[dc_port], %[dc_bit]  ;2 cmd mode   \n\t"         
    "                                               \n\t"
    "     out  %[spdr], r19           ;1            \n\t"         
    "2:   dec  r20                    ;6*3-1 : 17   \n\t"         
    "     brne 2b                                   \n\t"         
    "     out  %[spdr], r18           ;1            \n\t"        
    
    "     ldi  r18, %[width]          ;1            \n\t"         
    "     inc  r18                    ;1            \n\t"              
    "     rjmp 5f                     ;2            \n\t"              
    "4:                                             \n\t"
    "     lpm  r20, Z                 ;3 delay      \n\t"
    "     ld   r20, Z                 ;2            \n\t"
    "     sbi  %[dc_port], %[dc_bit]  ;2 data mode  \n\t"
    "     out  %[spdr], r20           ;1            \n\t" 
    "     cpse %[clear], __zero_reg__ ;1/2          \n\t" 
    "     mov  r20, __zero_reg__      ;1            \n\t" 
    "     st   Z+, r20                ;2            \n\t"
    "5:                                             \n\t"
    "     lpm  r20, Z                 ;3 delay      \n\t"
    "     dec  r18                    ;1            \n\t"
    "     brne 4b                     ;1/2          \n\t"
    "     inc  r19                    ;1            \n\t"
    "     cpi  r19,%[page_end]        ;1            \n\t"
    "     brne 1b                     ;1/2          \n\t"
    "     in    __tmp_reg__, %[spsr]                \n\t" //read SPSR to clear SPIF
    : [ptr]      "+&z" (image)
    : 
      [page_cmd] "M" (OLED_SET_PAGE_ADDRESS),
      [page_end] "M" (OLED_SET_PAGE_ADDRESS + (HEIGHT / 8)),
      [dc_port]  "I" (_SFR_IO_ADDR(DC_PORT)),
      [dc_bit]   "I" (DC_BIT),
      [spdr]     "I" (_SFR_IO_ADDR(SPDR)),
      [spsr]    "I"   (_SFR_IO_ADDR(SPSR)),
      [col_cmd]  "M" (OLED_SET_COLUMN_ADDRESS_HI),
      [width]    "M" (WIDTH),
      [clear]    "r" (clear)
    : "r18", "r19", "r20"
  );
#elif defined(OLED_96X96) || defined(OLED_128X96) || defined(OLED_128X128)|| defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128)
  // 1 bit to 4-bit expander display code with clear support.
  // Each transfer takes 18 cycles with additional 4 cycles for a column change.
  asm volatile(
   #if defined(OLED_128X64_ON_96X96)
    "  adiw   r30, 16                           \n\t"          
   #endif
    "  ldi   r25, %[col]                        \n\t"          
    ".lcolumn:                                  \n\t"         
    "   ldi  r24, %[row]            ;1          \n\t"
    ".lrow:                                     \n\t"
    "   ldi  r21, 7                 ;1          \n\t"
    "   ld   r22, z                 ;2          \n\t"
    "   ldd  r23, z+1               ;2          \n\t"
    ".lshiftstart:                              \n\t"
    "   ldi  r20, 0xFF              ;1          \n\t"
    "   sbrs r22, 0                 ;1          \n\t"
    "   andi r20, 0x0f              ;1          \n\t"
    "   sbrs r23, 0                 ;1          \n\t"
    "   andi r20,0xf0               ;1          \n\t"
    "   out  %[spdr], r20           ;1          \n\t"
    "                                           \n\t"
    "   cp   %[clear], __zero_reg__ ;1          \n\t"
    "   brne .lclear1               ;1/2        \n\t"
    ".lshiftothers:                             \n\t"
    "   movw r18, %A[ptr]           ;1          \n\t"
    "   rjmp .+0                    ;2          \n\t"
    "   rjmp .lshiftnext            ;2          \n\t"
    ".lclear1:                                  \n\t"
    "   st   z, __zero_reg__        ;2          \n\t" 
    "   std  z+1, __zero_reg__      ;2          \n\t"
    ".lshiftnext:                               \n\t"
    "                                           \n\t"
    "   lsr  r22                    ;1          \n\t"
    "   lsr  r23                    ;1          \n\t"
    "                                           \n\t"
    "   ldi  r20, 0xFF              ;1          \n\t"
    "   sbrs r22, 0                 ;1/2        \n\t"
    "   andi r20, 0x0f              ;1          \n\t"
    "   sbrs r23, 0                 ;1/2        \n\t"
    "   andi r20,0xf0               ;1          \n\t"
    "                                           \n\t"
    "   subi r18, %[top_lsb]        ;1          \n\t" //image - (WIDTH * ((HEIGHT / 8) - 1) - 2)
    "   sbci r19, %[top_msb]        ;1          \n\t"
    "   subi r21, 1                 ;1          \n\t"
    "   out  %[spdr], r20           ;1          \n\t"
    "   brne .lshiftothers          ;1/2        \n\t"
    "                                           \n\t"
    "   nop                         ;1          \n\t"
    "   subi %A[ptr], %[width]      ;1          \n\t" //image + width (negated addition)
    "   sbci %B[ptr], -1            ;1          \n\t"
    "   subi r24, 1                 ;1          \n\t"
    "   brne .lrow                  ;1/2        \n\t"
    "                                           \n\t"
    "   movw %A[ptr], r18           ;1          \n\t"
    "   subi r25, 1                 ;1          \n\t"
    "   brne .lcolumn               ;1/2        \n\t"
    "   in    __tmp_reg__, %[spsr]              \n\t" //read SPSR to clear SPIF
    : [ptr]     "+&z" (image)
    : [spdr]    "I" (_SFR_IO_ADDR(SPDR)),
      [spsr]    "I"   (_SFR_IO_ADDR(SPSR)),
      [row]     "M" (HEIGHT / 8),
     #if defined(OLED_128X64_ON_96X96)
      [col]     "M" (96 / 2),
     #else
      [col]     "M" (WIDTH / 2),
     #endif
      [width]   "M" (256 - WIDTH),
      [top_lsb] "M" ((WIDTH * ((HEIGHT / 8) - 1) - 2) & 0xFF),
      [top_msb] "M" ((WIDTH * ((HEIGHT / 8) - 1) - 2) >> 8),
      [clear]   "r" (clear)
    : "r18", "r19", "r20", "r21", "r22", "r23", "r24", "r25"
  );
#elif defined(OLED_64X128_ON_128X128)
  uint16_t i = WIDTH-1;
  for (uint8_t col = 0; col < WIDTH ; col++)
  {
    for (uint8_t row = 0; row < HEIGHT / 8; row++)
    {
      uint8_t b = *(image + i);
      if (clear) *(image + i) = 0;
      for (uint8_t shift = 0; shift < 4; shift++)
      {
        uint8_t c = 0xFF;
        if ((b & _BV(0)) == 0) c &= 0x0F;
        if ((b & _BV(1)) == 0) c &= 0xF0;
        SPDR = c;
        b = b >> 2;
        while (!(SPSR & _BV(SPIF)));
      }
      i += WIDTH;
    }
    i -= HEIGHT / 8 * WIDTH  + 1;
  }
#else
  //OLED SSD1306 and compatibles
  //data only transfer with clear support at 18 cycles per transfer
  uint16_t count;

  asm volatile (
    "   ldi   %A[count], %[len_lsb]               \n\t" //for (len = WIDTH * HEIGHT / 8)
    "   ldi   %B[count], %[len_msb]               \n\t"
    "1: ld    __tmp_reg__, %a[ptr]      ;2        \n\t" //tmp = *(image)
    "   out   %[spdr], __tmp_reg__      ;1        \n\t" //SPDR = tmp
    "   cpse  %[clear], __zero_reg__    ;1/2      \n\t" //if (clear) tmp = 0;
    "   mov   __tmp_reg__, __zero_reg__ ;1        \n\t"
    "2: sbiw  %A[count], 1              ;2        \n\t" //len --
    "   sbrc  %A[count], 0              ;1/2      \n\t" //loop twice for cheap delay
    "   rjmp  2b                        ;2        \n\t"
    "   st    %a[ptr]+, __tmp_reg__     ;2        \n\t" //*(image++) = tmp
    "   brne  1b                        ;1/2 :18  \n\t" //len > 0
    "   in    __tmp_reg__, %[spsr]                \n\t" //read SPSR to clear SPIF
    : [ptr]     "+&e" (image),
      [count]   "=&w" (count)
    : [spdr]    "I"   (_SFR_IO_ADDR(SPDR)),
      [spsr]    "I"   (_SFR_IO_ADDR(SPSR)),
      [len_msb] "M"   (WIDTH * (HEIGHT / 8 * 2) >> 8),   // 8: pixels per byte
      [len_lsb] "M"   (WIDTH * (HEIGHT / 8 * 2) & 0xFF), // 2: for delay loop multiplier
      [clear]   "r"   (clear)
  );
  #endif  
}
#if 0
// For reference, this is the "closed loop" C++ version of paintScreen()
// used prior to the above version.
void Arduboy2Core::paintScreen(uint8_t image[], bool clear)
{
  uint8_t c;
  int i = 0;

  if (clear)
  {
    SPDR = image[i]; // set the first SPI data byte to get things started
    image[i++] = 0;  // clear the first image byte
  }
  else
    SPDR = image[i++];

  // the code to iterate the loop and get the next byte from the buffer is
  // executed while the previous byte is being sent out by the SPI controller
  while (i < (HEIGHT * WIDTH) / 8)
  {
    // get the next byte. It's put in a local variable so it can be sent as
    // as soon as possible after the sending of the previous byte has completed
    if (clear)
    {
      c = image[i];
      // clear the byte in the image buffer
      image[i++] = 0;
    }
    else
      c = image[i++];

    while (!(SPSR & _BV(SPIF))) { } // wait for the previous byte to be sent

    // put the next byte in the SPI data register. The SPI controller will
    // clock it out while the loop continues and gets the next byte ready
    SPDR = c;
  }
  while (!(SPSR & _BV(SPIF))) { } // wait for the last byte to be sent
}
#endif

void Arduboy2Core::blank()
{
#if defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX)
  i2c_start(SSD1306_I2C_DATA);
  for (int i = 0; i < (HEIGHT * WIDTH) / 8; i++)
    i2c_sendByte(0);
  i2c_stop();
#elif  defined (OLED_SH1106_I2C)
  for (int page = 0; page < HEIGHT/8; page++)
  {
    i2c_start(SSD1306_I2C_CMD);
    i2c_sendByte(OLED_SET_PAGE_ADDRESS + page); // set page
    i2c_sendByte(OLED_SET_COLUMN_ADDRESS_HI);   // only reset hi nibble to zero
    i2c_stop();
    i2c_start(SSD1306_I2C_DATA);
    for (int i = 0; i < WIDTH; i++)
      i2c_sendByte(0);
    i2c_stop();
  }
#else
 #if defined (OLED_SH1106)
  for (int i = 0; i < (HEIGHT * 132) / 8; i++)
 #elif defined(OLED_96X96) || defined(OLED_128X96) || defined(OLED_128X128)|| defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128) || defined(OLED_64X128_ON_128X128)
  for (int i = 0; i < (HEIGHT * WIDTH) / 2; i++)
 #else //OLED SSD1306 and compatibles
  for (int i = 0; i < (HEIGHT * WIDTH) / 8; i++)
 #endif
    SPItransfer(0x00);
#endif
}

void Arduboy2Core::sendLCDCommand(uint8_t command)
{
#if defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C)
  i2c_start(SSD1306_I2C_CMD);
  i2c_sendByte(command);
  i2c_stop();
#elif !defined GU12864_800B 
  LCDCommandMode();
  SPItransfer(command);
  LCDDataMode();
#endif
}

// invert the display or set to normal
// when inverted, a pixel set to 0 will be on
void Arduboy2Core::invert(bool inverse)
{
 #if defined(GU12864_800B)
  displayEnable();
  displayWrite(0x24);
  if (inverse) displayWrite(0x50);
  else displayWrite(0x40);
  LCDDataMode();
  displayDisable();
 #else
  sendLCDCommand(inverse ? OLED_PIXELS_INVERTED : OLED_PIXELS_NORMAL);
 #endif
}

// turn all display pixels on, ignoring buffer contents
// or set to normal buffer display
void Arduboy2Core::allPixelsOn(bool on)
{
 #if defined(GU12864_800B)
  displayEnable();
  if (on) 
  {
    displayWrite(0x20);
    displayWrite(0x50);
  }
  else 
    displayWrite(0x24);
    displayWrite(0x40);
  LCDDataMode();
  displayDisable();
 #else
  sendLCDCommand(on ? OLED_ALL_PIXELS_ON : OLED_PIXELS_FROM_RAM);
 #endif  
}

// flip the display vertically or set to normal
void Arduboy2Core::flipVertical(bool flipped)
{
 #ifdef GU12864_800B 
  //not available
 #else
  sendLCDCommand(flipped ? OLED_VERTICAL_FLIPPED : OLED_VERTICAL_NORMAL);
 #endif
}

// flip the display horizontally or set to normal
void Arduboy2Core::flipHorizontal(bool flipped)
{
 #ifdef GU12864_800B 
  //not available
 #else
  sendLCDCommand(flipped ? OLED_HORIZ_FLIPPED : OLED_HORIZ_NORMAL);
 #endif
}

/* RGB LED */

void Arduboy2Core::setRGBledRedOn()
{
 #ifndef LCD_ST7565
  bitClear(RED_LED_PORT, RED_LED_BIT); // Red on
 #else
  bitSet(RED_LED_PORT, RED_LED_BIT); // Red on        
 #endif
}

void Arduboy2Core::setRGBledRedOff()
{
 #ifndef LCD_ST7565
  bitSet(RED_LED_PORT, RED_LED_BIT); // Red off     
 #else
  bitClear(RED_LED_PORT, RED_LED_BIT); // Red off
 #endif
}

void Arduboy2Core::setRGBledGreenOn()
{
 #ifndef LCD_ST7565
  bitClear(GREEN_LED_PORT, GREEN_LED_BIT); // Green on
 #else
  bitSet(GREEN_LED_PORT, GREEN_LED_BIT); // Green on        
 #endif
}

void Arduboy2Core::setRGBledGreenOff()
{
 #ifndef LCD_ST7565
  bitSet(GREEN_LED_PORT, GREEN_LED_BIT); // Green off     
 #else
  bitClear(GREEN_LED_PORT, GREEN_LED_BIT); // Green off
 #endif
}

void Arduboy2Core::setRGBledBlueOn()
{
 #ifndef LCD_ST7565
  bitClear(BLUE_LED_PORT, BLUE_LED_BIT); // Blue on
 #else
  bitSet(BLUE_LED_PORT, BLUE_LED_BIT); // Blue on        
 #endif
}

void Arduboy2Core::setRGBledBlueOff()
{
 #ifndef LCD_ST7565
  bitSet(BLUE_LED_PORT, BLUE_LED_BIT); // Blue off     
 #else
  bitClear(BLUE_LED_PORT, BLUE_LED_BIT); // Blue off
 #endif
}

void Arduboy2Core::setRGBled(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef LCD_ST7565
  if ((red | green | blue) == 0) //prevent backlight off 
  {
    red   = 255;
    green = 255;
    blue  = 255;
  }
#endif
#ifdef ARDUBOY_10 // RGB, all the pretty colors
  // timer 0: Fast PWM, OC0A clear on compare / set at top
  // We must stay in Fast PWM mode because timer 0 is used for system timing.
  // We can't use "inverted" mode because it won't allow full shut off.
 #ifndef AB_ALTERNATE_WIRING
  TCCR0A = _BV(COM0A1) | _BV(WGM01) | _BV(WGM00);
  #ifndef LCD_ST7565
   OCR0A = 255 - green;
  #else
   OCR0A = green;
  #endif
 #else
  TCCR0A = _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
  #ifndef LCD_ST7565
   OCR0B = 255 - green;
  #else
   OCR0B = green;
  #endif
 #endif
  // timer 1: Phase correct PWM 8 bit
  // OC1A and OC1B set on up-counting / clear on down-counting (inverted). This
  // allows the value to be directly loaded into the OCR with common anode LED.
  TCCR1A = _BV(COM1A1) | _BV(COM1A0) | _BV(COM1B1) | _BV(COM1B0) | _BV(WGM10);
 #ifndef LCD_ST7565
  OCR1AL = blue;
  OCR1BL = red;
 #else
  OCR1AL = 255 - blue;
  OCR1BL = 255 - red;
 #endif
#elif defined(AB_DEVKIT)
  // only blue on DevKit, which is not PWM capable
  (void)red;    // parameter unused
  (void)green;  // parameter unused
  bitWrite(BLUE_LED_PORT, BLUE_LED_BIT, blue ? RGB_ON : RGB_OFF);
#endif
}

void Arduboy2Core::setRGBled(uint8_t color, uint8_t val)
{
#ifdef ARDUBOY_10
  if (color == RED_LED)
  {
   #ifdef LCD_ST7565
    OCR1BL = 255 - val;
   #else
    OCR1BL = val;
   #endif
  }
  else if (color == GREEN_LED)
  {
   #ifndef AB_ALTERNATE_WIRING
    OCR0A = 255 - val;
   #else
    #ifdef LCD_ST7565
	OCR0B = val;            
    #else
	OCR0B = 255 - val;            
    #endif
   #endif
  }
  else if (color == BLUE_LED)
  {
   #ifdef LCD_ST7565
    OCR1AL = 255 - val;
   #else
    OCR1AL = val;
   #endif
  }
#elif defined(AB_DEVKIT)
  // only blue on DevKit, which is not PWM capable
  if (color == BLUE_LED)
  {
    bitWrite(BLUE_LED_PORT, BLUE_LED_BIT, val ? RGB_ON : RGB_OFF);
  }
#endif
}

void Arduboy2Core::freeRGBled()
{
#ifdef ARDUBOY_10
  // clear the COM bits to return the pins to normal I/O mode
  TCCR0A = _BV(WGM01) | _BV(WGM00);
  TCCR1A = _BV(WGM10);
#endif
}

void Arduboy2Core::digitalWriteRGB(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef LCD_ST7565
  if ((red & green & blue) == RGB_OFF) //prevent backlight off 
  {
    red   = RGB_ON;
    green = RGB_ON;
    blue  = RGB_ON;
  }
  bitWrite(RED_LED_PORT, RED_LED_BIT, !red);
  bitWrite(GREEN_LED_PORT, GREEN_LED_BIT, !green);
  bitWrite(BLUE_LED_PORT, BLUE_LED_BIT, !blue);
#else
 #ifdef ARDUBOY_10
  bitWrite(RED_LED_PORT, RED_LED_BIT, red);
  bitWrite(GREEN_LED_PORT, GREEN_LED_BIT, green);
  bitWrite(BLUE_LED_PORT, BLUE_LED_BIT, blue);
 #elif defined(AB_DEVKIT)
  // only blue on DevKit
  (void)red;    // parameter unused
  (void)green;  // parameter unused
  bitWrite(BLUE_LED_PORT, BLUE_LED_BIT, blue);
 #endif
#endif
}

void Arduboy2Core::digitalWriteRGB(uint8_t color, uint8_t val)
{
#ifdef ARDUBOY_10
  if (color == RED_LED)
  {
    bitWrite(RED_LED_PORT, RED_LED_BIT, val);
  }
  else if (color == GREEN_LED)
  {
    bitWrite(GREEN_LED_PORT, GREEN_LED_BIT, val);
  }
  else if (color == BLUE_LED)
  {
    bitWrite(BLUE_LED_PORT, BLUE_LED_BIT, val);
  }
#elif defined(AB_DEVKIT)
  // only blue on DevKit
  if (color == BLUE_LED)
  {
    bitWrite(BLUE_LED_PORT, BLUE_LED_BIT, val);
  }
#endif
}

/* Buttons */

uint8_t Arduboy2Core::buttonsState()
{
  uint8_t buttons;

#ifdef ARDUBOY_10
  // up, right, left, down
  buttons = ((~PINF) &
              (_BV(UP_BUTTON_BIT) | _BV(RIGHT_BUTTON_BIT) |
               _BV(LEFT_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT) |
              #ifdef SUPPORT_XY_BUTTONS
               _BV(X_BUTTON_BIT) | _BV(Y_BUTTON_BIT) |
              #endif              
               0));
  // A
  if (bitRead(A_BUTTON_PORTIN, A_BUTTON_BIT) == 0) { buttons |= A_BUTTON; }
  // B
  if (bitRead(B_BUTTON_PORTIN, B_BUTTON_BIT) == 0) { buttons |= B_BUTTON; }
#elif defined(AB_DEVKIT)
  // down, left, up
  buttons = ((~PINB) &
              (_BV(DOWN_BUTTON_BIT) | _BV(LEFT_BUTTON_BIT) | _BV(UP_BUTTON_BIT)));
  // right
  if (bitRead(RIGHT_BUTTON_PORTIN, RIGHT_BUTTON_BIT) == 0) { buttons |= RIGHT_BUTTON; }
  // A
  if (bitRead(A_BUTTON_PORTIN, A_BUTTON_BIT) == 0) { buttons |= A_BUTTON; }
  // B
  if (bitRead(B_BUTTON_PORTIN, B_BUTTON_BIT) == 0) { buttons |= B_BUTTON; }
#endif

  return buttons;
}

unsigned long Arduboy2Core::generateRandomSeed()
{
  unsigned long seed;

  power_adc_enable(); // ADC on

  // do an ADC read from an unconnected input pin
  ADCSRA |= _BV(ADSC); // start conversion (ADMUX has been pre-set in boot())
  while (bit_is_set(ADCSRA, ADSC)) { } // wait for conversion complete

  seed = ((unsigned long)ADC << 16) + micros();

  power_adc_disable(); // ADC off

  return seed;
}

// delay in ms with 16 bit duration
void Arduboy2Core::delayShort(uint16_t ms)
{
 #ifndef ARDUBOY_CORE
  delay((unsigned long) ms);
 #else
  ::delayShort(ms);
 #endif
}

void Arduboy2Core::delayByte(uint8_t ms)
{
  delayShort(ms);
}

void Arduboy2Core::exitToBootloader()
{
  cli();
 #ifdef ARDUBOY_CORE
  asm volatile 
  (
    "jmp exit_to_bootloader \n" // resuse ISR exit to bootloader code
  );
 #else
  // set bootloader magic key
  // storing two uint8_t instead of one uint16_t saves an instruction
  //  when high and low bytes of the magic key are the same
  *(uint8_t *)MAGIC_KEY_POS = lowByte(MAGIC_KEY);
  *(uint8_t *)(MAGIC_KEY_POS + 1) = highByte(MAGIC_KEY);
  // enable watchdog timer reset, with 16ms timeout
  wdt_reset();
  WDTCSR = (_BV(WDCE) | _BV(WDE));
  WDTCSR = _BV(WDE);
  while (true) { }
 #endif
}

// Replacement main() that eliminates the USB stack code.
// Used by the ARDUBOY_NO_USB macro. This should not be called
// directly from a sketch.

//=========================================
//========== class Arduboy2NoUSB ==========
//=========================================

void Arduboy2NoUSB::mainNoUSB()
{
  // disable USB
  UDCON = _BV(DETACH);
  UDIEN = 0;
  UDINT = 0;
  USBCON = _BV(FRZCLK);
  UHWCON = 0;
  power_usb_disable();

  init();

  // This would normally be done in the USB code that uses the TX and RX LEDs
  //TX_RX_LED_INIT; // configured by bootpins

 #ifndef ARDUBOY_CORE // (Arduboy  core supports UP + DOWN to enter bootloader)
  // Set the DOWN button pin for INPUT_PULLUP
  bitSet(DOWN_BUTTON_PORT, DOWN_BUTTON_BIT);
  bitClear(DOWN_BUTTON_DDR, DOWN_BUTTON_BIT);

  // Delay to give time for the pin to be pulled high if it was floating
  Arduboy2Core::delayByte(10);

  // if the DOWN button is pressed
  if (bitRead(DOWN_BUTTON_PORTIN, DOWN_BUTTON_BIT) == 0) {
    Arduboy2Core::exitToBootloader();
  }
 #endif
  // The remainder is a copy of the Arduino main() function with the
  // USB code and other unneeded code commented out.
  // init() was called above.
  // The call to function initVariant() is commented out to fix compiler
  // error: "multiple definition of 'main'".
  // The return statement is removed since this function is type void.

//  init();

//  initVariant();

//#if defined(USBCON)
//  USBDevice.attach();
//#endif

  setup();

  for (;;) {
    loop();
//    if (serialEventRun) serialEventRun();
  }

//  return 0;
}

