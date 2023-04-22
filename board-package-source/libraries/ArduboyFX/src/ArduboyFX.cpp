#include "ArduboyFX.h"
#include <wiring.c>

uint16_t FX::programDataPage; // program read only data location in flash memory
uint16_t FX::programSavePage; // program read and write data location in flash memory
Font     FX::font;
Cursor   FX::cursor = {0,0,0,WIDTH};

FrameControl FX::frameControl;


uint8_t FX::writeByte(uint8_t data)
{
  SPDR = data;
  asm volatile("nop\n");
  uint8_t result;
  do
  {
    result = SPDR; //reading data here saves 1 extra cycle
  }
  while ((SPSR & (1 << SPIF)) == 0);
  return result;
}


uint8_t FX::readByte()
{
  return writeByte(0);
}


void FX::begin()
{
  disableOLED();
  wakeUp();
}


void FX::begin(uint16_t developmentDataPage)
{
  disableOLED();
 #ifdef ARDUINO_ARCH_AVR
  const uint8_t* vector = (const uint8_t*)FX_DATA_VECTOR_KEY_POINTER;
  asm volatile
  (
    "lpm  r18, z+                       \n"
    "lpm  r19, z+                       \n"
    "lpm  r21, z+                       \n"
    "lpm  r20, z+                       \n"
    "subi r18, 0x18                     \n"
    "sbci r19, 0x95                     \n"
    "brne .+2                           \n" // skip if FX_DATA_VECTOR_KEY_POINTER != FX_VECTOR_KEY_VALUE
    "movw %A[devdata], r20              \n"
    "sts  %[datapage]+0, %A[devdata]    \n"
    "sts  %[datapage]+1, %B[devdata]    \n"
    :            "+&z" (vector),
      [devdata]  "+&r" (developmentDataPage)
    : [datapage] "" (&programDataPage)
    : "r18", "r19", "r20", "r21"
   );
 #else
  if (pgm_read_word(FX_DATA_VECTOR_KEY_POINTER) == FX_VECTOR_KEY_VALUE)
  {
    programDataPage = (pgm_read_byte(FX_DATA_VECTOR_PAGE_POINTER) << 8) | pgm_read_byte(FX_DATA_VECTOR_PAGE_POINTER + 1);
  }
  else
  {
    programDataPage = developmentDataPage;
  }
 #endif
  wakeUp();
}


void FX::begin(uint16_t developmentDataPage, uint16_t developmentSavePage)
{
  disableOLED();
 #ifdef ARDUINO_ARCH_AVR
  const uint8_t* vector = (const uint8_t*)FX_DATA_VECTOR_KEY_POINTER;
  asm volatile
  (
    "lpm  r18, z+                       \n"
    "lpm  r19, z+                       \n"
    "lpm  r21, z+                       \n"
    "lpm  r20, z+                       \n"
    "subi r18, 0x18                     \n"
    "sbci r19, 0x95                     \n"
    "brne .+2                           \n"  // skip if FX_DATA_VECTOR_KEY_POINTER != FX_VECTOR_KEY_VALUE
    "movw %A[devdata], r20              \n"
    "sts  %[datapage]+0, %A[devdata]    \n"
    "sts  %[datapage]+1, %B[devdata]    \n"
    "lpm  r18, z+                       \n"
    "lpm  r19, z+                       \n"
    "lpm  r21, z+                       \n"
    "lpm  r20, z+                       \n"
    "subi r18, 0x18                     \n"
    "sbci r19, 0x95                     \n"
    "brne .+2                           \n" //  skip if FX_SAVE_VECTOR_KEY_POINTER != FX_VECTOR_KEY_VALUE
    "movw %A[devsave], r20              \n"
    "sts  %[savepage]+0, %A[devsave]    \n"
    "sts  %[savepage]+1, %B[devsave]    \n"
    :            "+&z" (vector),
      [devdata]  "+&r" (developmentDataPage),
      [devsave]  "+&r" (developmentSavePage)
    : [datapage] "" (&programDataPage),
      [savepage] "" (&programSavePage)
    : "r18", "r19", "r20", "r21"
   );
 #else
  if (pgm_read_word(FX_DATA_VECTOR_KEY_POINTER) == FX_VECTOR_KEY_VALUE)
  {
    programDataPage = (pgm_read_byte(FX_DATA_VECTOR_PAGE_POINTER) << 8) | pgm_read_byte(FX_DATA_VECTOR_PAGE_POINTER + 1);
  }
  else
  {
    programDataPage = developmentDataPage;
  }
  if (pgm_read_word(FX_SAVE_VECTOR_KEY_POINTER) == FX_VECTOR_KEY_VALUE)
  {
    programSavePage = (pgm_read_byte(FX_SAVE_VECTOR_PAGE_POINTER) << 8) | pgm_read_byte(FX_SAVE_VECTOR_PAGE_POINTER + 1);
  }
  else
  {
    programSavePage = developmentSavePage;
  }
 #endif
  wakeUp();
}

void FX::readJedecID(JedecID & id)
{
  enable();
  writeByte(SFC_JEDEC_ID);
  id.manufacturer = readByte();
  id.device = readByte();
  id.size = readByte();
  disable();
}

void FX::readJedecID(JedecID* id)
{
  enable();
  writeByte(SFC_JEDEC_ID);
  id -> manufacturer = readByte();
  id -> device = readByte();
  id -> size = readByte();
  disable();
}

bool FX::detect()
{
  seekCommand(SFC_READ, 0);
  SPDR = 0;
  return readPendingLastUInt16() == 0x4152;
}


void FX::noFXReboot()
  {
    if (!detect())
    {
      do
      {
        if (*(uint8_t *)&timer0_millis & 0x80) bitSet(PORTB, RED_LED_BIT);
        else bitClear(PORTB, RED_LED_BIT);
      }
      while (bitRead(DOWN_BUTTON_PORTIN, DOWN_BUTTON_BIT)); // wait for DOWN button to enter bootloader
      Arduboy2Core::exitToBootloader();
    }
  }


void FX::writeCommand(uint8_t command)
{
  enable();
  writeByte(command);
  disable();
}

void FX::wakeUp()
{
  writeCommand(SFC_RELEASE_POWERDOWN);
}


