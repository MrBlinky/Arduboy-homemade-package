#include "core.h"

// need to redeclare these here since we declare them static in .h
volatile uint8_t *ArduboyCore::mosiport, 
   *ArduboyCore::csport,  *ArduboyCore::dcport;
uint8_t ArduboyCore::mosipinmask, 
  ArduboyCore::cspinmask, ArduboyCore::dcpinmask;

const uint8_t PROGMEM pinBootProgram[] = {
  // buttons
  PIN_LEFT_BUTTON, INPUT_PULLUP,
  PIN_RIGHT_BUTTON, INPUT_PULLUP,
  PIN_UP_BUTTON, INPUT_PULLUP,
  PIN_DOWN_BUTTON, INPUT_PULLUP,
  PIN_A_BUTTON, INPUT_PULLUP,
  PIN_B_BUTTON, INPUT_PULLUP,
  
#if (defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C))
  //I2C
  SDA, INPUT,
  SCL, INPUT,
#else    
  // OLED SPI
  DC, OUTPUT,
  CS, OUTPUT,
  RST, OUTPUT,
#endif  
  0
};

const uint8_t PROGMEM lcdBootProgram[] = {
    
  // boot defaults are commented out but left here incase they
  // might prove useful for reference
  //
  // Further reading: https://www.adafruit.com/datasheets/SSD1306.pdf

#if defined(GU12864_800B)
  0x24, 0x40,                   // enable Layer 0, graphic display area on
  0x47,                         // set brightness
  0x64, 0x00,                   // set x position 0
  0x84,                         // address mode set: X increment
#elif defined(OLED_SH1106) || defined(OLED_SH1106_I2C)
  0x8D, 0x14,                   // Charge Pump Setting v = enable (0x14)
  0xA1,                         // Set Segment Re-map
  0xC8,                         // Set COM Output Scan Direction
  0x81, 0xCF,                   // Set Contrast v = 0xCF
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
  0x81, 0xCF,       //Set contrast
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
  0x81, 0xCF,

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

ArduboyCore::ArduboyCore() {}

void ArduboyCore::boot()
{
  #if F_CPU == 8000000L
  slowCPU();
  #endif

  SPI.begin();
  bootPins();
  bootLCD();

  #ifdef SAFE_MODE
  if (buttonsState() == (LEFT_BUTTON | UP_BUTTON))
    safeMode();
  #endif

  saveMuchPower();
}

#if F_CPU == 8000000L
// if we're compiling for 8Mhz we need to slow the CPU down because the
// hardware clock on the Arduboy is 16MHz
void ArduboyCore::slowCPU()
{
  uint8_t oldSREG = SREG;
  cli();                // suspend interrupts
  CLKPR = _BV(CLKPCE);  // allow reprogramming clock
  CLKPR = 1;            // set clock divisor to 2 (0b0001)
  SREG = oldSREG;       // restore interrupts
}
#endif

void ArduboyCore::bootPins()
{
  uint8_t pin, mode;
  const uint8_t *i = pinBootProgram;

  while(true) {
    pin = pgm_read_byte(i++);
    mode = pgm_read_byte(i++);
    if (pin==0) break;
    pinMode(pin, mode);
  }
#if defined (OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX)
  I2C_SCL_LOW();
  I2C_SDA_LOW();
#else
  digitalWrite(RST, HIGH);
  delay(1);           // VDD (3.3V) goes high at start, lets just chill for a ms
  digitalWrite(RST, LOW);   // bring reset low
  delay(10);          // wait 10ms
  digitalWrite(RST, HIGH);  // bring out of reset
#endif
}

#if defined(GU12864_800B)
void ArduboyCore::displayEnable()
{
  *csport |= cspinmask;
  SPCR = _BV(SPE) | _BV(MSTR) | _BV(CPOL) | _BV(CPHA);
  LCDCommandMode();
}

void ArduboyCore::displayDisable()
{
  SPCR = _BV(SPE) | _BV(MSTR);
}

void ArduboyCore::displayWrite(uint8_t data)
{
  *csport &= ~cspinmask;
  SPI.transfer(data);
  *csport |= cspinmask;
}
#endif

void ArduboyCore::bootLCD()
{
#if defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C)
  i2c_start(SSD1306_I2C_CMD);
  for (uint8_t i = 0; i < sizeof(lcdBootProgram); i++)
    i2c_sendByte(pgm_read_byte(lcdBootProgram + i));
  i2c_stop();
  #if defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX)
  i2c_start(SSD1306_I2C_DATA);
  for (uint16_t i = 0; i < WIDTH * HEIGHT / 8; i++)
    i2c_sendByte(0);  
  i2c_stop();
  #else
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
  #endif
#else
  // setup the ports we need to talk to the OLED
  csport = portOutputRegister(digitalPinToPort(CS));
  cspinmask = digitalPinToBitMask(CS);
  *portOutputRegister(digitalPinToPort(CS)) &= ~cspinmask;
  dcport = portOutputRegister(digitalPinToPort(DC));
  dcpinmask = digitalPinToBitMask(DC);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
 #if defined(GU12864_800B)
  delay(1);
  digitalWrite(RST, HIGH);
  delay(10);
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
 #elif defined(OLED_128X64_ON_96X96) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128) || defined(OLED_64X128_ON_128X128)
  LCDDataMode();
  for (uint16_t i = 0; i < 8192; i++) SPI.transfer(0); //Clear all display ram
 #endif

  LCDCommandMode();
  // run our customized boot-up command sequence against the
  // OLED to initialize it properly for Arduboy
  for (uint8_t i = 0; i < sizeof(lcdBootProgram); i++) {
    SPI.transfer(pgm_read_byte(lcdBootProgram + i));
  }
  LCDDataMode();
#endif
}

void ArduboyCore::LCDDataMode()
{
 #if defined(GU12864_800B)
  *dcport &= ~dcpinmask;
 #else
  *dcport |= dcpinmask;
 #endif
  // *csport &= ~cspinmask; 
}

void ArduboyCore::LCDCommandMode()
{
  // *csport |= cspinmask;
 #if defined(GU12864_800B)
  *dcport |= dcpinmask;
 #else
  *dcport &= ~dcpinmask;
 #endif
  // *csport &= ~cspinmask; CS set once at bootLCD
}

#if defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C)
void ArduboyCore::i2c_start(uint8_t mode)
{
  I2C_SDA_LOW();       // disable posible internal pullup, ensure SDA low on enabling output
  I2C_SDA_AS_OUTPUT(); // SDA low before SCL for start condition
  I2C_SCL_LOW();
  I2C_SCL_AS_OUTPUT();  
  i2c_sendByte(SSD1306_I2C_ADDR << 1);
  i2c_sendByte(mode);
}

void ArduboyCore::i2c_sendByte(uint8_t byte)
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
    "    out  %[port],%[sda0]   \n" // clear SDA for ACK
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

void ArduboyCore::safeMode()
{
  blank(); // too avoid random gibberish
  while (true) {
    asm volatile("nop \n");
  }
}


/* Power Management */

void ArduboyCore::idle()
{
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_mode();
}

void ArduboyCore::saveMuchPower()
{
  power_adc_disable();
  power_usart0_disable();
  power_twi_disable();
  // timer 0 is for millis()
  // timers 1 and 3 are for music and sounds
  power_timer2_disable();
  power_usart1_disable();
  // we need USB, for now (to allow triggered reboots to reprogram)
  // power_usb_disable()
}

uint8_t ArduboyCore::width() { return WIDTH; }

uint8_t ArduboyCore::height() { return HEIGHT; }


/* Drawing */

void ArduboyCore::paint8Pixels(uint8_t pixels)
{
#if defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C)
  i2c_start(SSD1306_I2C_DATA);
  i2c_sendByte(pixels);
  i2c_stop();
#else
  SPI.transfer(pixels);
#endif
}

void ArduboyCore::paintScreen(const unsigned char *image)
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
      *csport &= ~cspinmask;
      SPDR = pgm_read_byte(image++);
      while (!(SPSR & _BV(SPIF)));
      *csport |= cspinmask;
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
  	SPDR = (OLED_SET_COLUMN_ADDRESS_HI); // we only need to reset hi nibble to 0
	while (!(SPSR & _BV(SPIF)));
  	LCDDataMode();
  	for (uint8_t j = WIDTH; j > 0; j--)
      {
  		SPDR = pgm_read_byte(*(image++));
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
  for (int i = 0; i < (HEIGHT*WIDTH)/8; i++)
  {
    SPI.transfer(pgm_read_byte(image + i));
  }
#endif
}

// paint from a memory buffer, this should be FAST as it's likely what
// will be used by any buffer based subclass
void ArduboyCore::paintScreen(unsigned char image[])
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
      *csport &= ~cspinmask;
      SPDR = *(image++);
      while (!(SPSR & _BV(SPIF)));
      *csport |= cspinmask;
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
    "    ld   r0, %a[ptr]+      \n" // fetch display byte from buffer
    "1:                         \n"
    "    sec                    \n" // set carry for 8 shift counts
    "    rol  r0                \n" // shift a bit out and count at the same time
    "2:                         \n"
    "    out  %[port], %[sda0]  \n" // preemtively clear SDA
    "    brcc 3f                \n" // skip if dealing with 0 bit
    "    out  %[pin], %[sda]    \n" 
    "3:                         \n" 
    "    out  %[pin], %[scl]    \n" // toggle SCL on
    "    lsl  r0                \n" // next bit to carry (moved here for 1 extra cycle delay)
    "    out  %[pin], %[scl]    \n" // toggle SCL off
    "    brne 2b                \n" // initial set carry will be shifted out after 8 loops setting Z flag
    "                           \n" 
    "    out  %[port], %[sda0]  \n" // clear SDA for ACK
    "    subi %A[len], 1        \n" // len-- part1 (moved here for 1 cycle delay)
    "    ld   r0, %a[ptr]+      \n" // fetch display byte from buffer (and delay)
    "    out  %[pin], %[scl]    \n" // set SCL (2 cycles required)
    "    sbci %B[len], 0        \n" // len-- part2 (moved here for 1 cycle delay)
    "    out  %[pin], %[scl]    \n" // clear SCL (2 cycles required)
    "    brne 1b                \n"
    :[ptr]   "+e" (image),
     [len]   "+d" (length)
    :[port]  "i" (_SFR_IO_ADDR(I2C_PORT)),
     [pin]   "i" (_SFR_IO_ADDR(I2C_PIN)),
     [sda0]  "r" (sda_clr),
     [scl]   "r" (scl),
     [sda]   "r" (sda)
  );
 #else
  //bitbanging I2C @ 2.66Mbps (6 cycles per bit / 56 cycles per byte)
  asm volatile (    
    "    ld   r0, %a[ptr]+      \n" // fetch display byte from buffer
    "1:                         \n"
    "    sbrc r0, 7             \n" // MSB first comes first
    "    out  %[pin], %[sda]    \n" // toggle SDA on for 1-bit
    "    out  %[pin], %[scl]    \n" // toggle SCL high
    "    cbi  %[port], %[sclb]  \n" // set SCL low
    "    out  %[port], %[sda0]  \n" // preemptively clear SDA for next bit
    "                           \n"    
    "    sbrc r0, 6             \n" // repeat of above but for bit 6
    "    out  %[pin], %[sda]    \n" //    
    "    out  %[pin], %[scl]    \n" //    
    "    cbi  %[port], %[sclb]  \n" // using cbi for extra extra clock cycle delay
    "    out  %[port], %[sda0]  \n" //    
    
    "    sbrc r0, 5             \n" // 
    "    out  %[pin], %[sda]    \n" //    
    "    out  %[pin], %[scl]    \n" //    
    "    cbi  %[port], %[sclb]  \n" // using cbi for extra extra clock cycle delay
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
    "    cbi  %[port],%[sclb]   \n" // using cbi for extra extra clock cycle delay
    "    out  %[port], %[sda0]  \n" // 
    
    "    sbrc r0, 0             \n" // 
    "    out  %[pin], %[sda]    \n" //    
    "    out  %[pin], %[scl]    \n" //    
    "    subi %A[len], 1        \n" // length-- part 1 (also serves as extra clock cycle delay)
    "    out  %[pin], %[scl]    \n" //    
    "    out  %[port], %[sda0]  \n" // SDA low for ACK   
    
    "    sbci %B[len], 0        \n" // length-- part 2 (also serves as extra clock cycle delay)
    "    out  %[pin], %[scl]    \n" // // clock ACK bit
    "    ld   r0, %a[ptr]+      \n" // fetch next buffer byte (also serves as clock delay)
    "    out  %[pin], %[scl]    \n" // 
    "    brne 1b                \n" // length != 0 do next byte
    :[ptr]   "+e" (image),
     [len]   "+d" (length)
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
    for (int i = 0; i < WIDTH; i++)
      i2c_sendByte(*(image++));
    i2c_stop();
  }
#elif defined(OLED_SH1106) || defined(LCD_ST7565)
  for (uint8_t i = 0; i < HEIGHT / 8; i++)
  {
  	LCDCommandMode();
  	SPDR = (OLED_SET_PAGE_ADDRESS + i);
	while (!(SPSR & _BV(SPIF)));
  	SPDR = (OLED_SET_COLUMN_ADDRESS_HI); // we only need to reset hi nibble to 0
	while (!(SPSR & _BV(SPIF)));
  	LCDDataMode();
  	for (uint8_t j = WIDTH; j > 0; j--)
      {
  		SPDR = *(image++);
		while (!(SPSR & _BV(SPIF)));
      }
  }
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
      [clear]   "r" (0)
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
  uint8_t c;
  int i = 0;

  SPDR = image[i++]; // set the first SPI data byte to get things started

  // the code to iterate the loop and get the next byte from the buffer is
  // executed while the previous byte is being sent out by the SPI controller
  while (i < (HEIGHT * WIDTH) / 8)
  {
    // get the next byte. It's put in a local variable so it can be sent as
    // as soon as possible after the sending of the previous byte has completed
    c = image[i++];

    while (!(SPSR & _BV(SPIF))) { } // wait for the previous byte to be sent

    // put the next byte in the SPI data register. The SPI controller will
    // clock it out while the loop continues and gets the next byte ready
    SPDR = c;
  }
  while (!(SPSR & _BV(SPIF))) { } // wait for the last byte to be sent