void FX::sleep()
{
  writeCommand(SFC_POWERDOWN);
}

void FX::writeEnable()
{
  writeCommand(SFC_WRITE_ENABLE);
}


void FX::seekCommand(uint8_t command, uint24_t address)
{
  enable();
 #ifdef ARDUINO_ARCH_AVR
  register uint8_t cmd asm("r24") = command; //assembly optimizer for AVR platform ~saves 12 bytes
  asm volatile
  (
    "call %x2           \n"
    "mov  r24, %C[addr] \n"
    "call %x2           \n"
    "mov  r24, %B[addr] \n"
    "call %x2           \n"
    "mov  r24, %A[addr] \n"
    "jmp  %x2           \n"
    : [cmd]   "+&r" (cmd)
    : [addr]  "r"   (address),
      [write] "i" (writeByte)
    :
  );
 #else
  writeByte(command);
  writeByte(address >> 16);
  writeByte(address >> 8);
  writeByte(address);
 #endif
}


void FX::seekData(uint24_t address)
{
  uint24_t abs_address = address;
 #ifdef ARDUINO_ARCH_AVR
  asm volatile
  (
    "mov  %A[abs], %A[addr]  \n"
    "lds  %B[abs], %[page]+0 \n"
    "add  %B[abs], %B[addr]  \n"
    "lds  %C[abs], %[page]+1 \n"
    "adc  %C[abs], %C[addr]  \n"
    :
     [abs]  "=r" (abs_address)
    :[page] ""   (&programDataPage),
     [addr] "r"  (address)
    :
  );
 #else // C++ version for non AVR platforms
  abs_address = address + (uint24_t)programDataPage << 8;
 #endif
  seekCommand(SFC_READ, abs_address);
  SPDR = 0;
}


void FX::seekDataArray(uint24_t address, uint8_t index, uint8_t offset, uint8_t elementSize)
{
 #ifdef ARDUINO_ARCH_AVR
  register uint24_t addr asm("r22") = address;
  asm volatile
  (
    "   mul     %[index], %[size]   \n"
    "   brne    .+2                 \n" //treat size 0 as size 256
    "   mov     r1, %[index]        \n"
    "   clr     r21                 \n" //use as alternative zero reg
    "   add     r0, %[offset]       \n"
    "   adc     r1, r21             \n"
    "   add     %A[addr], r0        \n"
    "   adc     %B[addr], r1        \n"
    "   adc     %C[addr], r21       \n"
    "   clr     r1                  \n"
    "   jmp     %x4                 \n" //seekData
    : [addr]    "+r"  (addr)
    : [index]   "r"  (index),
      [offset]  "r"  (offset),
      [size]    "r"  (elementSize),
                ""   (seekData)
    : "r21"
  );
 #else
  address += elementSize ? index * elementSize + offset : index * 256 + offset;
  seekData(address);
 #endif
}


void FX::seekSave(uint24_t address)
{
 #ifdef ARDUINO_ARCH_AVR
  uint24_t abs_address = address;
  asm volatile
  (
    "mov  %A[abs], %A[addr]  \n"
    "lds  %B[abs], %[page]+0 \n"
    "add  %B[abs], %B[addr]  \n"
    "lds  %C[abs], %[page]+1 \n"
    "adc  %C[abs], %C[addr]  \n"
    :
     [abs]  "=r" (abs_address)
    :[page] ""    (&programSavePage),
     [addr] "r"   (address)
    :
  );
 #else // C++ version for non AVR platforms
  abs_address = address + (uint24_t)programSavePage << 8;
 #endif
  seekCommand(SFC_READ, abs_address);
  SPDR = 0;
}


uint8_t FX::readPendingUInt8()
{
  wait();
  uint8_t result = SPDR;
  SPDR = 0;
  return result;
}


uint8_t FX::readPendingLastUInt8()
{
  wait();                 // wait for a pending read to complete
  return readUnsafeEnd(); // read last byte and disable flash
}


uint16_t FX::readPendingUInt16()
{
 #ifdef ARDUINO_ARCH_AVR // Assembly implementation for AVR platform
  uint16_t result asm("r24"); // we want result to be assigned to r24,r25
  asm volatile
  (
    "call %x1           \n"
    "mov  %B[val], r24  \n"
    "jmp  %x1           \n"
    : [val] "=&r" (result)
    : "" (readPendingUInt8)
    :
  );
  return result;
 #else //C++ implementation for non AVR platforms
  return ((uint16_t)readPendingUInt8() << 8) | (uint16_t)readPendingUInt8();
 #endif
}


uint16_t FX::readPendingLastUInt16()
{
 #ifdef ARDUINO_ARCH_AVR // Assembly implementation for AVR platform
  uint16_t result asm("r24"); // we want result to be assigned to r24,r25
  asm volatile
  (
    "call %x1           \n"
    "mov  %B[val], r24  \n"
    "jmp  %x2           \n"
    : [val] "=&r" (result)
    : "" (readPendingUInt8),
      "" (readEnd)
    :
  );
  return result;
 #else //C++ implementation for non AVR platforms
  return ((uint16_t)readPendingUint8() << 8) | (uint16_t)readEnd();
 #endif
}


uint24_t FX::readPendingUInt24()
{
 #ifdef ARDUINO_ARCH_AVR // Assembly implementation for AVR platform
  uint24_t result asm("r24"); // we want result to be assigned to r24,r25,r26
  asm volatile
  (
    "call %x1           \n"
    "mov  %B[val], r24  \n"
    "call %x2           \n"
    "mov  %A[val], r24  \n"
    "mov  %C[val], r25  \n"
    : [val] "=&r" (result)
    : "" (readPendingUInt16),
      "" (readPendingUInt8)
    :
  );
  return result;
 #else //C++ implementation for non AVR platforms
  return ((uint24_t)readPendingUInt16() << 8) | readPendingUInt8();
 #endif
}


uint24_t FX::readPendingLastUInt24()
{
 #ifdef ARDUINO_ARCH_AVR // Assembly implementation for AVR platform
  uint24_t result asm("r24"); // we want result to be assigned to r24,r25,r26
  asm volatile
  (
    "call %x1           \n"
    "mov  %B[val], r24  \n"
    "call %x2           \n"
    "mov  %A[val], r24  \n"
    "mov  %C[val], r25  \n"
    : [val] "=&r" (result)
    : "" (readPendingUInt16),
      "" (readEnd)
    :
  );
  return result;
 #else //C++ implementation for non AVR platforms
  return ((uint24_t)readPendingUInt16() << 8) | readEnd();
 #endif
}