#endif
}

void ArduboyCore::blank()
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
 #elif defined(OLED_96X96) || defined(OLED_128X96) || defined(OLED_128X128) || defined(OLED_128X64_ON_128X96) || defined(OLED_128X64_ON_128X128)|| defined(OLED_128X96_ON_128X128) || defined(OLED_96X96_ON_128X128) || defined(OLED_64X128_ON_128X128)
  for (int i = 0; i < (HEIGHT * WIDTH) / 2; i++)
 #else //OLED SSD1306 and compatibles
  for (int i = 0; i < (HEIGHT * WIDTH) / 8; i++)
 #endif
    SPI.transfer(0x00);
#endif    
}

void ArduboyCore::sendLCDCommand(uint8_t command)
{
#if defined(OLED_SSD1306_I2C) || (OLED_SSD1306_I2CX) || (OLED_SH1106_I2C)
  i2c_start(SSD1306_I2C_CMD);
  i2c_sendByte(command);
  i2c_stop();
#else
  LCDCommandMode();
  SPI.transfer(command);
  LCDDataMode();
#endif
}

// invert the display or set to normal
// when inverted, a pixel set to 0 will be on
void ArduboyCore::invert(boolean inverse)
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
void ArduboyCore::allPixelsOn(boolean on)
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
void ArduboyCore::flipVertical(boolean flipped)
{
  sendLCDCommand(flipped ? OLED_VERTICAL_FLIPPED : OLED_VERTICAL_NORMAL);
}