uint32_t FX::readPendingUInt32()
{
 #ifdef ARDUINO_ARCH_AVR //Assembly implementation for AVR platform
  uint32_t result asm("r24"); // we want result to be assigned to r24,r25,r26,r27
  asm volatile
  (
    "call %x1           \n"
    "movw  %C[val], r24 \n"
    "call %x1           \n"
    "movw  %A[val], r24 \n"
    : [val] "=&r" (result)
    : "" (readPendingUInt16)
    :
  );
  return result;
 #else //C++ implementation for non AVR platforms
  return ((uint32_t)readPendingUInt16() << 16) | readPendingUInt16();
 #endif
}


uint32_t FX::readPendingLastUInt32()
{
 #ifdef ARDUINO_ARCH_AVR //Assembly implementation for AVR platform
  uint32_t result asm("r24"); // we want result to be assigned to r24,r25,r26,r27
  asm volatile
  (
    "call %x1           \n"
    "movw  %C[val], r24 \n"
    "call %x2           \n"
    "movw  %A[val], r24 \n"
    : [val] "=&r" (result)
    : "" (readPendingUInt16),
      "" (readPendingLastUInt16)
    :
  );
  return result;
 #else //C++ implementation for non AVR platforms
  return ((uint32_t)readPendingUInt16() << 16) | readPendingLastUInt16();
 #endif
}


void FX::readBytes(uint8_t* buffer, size_t length)
{
 #ifdef ARDUINO_ARCH_AVR
  asm volatile(
    "1:              \n"
    "call %x2        \n"
    "st z+,r24       \n"
    "subi %A[len], 1 \n"
    "sbci %B[len], 0 \n"
    "brne 1b         \n"
    :      "+&z" (buffer),
     [len] "+&d" (length)
    : ""    (readPendingUInt8)
    : "r24"
  );
 #else
  for (size_t i = 0; i < length; i++)
  {
    buffer[i] = readPendingUInt8();
  }
 #endif
}


void FX::readBytesEnd(uint8_t* buffer, size_t length)
{
#ifdef ARDUINO_ARCH_AVR
  asm volatile(
    "1:                 \n"
    "subi %A[len], 1    \n"
    "sbci %B[len], 0    \n"
    "breq 2f            \n"
    "                   \n"
    "call %x2           \n"
    "st   z+, r24       \n"
    "rjmp 1b            \n"
    "2:                 \n"
    "call %x3           \n"
    "st   z, r24        \n"

    :      "+&z" (buffer),
     [len] "+&d" (length)
    : ""    (readPendingUInt8),
      ""    (readEnd)
    : "r24"
  );
 #else
   for (size_t i = 0; i <= length; i++)
  {
    if ((i+1) != length)
    buffer[i] = readPendingUInt8();
    else
    {
      buffer[i] = readEnd();
      break;
    }
  }
 #endif
}


uint8_t FX::readEnd()
{
  wait();                 // wait for a pending read to complete
  return readUnsafeEnd(); // read last byte and disable flash
}


void FX::readDataBytes(uint24_t address, uint8_t* buffer, size_t length)
{
  seekData(address);
  readBytesEnd(buffer, length);
}


void FX::readSaveBytes(uint24_t address, uint8_t* buffer, size_t length)
{
  seekSave(address);
  readBytesEnd(buffer, length);
}

uint8_t FX::loadGameState(uint8_t* gameState, size_t size) // ~54 bytes
{
 #ifdef ARDUINO_ARCH_AVR
  uint8_t result asm("r24");
  asm volatile(
    "ldi  r22, 0        \n" //seekSave(0)
    "ldi  r23, 0        \n"
    "ldi  r24, 0        \n"
    "call %x3           \n" //seekSave uses r20, r21, r22, r23, r24
    "movw r18, r26      \n" //save size
    "movw r20, r30      \n" //save gameState
    "0:                 \n"
    "ldi  r22,0         \n" //result = 0
    "1:                 \n"
    "call %x4           \n" //readPendingUInt16 uses r24, r25
    "cp   r24, r18      \n"
    "cpc  r25, r19      \n"
    "brne 4f            \n" //if (readPendingUInt16 != size) break
    "                   \n"
    "movw r26, r18      \n" //restore size
    "movw r30, r20      \n" //restore gameState
    "ldi  r22, 0        \n" //result = 0
    "2:                 \n" //do
    "call %x5           \n" //  data = readPendingUint8
    "st   z+, r24       \n" //  addr = data
    "sbiw r26, 1        \n" //  size--
    "brne 2b            \n" //until size == 0
    "ldi  r22, 1        \n" //result = 1
    "rjmp 1b            \n" //next
    "4:                 \n"
    "call %x6           \n" //readEnd
    "mov  r24, r22      \n" //return result
    : [addr] "+&z" (gameState),
      [size] "+&x" (size),
      [val]  "=&r"  (result)
    :        "i"   (seekSave),
             "i"   (readPendingUInt16),
             "i"   (readPendingUInt8),
             "i"   (readEnd)
    : "r18", "r19", "r20", "r21"
  );
 #else
  seekSave(0);
  uint8_t result = 0;
  while (readPendingUInt16() == size) // if gameState size not equal, most recent gameState has been read or there is no gameState
  {
    for (uint16_t i = 0; i < size; i++)
    {
      uint8_t data = readPendingUInt8();
      gameState[i] = data;
    }
    {
      result = 1; // signal gameState loaded
    }
  }
  readEnd();
 #endif
  return result;
}

void FX::saveGameState(const uint8_t* gameState, size_t size) // ~152 bytes locates free space in 4K save block and saves the GamesState.
{                                                       //            if there is not enough free space, the block is erased prior to saving
 register size_t sz asm("r18") = size;
 #ifdef ARDUINO_ARCH_AVR
  asm volatile(
    "ldi  r26, 0                \n" //addr = 0
    "ldi  r27, 0                \n"
    "1:                         \n"
    "movw r22, r26              \n" //uint24_t addr
    "ldi  r24, 0                \n"
    "call %x2                   \n" //seekSave uses r20, r21, r22, r23, r24
    "call %x3                   \n" //readPendingLastUInt16 uses r24, r25
    "movw r22, r26              \n" //save addr
    "adiw r26, 2                \n" //addr += 2 for size word
    "add  r26, r18              \n" //addr += size
    "adc  r27, r19              \n"
    "cp   r24, r18              \n"
    "cpc  r25, r19              \n"
    "breq 1b                    \n" //readPendingLastUInt16 == size
    "                           \n"
    "subi r24, 0xFF             \n" //if result of readPendingLastUInt16 != 0xFFFF
    "sbci r25, 0xFF             \n"
    "brne 2f                    \n" //erase block
    "                           \n"
    "subi r26, lo8(4094+1)      \n"
    "sbci r27, hi8(4094+1)      \n"// addr < 4094+1 (last two bytes in 4K block always 0xFF)
    "movw r26, r22              \n"// addr -= size - 2  point to start of free space
    "brcs 3f                    \n"
    "2:                         \n" //erase 4K save block at addr
    "call %x4                   \n" //writeEnable
    "ldi  r20, 0                \n"
    "lds  r21, %[page]+0        \n"
    "lds  r22, %[page]+1        \n"
    "ldi  r24, %[erase]         \n" //SFC_ERASE
    "call %x5                   \n" //seekCommand
    "sbi  %[fxport], %[fxbit]   \n" //disable
    "call %x6                   \n" //waitWhileBusy
    "ldi  r26, 0                \n" //addr = 0
    "ldi  r27, 0                \n"
    "3:                         \n"
    "ldi  r23, 0xFC             \n" // int8_t shiftstate = -4
    "4:                         \n"
    "call %x4                   \n" //writeEnable
    "mov  r20, r26              \n" //addr
    "lds  r21, %[page]+0        \n"
    "add  r21, r27              \n"
    "lds  r22, %[page]+1        \n"
    "adc  r22, r1               \n"
    "ldi  r24, %[write]         \n" //SFC_WRITE
    "call %x5                   \n" //seekCommand
    "5:                         \n"
    "mov  r24, r19              \n"
    "sbrc r23, 1                \n" //if (shiftstate & 3 == 0) writebyte(size >> 8)
    "mov  r24, r18              \n" //if (shiftstate & 3 == 2) writebyte(size & 0xFF)
    "sbrc r23, 0                \n" //else writeByte(gameState++);
    "ld   r24, z+               \n" //saveState
    "call %x7                   \n" //writeByte
    "asr  r23                   \n" //shiftstate >>= 1
    "brcc .+6                   \n" //if (shiftstate == -1) size--
    "subi r18, 1                \n"
    "sbci r19, 0                \n"
    "breq 6f                    \n" //size == 0
    "                           \n"
    "adiw r26, 1                \n" //addr++
    "and  r26, r26              \n"
    "brne 5b                    \n" //while addr & 0xFF != 0 (not end of page)
    "6:                         \n"
    "sbi  %[fxport], %[fxbit]   \n" //disable
    "call %x6                   \n" //waitWhileBusy
    "cp   r18, r1               \n"
    "cpc  r19, r1               \n"
    "brne 4b                    \n" //while size != 0
    :[state]  "+&z" (gameState),
     [size]   "+&r" (sz)
    :         ""    (seekSave),
              ""    (readPendingLastUInt16),
              ""    (writeEnable),
              ""    (seekCommand),
              ""    (waitWhileBusy),
              ""    (writeByte),
     [fxport] "i"   (_SFR_IO_ADDR(FX_PORT)),
     [fxbit]  "i"   (FX_BIT),
     [erase]  "i"   (SFC_ERASE),
     [write]  "i"   (SFC_WRITE),
     [page]   ""    (&programSavePage)
    : "r20", "r21", "r22", "r23", "r24", "r25", "r26", "r27"
  );
 #else
  uint16_t addr = 0;
  for(;;) // locate end of previous gameStates
  {
    seekSave(addr);
    if (readPendingLastUInt16() != size) break; //found end of previous gameStates
    addr += size + 2;
  }
  if ((addr + size) > 4094) //is there enough space left? last two bytes of 4K block must always be unused (0xFF)
  {
    eraseSaveBlock(0); // erase save block
    waitWhileBusy();   // wait for erase to complete
    addr = 0;          // write saveState at the start of block
  }

  while (size)
  {
    writeEnable();
    seekCommand(SFC_WRITE, (uint24_t)(programSavePage << 8) + addr);
    do
    {
      writeByte(*gameState++);
      if (--size == 0) break;
    }
    while ((uint8_t)++addr); // write bytes until end of a page
    disable();               // start writing the (partial) page
    waitWhileBusy();         // wait for page write to complete
  }
 #endif
}

void  FX::eraseSaveBlock(uint16_t page)
{
  writeEnable();
  seekCommand(SFC_ERASE, (uint24_t)(programSavePage + page) << 8);
  disable();
}


void FX::writeSavePage(uint16_t page, uint8_t* buffer)
{
  writeEnable();
  seekCommand(SFC_WRITE, (uint24_t)(programSavePage + page) << 8);
  uint8_t i = 0;
  do
  {
    writeByte(buffer[i]);
  }
  while (i++ < 255);
  disable();
}

void FX::waitWhileBusy()
{
  enable();
  writeByte(SFC_READSTATUS1);
  while(readByte() & 1)
    ; // wait while BUSY status bit is set
  disable();
}