#define OLED_HORIZ_FLIPPED 0xA0 // reversed segment re-map
#define OLED_HORIZ_NORMAL 0xA1 // normal segment re-map

// flip the display horizontally or set to normal
void ArduboyCore::flipHorizontal(boolean flipped)
{
  sendLCDCommand(flipped ? OLED_HORIZ_FLIPPED : OLED_HORIZ_NORMAL);
}

/* RGB LED */

void ArduboyCore::setRGBled(uint8_t red, uint8_t green, uint8_t blue)
{
#ifdef ARDUBOY_10 // RGB, all the pretty colors
 #if defined(LCD_ST7565)
  if ((red | green | blue) == 0) //prevent backlight off 
  {
    red   = 255;
    green = 255;
    blue  = 255;
  }
  analogWrite(RED_LED, red);
  analogWrite(GREEN_LED, green);
  analogWrite(BLUE_LED, blue);
 #else
  // inversion is necessary because these are common annode LEDs
  analogWrite(RED_LED, 255 - red);
  analogWrite(GREEN_LED, 255 - green);
  analogWrite(BLUE_LED, 255 - blue);
 #endif
#elif defined(AB_DEVKIT)
  // only blue on devkit
  digitalWrite(BLUE_LED, ~blue);
#endif
}

/* Buttons */

uint8_t ArduboyCore::getInput()
{
  return buttonsState();
}


uint8_t ArduboyCore::buttonsState()
{
  uint8_t buttons;
  
  // using ports here is ~100 bytes smaller than digitalRead()
#ifdef AB_DEVKIT
  // down, left, up
  buttons = ((~PINB) & B01110000);
  // right button
  //buttons = buttons | (((~PINC) & B01000000) >> 4);
  if ((PINC & B01000000) == 0) buttons |= 0x04; //compiles to shorter and faster code
  // A and B
  //buttons = buttons | (((~PINF) & B11000000) >> 6);
  if ((PINF & B10000000) == 0) buttons |= 0x02; //compiles to shorter and faster code
  if ((PINF & B01000000) == 0) buttons |= 0x01; 
#elif defined(ARDUBOY_10)
  // down, up, left right
  buttons = ((~PINF) & B11110000);
  // A (left)
  //buttons = buttons | (((~PINE) & B01000000) >> 3);
  if ((PINE & B01000000) == 0) {buttons |= 0x08;} //compiles to shorter and faster code
  // B (right)
  //buttons = buttons | (((~PINB) & B00010000) >> 2);
  if ((PINB & B00010000) == 0) {buttons |= 0x04;} //compiles to shorter and faster code
#endif
  
  return buttons;
}