void FX::drawBitmap(int16_t x, int16_t y, uint24_t address, uint8_t frame, uint8_t mode)
{
  // read bitmap dimensions from flash
  seekData(address);
  int16_t width  = readPendingUInt16();
  int16_t height = readPendingLastUInt16();
  // return if the bitmap is completely off screen
  if (x + width <= 0 || x >= WIDTH || y + height <= 0 || y >= HEIGHT) return;

  // determine render width
  int16_t skipleft = 0;
  uint8_t renderwidth;
  if (x<0)
  {
    skipleft = -x;
    if (width - skipleft < WIDTH) renderwidth = width - skipleft;
    else renderwidth = WIDTH;
  }
  else
  {
    if (x + width > WIDTH) renderwidth = WIDTH - x;
    else renderwidth = width;
  }

  //determine render height
  int16_t skiptop;     // pixel to be skipped at the top
  int8_t renderheight; // in pixels
  if (y < 0)
  {
    skiptop = -y & -8; // optimized -y / 8 * 8
    if (height - skiptop <= HEIGHT) renderheight = height - skiptop;
    else renderheight = HEIGHT + (-y & 7);
    skiptop = fastDiv8(skiptop); // pixels to displayrows
  }
  else
  {
    skiptop = 0;
    if (y + height > HEIGHT) renderheight = HEIGHT - y;
    else renderheight = height;
  }
  uint24_t offset = (uint24_t)(frame * (fastDiv8(height+(uint16_t)7)) + skiptop) * width + skipleft;
  if (mode & dbmMasked)
  {
    offset += offset; // double for masked bitmaps
    width += width;
  }
  address += offset + 4; // skip non rendered pixels, width, height
  int8_t displayrow = (y >> 3) + skiptop;
  uint16_t displayoffset = displayrow * WIDTH + x + skipleft;
  uint8_t yshift = bitShiftLeftUInt8(y); //shift by multiply
#ifdef ARDUINO_ARCH_AVR
  uint8_t rowmask;
  uint16_t bitmap;
  asm volatile(
    "1: ;render_row:                                \n"
    "   cbi     %[fxport], %[fxbit]                 \n"
    "   ldi     r24, %[cmd]                         \n" // writeByte(SFC_READ);
    "   out     %[spdr], r24                        \n"
    "   lds     r24, %[datapage]+0                  \n" // address + programDataPage;
    "   lds     r25, %[datapage]+1                  \n"
    "   add     r24, %B[address]                    \n"
    "   adc     r25, %C[address]                    \n"
    "   in      r0, %[spsr]                         \n" // wait()
    "   sbrs    r0, %[spif]                         \n"
    "   rjmp    .-6                                 \n"
    "   out     %[spdr], r25                        \n" // writeByte(address >> 16);
    "   in      r0, %[spsr]                         \n" // wait()
    "   sbrs    r0, %[spif]                         \n"
    "   rjmp    .-6                                 \n"
    "   out     %[spdr], r24                        \n" // writeByte(address >> 8);
    "   in      r0, %[spsr]                         \n" // wait()
    "   sbrs    r0, %[spif]                         \n"
    "   rjmp    .-6                                 \n"
    "   out     %[spdr], %A[address]                \n" // writeByte(address);
    "                                               \n"
    "   add     %A[address], %A[width]              \n" // address += width;
    "   adc     %B[address], %B[width]              \n"
    "   adc     %C[address], r1                     \n"
    "   in      r0, %[spsr]                         \n" // wait();
    "   sbrs    r0, %[spif]                         \n"
    "   rjmp    .-6                                 \n"
    "   out     %[spdr], r1                         \n" // SPDR = 0;
    "                                               \n"
    "   lsl     %[mode]                             \n" // 'clear' mode dbfExtraRow by shifting into carry
    "   cpi     %[displayrow], %[lastrow]           \n"
    "   brge    .+4                                 \n" // row >= lastrow, clear carry
    "   sec                                         \n" // row < lastrow set carry
    "   sbrc    %[yshift], 0                        \n" // yshift != 1, don't change carry state
    "   clc                                         \n" // yshift == 1, clear carry
    "   ror     %[mode]                             \n" // carry to mode dbfExtraRow
    "                                               \n"
    "   ldi     %[rowmask], 0x02                    \n" // rowmask = 0xFF >> (8 - (height & 7));
    "   sbrc    %[renderheight], 1                  \n"
    "   ldi     %[rowmask], 0x08                    \n"
    "   sbrc    %[renderheight], 2                  \n"
    "   swap    %[rowmask]                          \n"
    "   sbrs    %[renderheight], 0                  \n"
    "   lsr     %[rowmask]                          \n"
    "   dec     %[rowmask]                          \n"
    "   breq    .+4                                 \n"
    "   cpi     %[renderheight], 8                  \n" // if (renderheight >= 8) rowmask = 0xFF;
    "   brlt    .+2                                 \n"
    "   ldi     %[rowmask], 0xFF                    \n"
    "                                               \n"
    "   mov     r25, %[renderwidth]                 \n" // for (c < renderwidth)
    "2: ;render_column:                             \n"
    "   in      r0, %[spdr]                         \n" // read bitmap data
    "   out     %[spdr], r1                         \n" // start next read
    "                                               \n"
    "   sbrc    %[mode], %[reverseblack]            \n" // test reverse mode
    "   eor     r0, %[rowmask]                      \n" // reverse bitmap data
    "   mov     r24, %[rowmask]                     \n" // temporary move rowmask
    "   sbrc    %[mode], %[whiteblack]              \n" // for black and white modes:
    "   mov     r24, r0                             \n" // rowmask = bitmap
    "   sbrc    %[mode], %[black]                   \n" // for black mode:
    "   clr     r0                                  \n" // bitmap = 0
    "   mul     r0, %[yshift]                       \n"
    "   movw    %[bitmap], r0                       \n" // bitmap *= yshift
    "   bst     %[mode], %[masked]                  \n" // if bitmap has no mask:
    "   brtc    3f ;render_mask                     \n" // skip next part
    "                                               \n"
    "   lpm                                         \n" // above code took 11 cycles, wait 7 cycles more for SPI data ready
    "   lpm                                         \n"
    "   clr     r1                                  \n" // restore zero reg
    "                                               \n"
    "   in      r0, %[spdr]                         \n" // read mask data
    "   out     %[spdr],r1                          \n" // start next read
    "   sbrc    %[mode], %[whiteblack]              \n" //
    "3: ;render_mask:                               \n"
    "   mov     r0, r24                             \n" // get mask in r0
    "   mul     r0, %[yshift]                       \n" // mask *= yshift
    ";render_page0:                                 \n"
    "   cpi     %[displayrow], 0                    \n" // skip if displayrow < 0
    "   brlt    4f ;render_page1                    \n"
    "                                               \n"
    "   ld      r24, %a[buffer]                     \n" // do top row or to row half
    "   sbrs    %[mode],%[invert]                   \n" // skip 1st eor for invert mode
    "   eor     %A[bitmap], r24                     \n"
    "   and     %A[bitmap], r0                      \n" // and with mask LSB
    "   eor     %A[bitmap], r24                     \n"
    "   st      %a[buffer], %A[bitmap]              \n"
    "4: ;render_page1:                              \n"
    "   subi    %A[buffer], lo8(-%[displaywidth])   \n"
    "   sbci    %B[buffer], hi8(-%[displaywidth])   \n"
    "   sbrs    %[mode], %[extrarow]                \n" // test if ExtraRow mode:
    "   rjmp    5f ;render_next                     \n" // else skip
    "                                               \n"
    "   ld      r24, %a[buffer]                     \n" // do shifted 2nd half
    "   sbrs    %[mode], %[invert]                  \n" // skip 1st eor for invert mode
    "   eor     %B[bitmap], r24                     \n"
    "   and     %B[bitmap], r1                      \n"// and with mask MSB
    "   eor     %B[bitmap], r24                     \n"
    "   st      %a[buffer], %B[bitmap]              \n"
    "5: ;render_next:                               \n"
    "   clr     r1                                  \n" // restore zero reg
    "   subi    %A[buffer], lo8(%[displaywidth]-1)  \n"
    "   sbci    %B[buffer], hi8(%[displaywidth]-1)  \n"
    "   dec     r25                                 \n"
    "   brne    2b ;render_column                   \n" // for (c < renderheigt) loop
    "                                               \n"
    "   subi    %A[buffer], lo8(-%[displaywidth])   \n" // buffer += WIDTH - renderwidth
    "   sbci    %B[buffer], hi8(-%[displaywidth])   \n"
    "   sub     %A[buffer], %[renderwidth]          \n"
    "   sbc     %B[buffer], r1                      \n"
    "   subi    %[renderheight], 8                  \n" // reinderheight -= 8
    "   inc     %[displayrow]                       \n" // displayrow++
    "   in      r0, %[spsr]                         \n" // clear SPI status
    "   sbi     %[fxport], %[fxbit]                 \n" // disable external flash
    "   cp      r1, %[renderheight]                 \n" // while (renderheight > 0)
    "   brge    .+2                                 \n"
    "   rjmp    1b ;render_row                      \n"
   :
    [address]      "+r" (address),
    [mode]         "+r" (mode),
    [rowmask]      "=&d" (rowmask),
    [bitmap]       "=&r" (bitmap),
    [renderheight] "+d" (renderheight),
    [displayrow]   "+d" (displayrow)
   :
    [width]        "r" (width),
    [height]       "r" (height),
    [yshift]       "r" (yshift),
    [renderwidth]  "r" (renderwidth),
    [buffer]       "e" (Arduboy2Base::sBuffer + displayoffset),

    [fxport]       "I" (_SFR_IO_ADDR(FX_PORT)),
    [fxbit]        "I" (FX_BIT),
    [cmd]          "I" (SFC_READ),
    [spdr]         "I" (_SFR_IO_ADDR(SPDR)),
    [datapage]     ""  (&programDataPage),
    [spsr]         "I" (_SFR_IO_ADDR(SPSR)),
    [spif]         "I" (SPIF),
    [lastrow]      "I" (HEIGHT / 8 - 1),
    [displaywidth] ""  (WIDTH),
    [reverseblack] "I" (dbfReverseBlack),
    [whiteblack]   "I" (dbfWhiteBlack),
    [black]        "I" (dbfBlack),
    [masked]       "I" (dbfMasked),
    [invert]       "I" (dbfInvert),
    [extrarow]     "I" (dbfExtraRow)
   :
    "r24", "r25"
   );
#else
  uint8_t lastmask = bitShiftRightMaskUInt8(8 - height); // mask for bottom most pixels
  do
  {
    seekData(address);
    address += width;
    mode &= ~((1 << dbfExtraRow));
    if (yshift != 1 && displayrow < (HEIGHT / 8 - 1)) mode |= (1 << dbfExtraRow);
    uint8_t rowmask = 0xFF;
    if (renderheight < 8) rowmask = lastmask;
    wait();
    for (uint8_t c = 0; c < renderwidth; c++)
    {
      uint8_t bitmapbyte = readUnsafe();
      if (mode & (1 << dbfReverseBlack)) bitmapbyte ^= rowmask;
      uint8_t maskbyte = rowmask;
      if (mode & (1 << dbfWhiteBlack)) maskbyte = bitmapbyte;
      if (mode & (1 << dbfBlack)) bitmapbyte = 0;
      uint16_t bitmap = multiplyUInt8(bitmapbyte, yshift);
      if (mode & (1 << dbfMasked))
      {
        wait();
        uint8_t tmp = readUnsafe();
        if ((mode & dbfWhiteBlack) == 0) maskbyte = tmp;
      }
      uint16_t mask = multiplyUInt8(maskbyte, yshift);
      if (displayrow >= 0)
      {
        uint8_t pixels = bitmap;
        uint8_t display = Arduboy2Base::sBuffer[displayoffset];
        if ((mode & (1 << dbfInvert)) == 0) pixels ^= display;
        pixels &= mask;
        pixels ^= display;
        Arduboy2Base::sBuffer[displayoffset] = pixels;
      }
      if (mode & (1 << dbfExtraRow))
      {
        uint8_t display = Arduboy2Base::sBuffer[displayoffset + WIDTH];
        uint8_t pixels = bitmap >> 8;
        if ((mode & dbfInvert) == 0) pixels ^= display;
        pixels &= mask >> 8;
        pixels ^= display;
        Arduboy2Base::sBuffer[displayoffset + WIDTH] = pixels;
      }
      displayoffset++;
    }
    displayoffset += WIDTH - renderwidth;
    displayrow ++;
    renderheight -= 8;
    readEnd();
  } while (renderheight > 0);
#endif
}

void FX::setFrame(uint24_t frame, uint8_t repeat) //~22 bytes
{
 #ifdef ARDUINO_ARCH_AVR
  FrameControl* ctrl = &frameControl;
  asm volatile(
    "ldi    r30, lo8(%[ctrl]) \n"
    "ldi    r31, hi8(%[ctrl]) \n"
    "st     z,   %A[frame]    \n" // start
    "std    z+1, %B[frame]    \n"
    "std    z+2, %C[frame]    \n"
    "std    z+3, %A[frame]    \n" // current
    "std    z+4, %B[frame]    \n"
    "std    z+5, %C[frame]    \n"
    "std    z+6, %A[repeat]   \n" // repeat
    "std    z+7, %A[repeat]   \n" // count
    :
    : [ctrl]   ""  (ctrl),
      [frame]  "r" (frame),
      [repeat] "r" (repeat)
    : //"r30", "r31"
  );
 #else
  frameControl.start   = frame;
  frameControl.current = frame;
  frameControl.repeat  = repeat;
  frameControl.count   = repeat;
  #endif
}

uint8_t FX::drawFrame() // ~66 bytes
{
  uint24_t frame = drawFrame(frameControl.current);
  uint8_t moreFrames;
 #ifdef ARDUINO_ARCH_AVR
  FrameControl* ctrl = &frameControl;
  asm volatile(
    "ldi    r30, lo8(%[ctrl])   \n"
    "ldi    r31, hi8(%[ctrl])   \n"
    "ldd    r0, z+7             \n" // frameControl.count
    "mov    %[more], r0         \n" // moreFrames = (frame != 0) | frameControl.count;
    "or     %[more], %A[frame]  \n"
    "or     %[more], %B[frame]  \n"
    "or     %[more], %C[frame]  \n"
    "tst    r0                  \n"
    "breq   1f                  \n" // skip frameControl.count == 0
    "                           \n"
    "dec    r0                  \n"
    "std    z+7, r0             \n" // frameControl.count--
    "rjmp   3f                  \n" // return
    "1:                         \n"
    "ldd    r0, z+6             \n" // frameControl.count = frameControl.repeat
    "std    z+7, r0             \n"
    "tst    %[more]             \n" //
    "brne   2f                  \n" // if if moreFrames skip
    "                           \n"
    "ld     %A[frame], z        \n" // frame = frameControl.start
    "ldd    %B[frame], z+1      \n"
    "ldd    %C[frame], z+2      \n"
    "2:                         \n"
    "std    z+3, %A[frame]      \n" // frameControl.current = frame
    "std    z+4, %B[frame]      \n"
    "std    z+5, %C[frame]      \n"
    "3:                         \n"
    : [more]  "=&r" (moreFrames)
    : [ctrl]  ""   (ctrl),
      [frame] "r"  (frame)
    : "r0", "r30", "r31"
    );
 #else
  moreFrames = (frame != 0) | frameControl.count;
  if (frameControl.count > 0)
  {
    frameControl.count--;
  }
  else
  {
    frameControl.count = frameControl.repeat;
    if (!moreFrames) frame = frameControl.start;
    frameControl.current = frame;
  }
 #endif
  return moreFrames;
}

uint24_t FX::drawFrame(uint24_t address) //~94 bytes
{
  FrameData f;
 #ifdef ARDUINO_ARCH_AVR
  asm volatile (
    "push   r6                  \n"
    "push   r7                  \n"
    "push   r8                  \n"
    "push   r14                 \n"
    "push   r16                 \n"
    "0:                         \n"
    "movw   r6, %A[addr]        \n" //save address for calls
    "mov    r8, %C[addr]        \n"
    "call   %x6                 \n"
    "call   %x1                 \n"
    "movw   r30, r24            \n" //temporary save x
    "call   %x1                 \n"
    "movw   r26, r24            \n" //temporary save y
    "call   %x2                 \n"
    "mov    r20, r24            \n" // bmp address
    "movw   r18, r22            \n"
    "call   %x3                 \n"
    "mov    r16, r24            \n" // frame
    "call   %x4                 \n"
    "mov    r14, r24            \n" // mode
    "movw   r24, r30            \n" // x
    "movw   r22, r26            \n" // y
    "call   %x5                 \n" // drawbitmap
    "movw   %A[addr], r6        \n" // restore address
    "mov    %C[addr], r8        \n"
    "subi   %A[addr], -%[size]  \n"
    "sbci   %B[addr], 0xFF      \n"
    "sbci   %C[addr], 0xFF      \n"
    "                           \n"
    "sbrc   r14, 6              \n" // test next frame
    "rjmp   1f                  \n" // skip end of this frame
    "                           \n"
    "sbrs   r14, 7              \n" // test last frame
    "rjmp   0b                  \n" // loop not last frame
    "                           \n"
    "clr    %A[addr]            \n"
    "clr    %B[addr]            \n"
    "clr    %C[addr]            \n"
    "1:                         \n"
    "pop    r16                 \n"
    "pop    r14                 \n"
    "pop    r8                  \n"
    "pop    r7                  \n"
    "pop    r6                  \n"

  : [addr] "+&r" (address)
  : "" (readPendingUInt16),
    "" (readPendingUInt24),
    "" (readPendingUInt8),
    "" (readEnd),
    "" (drawBitmap),
    "" (seekData),
    [size] "" (sizeof(f))
  : "r18","r19", "r20", "r21", "r25", "r26", "r27", "r30", "r31"
  );
  return address;
 #else
  for(;;)
  {
    seekData(address);
    address += sizeof(f);
    f.x = readPendingUInt16();
    f.y = readPendingUInt16();
    f.bmp = readPendingUInt24();
    f.frame = readPendingUInt8();
    f.mode = readEnd();
    drawBitmap(f.x, f.y, f.bmp, f.frame, f.mode);
    if (f.mode & dbmEndFrame) return address;
    if (f.mode & dbmLastFrame) return 0;
  }
 #endif
}

void FX::readDataArray(uint24_t address, uint8_t index, uint8_t offset, uint8_t elementSize, uint8_t* buffer, size_t length)
{
  seekDataArray(address, index, offset, elementSize);
  readBytesEnd(buffer, length);
}


uint8_t FX::readIndexedUInt8(uint24_t address, uint8_t index)
{
  seekDataArray(address, index, 0, sizeof(uint8_t));
  return readEnd();
}


uint16_t FX::readIndexedUInt16(uint24_t address, uint8_t index)
{
  seekDataArray(address, index, 0, sizeof(uint16_t));
  return readPendingLastUInt16();
}


uint24_t FX::readIndexedUInt24(uint24_t address, uint8_t index)
{
  seekDataArray(address, index, 0, sizeof(uint24_t));
  return readPendingLastUInt24();
}


uint32_t FX::readIndexedUInt32(uint24_t address, uint8_t index)
{
  seekDataArray(address, index, 0, sizeof(uint24_t));
  return readPendingLastUInt32();
}

void FX::displayPrefetch(uint24_t address, uint8_t* target, uint16_t len, bool clear)
{
  seekData(address);
  asm volatile
  (
    "   ldi     r30, lo8(%[sbuf])               \n" // uint8_t* ptr = Arduboy2::sBuffer;
    "   ldi     r31, hi8(%[sbuf])               \n"
    "   ldi     r25, hi8(%[end])                \n"
    "   in      r0, %[spsr]                     \n" // wait(); //for 1st target data recieved (can't enable OLED mid transfer)
    "   sbrs	r0, %[spif]                     \n"
    "   rjmp	.-6                             \n"
    "   cbi     %[csport], %[csbit]             \n" // enableOLED();
    "1:                                         \n" // while (true) {
    "   ld      r0, Z                   ;2      \n" // uint8_t displaydata = *ptr;
    "   in      r24, %[spdr]            ;1  /3  \n" // uint8_t targetdata = SPDR;
    "   out     %[spdr], r0             ;1      \n" // SPDR = displaydata;
    "   cpse    %[clear], r1            ;1-2    \n" // if (clear) displaydata = 0;
    "   mov     r0, r1                  ;1      \n"
    "   st      Z+, r0                  ;2      \n" // *ptr++ = displaydata;
    "   subi    %A[len], 1              ;1      \n" // if (--len >= 0) *target++ = targetdata;
    "   sbci    %B[len], 0              ;1      \n"
    "   brmi    3f                      ;1-2    \n" // branch ahead and back to 2 to burn 4 cycles
    "   nop                             ;1      \n"
    "   st      %a[target]+, r24        ;2  /11 \n"
    "2:                                         \n"
    "   cpi     r30, lo8(%[end])        ;1      \n" // if (ptr >= Arduboy2::sBuffer + WIDTH * HEIGHT / 8) break;
    "   cpc     r31, r25                ;1      \n"
    "   brcs    1b                      ;1-2/4  \n" // }
    "3:                                         \n"
    "   brmi    2b                      ;1-2    \n" // branch only when coming from above brmi
    : [target]   "+e" (target),
      [len]      "+d" (len)
    : [sbuf]     ""   (Arduboy2::sBuffer),
      [end]      ""   (Arduboy2::sBuffer + WIDTH * HEIGHT / 8),
      [clear]    "r" (clear),
      [spsr]     "I" (_SFR_IO_ADDR(SPSR)),
      [spif]     "I" (SPIF),
      [spdr]     "I" (_SFR_IO_ADDR(SPDR)),
      [csport]   "I" (_SFR_IO_ADDR(CS_PORT)),
      [csbit]    "I" (CS_BIT)
    : "r24", "r25", "r30", "r31"
  );
  disableOLED();
  disable();
  SPSR;
}

void FX::display()
{
  enableOLED();
  Arduboy2Base::display();
  disableOLED();
}

void FX::display(bool clear)
{
  enableOLED();
  Arduboy2Base::display(clear);
  disableOLED();
}

void FX::setFont(uint24_t address, uint8_t mode)
{
  font.address = address;
  font.mode = mode;
  seekData(address);
  font.width = readPendingUInt16();
  font.height = readPendingLastUInt16();
}

void FX::setFontMode(uint8_t mode)
{
  font.mode = mode;
}

void FX::setCursor(int16_t x, int16_t y)
{
  cursor.x = x;
  cursor.y = y;
}

void FX::setCursorX(int16_t x)
{
  cursor.x = x;
}

void FX::setCursorY(int16_t y)
{
  cursor.y = y;
}

void FX::setCursorRange(int16_t left, int16_t wrap)
{
  cursor.left = left;
  cursor.wrap = wrap;
}

void FX::setCursorLeft(int16_t left)
{
  cursor.left = left;
}

void FX::setCursorWrap(int16_t wrap)
{
  cursor.wrap = wrap;
}

void FX::drawChar(uint8_t c)
{
  if (c == '\r') return;
  uint8_t mode = font.mode;
  int16_t x = cursor.x;
  int16_t y = cursor.y;
  if (c != '\n')
  {
    drawBitmap(x, y, font.address, c, mode);
    if (mode & dcmProportional)
    {
      seekData(font.address - 256 + c);
      x += readEnd();
    }
    else
    {
      x += font.width;
    }
  }
  if ((c == '\n') || (x >= cursor.wrap))
  {
    x = cursor.left;
    y += font.height;
  }
  setCursor(x,y);
}

void FX::drawString(const uint8_t* buffer)
{
  for(;;)
  {
    uint8_t c = *buffer++;
    if (c) drawChar(c);
    else break;
  }
}

void FX::drawString(const char* str)
{
  FX::drawString((const uint8_t*)str);
}

void FX::drawString(uint24_t address)
{
  for(;;)
  {
    seekData(address++);
    uint8_t c = readEnd();
    if (c) drawChar(c);
    else break;
  }
}

void FX::drawNumber(int16_t n, int8_t digits)
{
  drawNumber((int32_t)n, digits);
}

void FX::drawNumber(uint16_t n, int8_t digits)
{
  drawNumber((uint32_t)n, digits);
}

void FX::drawNumber(int32_t n, int8_t digits)
{
  if (n < 0)
  {
    n = -n;
    drawChar('-');
  }
  else if (digits != 0)
  {
    drawChar(' ');
  }
  drawNumber((uint32_t)n, digits);
}

void FX::drawNumber(uint32_t n, int8_t digits) //
{
  uint8_t buf[33]; //max 32 digits + terminator
  uint8_t *str = &buf[sizeof(buf) - 1];
  *str = '\0';
  do {
    char c = n % 10;
    n /= 10;
    *--str = c + '0';
    if ((digits > 0) && (--digits == 0)) break;
    if ((digits < 0) && (++digits == 0)) break;
  } while(n);
    while (digits > 0) {--digits; *--str = '0';}
    while (digits < 0) {++digits; *--str = ' ';}
  drawString(str);
}
