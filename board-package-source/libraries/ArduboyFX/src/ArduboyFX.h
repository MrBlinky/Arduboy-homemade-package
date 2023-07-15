#ifndef ARDUBOYFX_H
#define ARDUBOYFX_H

// For uint8_t, uint16_t
#include <stdint.h>

// For size_t
#include <stddef.h>

// For ARDUINO_ARCH_AVR, PORTD, ...
#include <Arduino.h>

// For Arduboy2Base::sBuffer, WIDTH, HEIGHT, CS_PORT ...
#include <Arduboy2.h>

#if defined (CART_CS_RX)
  #define FX_PORT PORTD
  #define FX_BIT PORTD2
#elif defined (CART_CS_HWB)
  #define FX_PORT PORTE
  #define FX_BIT PORTE2
#else
  #define FX_PORT PORTD
  #define FX_BIT PORTD1
#endif


// progam data and save data pages(set by PC manager tool)
constexpr uint16_t FX_VECTOR_KEY_VALUE  = 0x9518;        /* RETI instruction used as magic key */
constexpr uint16_t FX_DATA_VECTOR_KEY_POINTER  = 0x0014; /* reserved interrupt vector 5  area */
constexpr uint16_t FX_DATA_VECTOR_PAGE_POINTER = 0x0016;
constexpr uint16_t FX_SAVE_VECTOR_KEY_POINTER  = 0x0018; /* reserved interrupt vector 6  area */
constexpr uint16_t FX_SAVE_VECTOR_PAGE_POINTER = 0x001A;

// Serial Flash Commands
constexpr uint8_t SFC_JEDEC_ID          = 0x9F;
constexpr uint8_t SFC_READSTATUS1       = 0x05;
constexpr uint8_t SFC_READSTATUS2       = 0x35;
constexpr uint8_t SFC_READSTATUS3       = 0x15;
constexpr uint8_t SFC_READ              = 0x03;
constexpr uint8_t SFC_WRITE_ENABLE      = 0x06;
constexpr uint8_t SFC_WRITE             = 0x02;
constexpr uint8_t SFC_ERASE             = 0x20;
constexpr uint8_t SFC_RELEASE_POWERDOWN = 0xAB;
constexpr uint8_t SFC_POWERDOWN         = 0xB9;

// drawbitmap bit flags (used by modes below and internally)
constexpr uint8_t dbfWhiteBlack   = 0; // bitmap is used as mask
constexpr uint8_t dbfInvert       = 1; // bitmap is exclusive or-ed with display
constexpr uint8_t dbfBlack        = 2; // bitmap will be blackened
constexpr uint8_t dbfReverseBlack = 3; // reverses bitmap data
constexpr uint8_t dbfMasked       = 4; // bitmap contains mask data
constexpr uint8_t dbfFlip         = 5; // mirror bitmap
constexpr uint8_t dbfExtraRow     = 7; // ignored (internal use)
constexpr uint8_t dbfEndFrame     = 6; // last bitmap image of a frame
constexpr uint8_t dbfLastFrame    = 7; // last bitmap image of the last frame

// drawBitmap modes with same behaviour as Arduboy library drawBitmap modes
constexpr uint8_t dbmBlack   = (1 << dbfReverseBlack) |   // white pixels in bitmap will be drawn as black pixels on display
                               (1 << dbfBlack) |          // black pixels in bitmap will not change pixels on display
                               (1 << dbfWhiteBlack);      // (same as sprites drawErase)

constexpr uint8_t dbmWhite   = (1 << dbfWhiteBlack);      // white pixels in bitmap will be drawn as white pixels on display
                                                        // black pixels in bitmap will not change pixels on display
                                                        //(same as sprites drawSelfMasked)

constexpr uint8_t dbmInvert  = (1 << dbfInvert);          // when a pixel in bitmap has a different color than on display the
                                                        // pixel on display will be drawn as white. In all other cases the
                                                        // pixel will be drawn as black

constexpr uint8_t dbmFlip  = (1 << dbfFlip);            // when a pixel in bitmap has a different color than on display the
                                                        // pixel on display will be drawn as white. In all other cases the
                                                        // pixel will be drawn as black
// additional drawBitmap modes
constexpr uint8_t dbmNormal     = 0;                    // White pixels in bitmap will be drawn as white pixels on display
constexpr uint8_t dbmOverwrite  = 0;                    // Black pixels in bitmap will be drawn as black pixels on display
                                                        // (Same as sprites drawOverwrite)

constexpr uint8_t dbmReverse = (1 << dbfReverseBlack);    // White pixels in bitmap will be drawn as black pixels on display
                                                        // Black pixels in bitmap will be drawn as white pixels on display

constexpr uint8_t dbmMasked  = (1 << dbfMasked);          // The bitmap contains a mask that will determine which pixels are
                                                        // drawn and which pixels remain unchanged on display
                                                        // (same as sprites drawPlusMask)
constexpr uint8_t dbmEndFrame = (1 << dbfEndFrame);       // last bitmap of a frame but more frames
constexpr uint8_t dbmLastFrame = (1 << dbfLastFrame);     // last bitmap of a frame and at end of frames

// Note above modes may be combined like (dbmMasked | dbmReverse)

// drawChar bit flags (used by modes below)
constexpr uint8_t dcfWhiteBlack   = 0; // character is used as mask
constexpr uint8_t dcfInvert       = 1; // character is exclusive or-ed with display
constexpr uint8_t dcfBlack        = 2; // character will be blackened
constexpr uint8_t dcfReverseBlack = 3; // reverses character data
constexpr uint8_t dcfMasked       = 4; // character contains mask data
constexpr uint8_t dcfProportional = 5; // use fonts width table to mimic proportional characters

//draw Font character modes
constexpr uint8_t dcmBlack   = (1 << dcfReverseBlack) |   // white pixels in character will be drawn as black pixels on display
                               (1 << dcfBlack) |          // black pixels in character will not change pixels on display
                               (1 << dcfWhiteBlack);      // (same as sprites drawErase)

constexpr uint8_t dcmWhite   = (1 << dcfWhiteBlack);      // white pixels in character will be drawn as white pixels on display
                                                        // black pixels in character will not change pixels on display
                                                        //(same as sprites drawSelfMasked)

constexpr uint8_t dcmInvert  = (1 << dcfInvert);          // when a pixel in character has a different color than on display the
                                                        // pixel on display will be drawn as white. In all other cases the
                                                        // pixel will be drawn as black
// additional drawcharacter modes
constexpr uint8_t dcmNormal     = 0;                    // White pixels in character will be drawn as white pixels on display
constexpr uint8_t dcmOverwrite  = 0;                    // Black pixels in character will be drawn as black pixels on display
                                                        // (Same as sprites drawOverwrite)

constexpr uint8_t dcmReverse = (1 << dcfReverseBlack);    // White pixels in character will be drawn as black pixels on display
                                                        // Black pixels in character will be drawn as white pixels on display

constexpr uint8_t dcmMasked  = (1 << dcfMasked);          // The character contains a mask that will determine which pixels are

constexpr uint8_t dcmProportional = (1 << dcfProportional); // draw characters with variable spacing. When this mode is used a
                                                          // 256 byte width table must precede the font data

// Note above modes may be combined like (dcmMasked | dcmProportional)


using uint24_t = __uint24;

struct JedecID
{
  uint8_t manufacturer;
  uint8_t device;
  uint8_t size;
};

struct FXAddress
{
  uint16_t page;
  uint8_t  offset;
};

struct Font
{
  uint24_t address;
  uint8_t  mode;
  uint8_t  width;
  uint8_t  height;
};

struct Cursor
{
  int16_t x;
  int16_t y;
  int16_t left;
  int16_t wrap;
};

struct FrameControl
{
  uint24_t start;
  uint24_t current;
  uint8_t repeat;
  uint8_t count;
};

struct FrameData
{
  int16_t  x;
  int16_t  y;
  uint24_t bmp;
  uint8_t  frame;
  uint8_t  mode;
};


class FX
{
  public:
    [[gnu::always_inline]]
    static inline void enableOLED() // selects OLED display.
    {
      CS_PORT &= ~(1 << CS_BIT);
    };

    [[gnu::always_inline]]
    static inline void disableOLED() // deselects OLED display.
    {
      CS_PORT |=  (1 << CS_BIT);
    };

    [[gnu::always_inline]]
    static inline void enable() // selects external flash memory and allows new commands
    {
      FX_PORT  &= ~(1 << FX_BIT);
    };

    [[gnu::always_inline]]
    static inline void disable() // deselects external flash memory and ends the last command
    {
      FX_PORT  |=  (1 << FX_BIT);
    };

    [[gnu::always_inline]]
    static inline void wait() // wait for a pending flash transfer to complete
    {
      while ((SPSR & (1 << SPIF)) == 0);
    }

    static uint8_t writeByte(uint8_t data); // write a single byte to flash memory.

    [[gnu::always_inline]]
    static inline void writeByteBeforeWait(uint8_t data)
    {
      SPDR = data;
      asm volatile("nop\n");
      wait();
    }

    [[gnu::always_inline]]
    static inline void writeByteAfterWait(uint8_t data)
    {
      wait();
      SPDR = data;
    }

    static uint8_t readByte(); // read a single byte from flash memory

    static void displayPrefetch(uint24_t address, uint8_t* target, uint16_t len, bool clear);

    static void display(); // display screen buffer

    static void display(bool clear); // display screen buffer with clear

    static void begin(); // Initializes flash memory. Use only when program does not require data and save areas in flash memory

    static void begin(uint16_t programDataPage); // Initializes flash memory. Use when program depends on data in flash memory

    static void begin(uint16_t datapage, uint16_t savepage); // Initializes flash memory. Use when program depends on both data and save data in flash memory

    /// @brief Reads the JedecID of the attached flash chip.
    /// @param id An object into which the ID will be read.
    static void readJedecID(JedecID & id);

    static void readJedecID(JedecID* id);

    static bool detect(); //detect presence of initialized flash memory

    static void noFXReboot(); // flash RGB LED red and wait for DOWN button to exit to bootloader when no initialized external flash memory is present

    static void writeCommand(uint8_t command); // write a single byte flash command

    static void wakeUp(); // Wake up flash memory from power down mode

    static void sleep(); // Put flash memory in power down mode for low power

    static void writeEnable();// Puts flash memory in write mode, required prior to any write command

    [[gnu::noinline, gnu::naked]]
    static void seekCommand(uint8_t command, uint24_t address);// Write command and selects flash memory address. Required by any read or write command

    /// @brief selects flash address of program data for reading and starts the first read
    /// @param address The base address of program data memory.

    [[gnu::noinline]]
    static void seekData(uint24_t address);

    /// @brief Seeks an element of an array.
    /// @tparam Type The type of the element to be read.
    /// @param address The base address of the array.
    /// @param index The index of the array element.
    template<typename Type>
    static void seekArrayElement(uint24_t address, uint8_t index)
    {
      // Note: By the laws of the language this should never happen.
      // This assert exists only as a precaution against e.g. weird compiler extensions.
      static_assert(sizeof(Type) > 0, "Cannot use a Type with a size of 0.");

      seekData(address + (index * sizeof(Type)));
    }

    /// @brief Seeks a member of an object that is an element of an array.
    /// @tparam Type The type of the elements in the array.
    /// @param address The base address of the array.
    /// @param index The index of the array element.
    /// @param offset The offset of the member of the array element.
    /// @note
    /// It is intended that the value of `offset` be acquired using the
    /// `offsetof` macro from `stddef.h`, as this is the safest way
    /// to obtain the offset of an object member.
    template<typename Type>
    static void seekArrayElementMember(uint24_t address, uint8_t index, size_t offset)
    {
      // Note: By the laws of the language this should never happen.
      // This assert exists only as a precaution against e.g. weird compiler extensions.
      static_assert(sizeof(Type) > 0, "Cannot use a Type with a size of 0.");

      seekData(address + ((index * sizeof(Type)) + offset));
    }

    [[gnu::noinline, gnu::naked]]
    static void seekDataArray(uint24_t address, uint8_t index, uint8_t offset, uint8_t elementSize);

    [[gnu::noinline]]
    static void seekSave(uint24_t address); // selects flashaddress of program save area for reading and starts the first read

    [[gnu::always_inline]]
    static inline uint8_t readUnsafe() // read flash data without performing any checks and starts the next read.
    {
      uint8_t result = SPDR;
      SPDR = 0;
      return result;
    };

    [[gnu::always_inline]]
    static inline uint8_t readUnsafeEnd()
    {
      uint8_t result = SPDR;
      disable();
      return result;
    };

    [[gnu::noinline]]
    static uint8_t readPendingUInt8();    //read a prefetched byte from the current flash location

    /// @brief read the last prefetched byte from the current flash location and ends the read command.
    /// This function performs the same action as readEnd()

    [[gnu::noinline]]
    static uint8_t readPendingLastUInt8();

    [[gnu::noinline, gnu::naked]]
    static uint16_t readPendingUInt16(); //read a partly prefetched 16-bit word from the current flash location

    [[gnu::noinline, gnu::naked]]
    static uint16_t readPendingLastUInt16(); //read a partly prefetched 16-bit word from the current flash location

    static uint24_t readPendingUInt24() ; //read a partly prefetched 24-bit word from the current flash location

    static uint24_t readPendingLastUInt24() ; //read a partly prefetched 24-bit word from the current flash location

    static uint32_t readPendingUInt32(); //read a partly prefetched a 32-bit word from the current flash location

    static uint32_t readPendingLastUInt32(); //read a partly prefetched a 32-bit word from the current flash location

    /// @brief Reads an object from the current flash location.
    /// @tparam Type The type of the object to be read.
    /// @param object An object into which the target object will be read.
    /// @warning
    /// `Type` should be:
    /// * _[trivially copyable](https://en.cppreference.com/w/cpp/named_req/TriviallyCopyable)_
    /// * a _[standard-layout](https://en.cppreference.com/w/cpp/language/data_members#Standard-layout)_ type
    /// Attempting to read an object that does not meet these restrictions will result in _undefined behaviour_.
    template<typename Type>
    static void readObject(Type & object)
    {
      readBytes(reinterpret_cast<uint8_t *>(&object), sizeof(object));
    }

    static void readBytes(uint8_t* buffer, size_t length);// read a number of bytes from the current flash location

    static void readBytesEnd(uint8_t* buffer, size_t length); // read a number of bytes from the current flash location and ends the read command

    /// @brief read the last prefetched byte from the current flash location and ends the read command

    [[gnu::noinline]]
    static uint8_t readEnd();

    /// @brief Reads an object from the specified address in the game's data section.
    /// @tparam Type The type of the object to be read.
    /// @param address The address of the object in flash memory.
    /// @param object An object into which the target object will be read.
    /// @warning
    /// `Type` should be:
    /// * _[trivially copyable](https://en.cppreference.com/w/cpp/named_req/TriviallyCopyable)_
    /// * a _[standard-layout](https://en.cppreference.com/w/cpp/language/data_members#Standard-layout)_ type
    /// Attempting to read an object that does not meet these restrictions will result in _undefined behaviour_.
    template<typename Type>
    static void readDataObject(uint24_t address, Type & object)
    {
      readDataBytes(address, reinterpret_cast<uint8_t *>(&object), sizeof(object));
    }

    static void readDataBytes(uint24_t address, uint8_t* buffer, size_t length);

    /// @brief Reads an object from the specified address in the game's save section.
    /// @tparam Type The type of the object to be read.
    /// @param address The address of the object in flash memory.
    /// @param object An object into which the target object will be read.
    /// @warning
    /// `Type` should be:
    /// * _[trivially copyable](https://en.cppreference.com/w/cpp/named_req/TriviallyCopyable)_
    /// * a _[standard-layout](https://en.cppreference.com/w/cpp/language/data_members#Standard-layout)_ type
    /// Attempting to read an object that does not meet these restrictions will result in _undefined behaviour_.
    template<typename Type>
    static void readSaveObject(uint24_t address, Type & object)
    {
      readSaveBytes(address, reinterpret_cast<uint8_t *>(&object), sizeof(object));
    }

    static void readSaveBytes(uint24_t address, uint8_t* buffer, size_t length);

    /// @brief Loads a saved game state object from an exclusive 4KB save data block.
    /// @tparam Type The type of the object to be loaded.
    /// @param object The object into which the saved state will be loaded.
    /// @warning
    /// `Type` should be:
    /// * _[trivially copyable](https://en.cppreference.com/w/cpp/named_req/TriviallyCopyable)_
    /// * a _[standard-layout](https://en.cppreference.com/w/cpp/language/data_members#Standard-layout)_ type
    /// Attempting to read an object that does not meet these restrictions will result in _undefined behaviour_.
    template<typename Type>
    static uint8_t loadGameState(Type & object)
    {
      return loadGameState((uint8_t*)(&object), sizeof(object));
    }

    [[gnu::noinline]]
    static uint8_t loadGameState(uint8_t* gameState, size_t size); //loads GameState from program exclusive 4K save data block.

    /// @brief Saves a game state object into an exclusive 4KB save data block.
    /// @tparam Type The type of the object to be saved.
    /// @param object The object to be saved.
    /// @warning
    /// `Type` should be:
    /// * _[trivially copyable](https://en.cppreference.com/w/cpp/named_req/TriviallyCopyable)_
    /// * a _[standard-layout](https://en.cppreference.com/w/cpp/language/data_members#Standard-layout)_ type
    /// Attempting to read an object that does not meet these restrictions will result in _undefined behaviour_.
    template<typename Type>
    static void saveGameState(const Type & object)
    {
      saveGameState(reinterpret_cast<const uint8_t *>(&object), sizeof(object));
    }

    [[gnu::noinline]]
    static void saveGameState(const uint8_t* gameState, size_t size); // Saves GameState in RAM to programes exclusive 4K save data block.

    static void eraseSaveBlock(uint16_t page); // erases 4K flash block

    static void writeSavePage(uint16_t page, uint8_t* buffer);

    static void waitWhileBusy(); // wait for outstanding erase or write to finish

    [[gnu::noinline]]
    static void drawBitmap(int16_t x, int16_t y, uint24_t address, uint8_t frame, uint8_t mode);

    [[gnu::noinline]]
    static void setFrame(uint24_t frame, uint8_t repeat);

    static uint8_t drawFrame();

    [[gnu::noinline]]
    static uint24_t drawFrame(uint24_t address); // draw a list of bitmap images located at address

    static void readDataArray(uint24_t address, uint8_t index, uint8_t offset, uint8_t elementSize, uint8_t* buffer, size_t length);

    static uint8_t readIndexedUInt8(uint24_t address, uint8_t index);

    static uint16_t readIndexedUInt16(uint24_t address, uint8_t index);

    static uint24_t readIndexedUInt24(uint24_t address, uint8_t index);

    static uint32_t readIndexedUInt32(uint24_t address, uint8_t index);

    /* Draw character functions */

    static void setFont(uint24_t address, uint8_t mode);

    static void setFontMode(uint8_t mode);

    static void setCursor(int16_t x, int16_t y);

    static void setCursorX(int16_t x);

    static void setCursorY(int16_t y);

    static void setCursorRange(int16_t left, int16_t wrap);

    static void setCursorLeft(int16_t x);

    static void setCursorWrap(int16_t y);

    static void drawChar(uint8_t c);

    static void drawString(const uint8_t* buffer);

    static void drawString(const char* str);

    static void drawString(uint24_t address);

    //static void drawNumber(int8_t n, int8_t digits = 0); // draw a signed 8-bit number
    //
    //static void drawNumber(unt8_t n, int8_t digits = 0); // draw a unsigned 8-bit number
    //
    static void drawNumber(int16_t n, int8_t digits = 0); // draw a signed 16-bit number

    static void drawNumber(uint16_t n, int8_t digits = 0); // draw a unsigned 16-bit number

    static void drawNumber(int32_t n, int8_t digits = 0);  // draw signed 32-bit number with a fixed number of digits
                                                           // digits == 0: No leading characters are added, all digits are drawn
                                                           // digits >  0: Leading zeros are added if the number has less than 'digit' digits
                                                           // digits <  0: Leading spaces are added if the number has less than 'digit' digits
                                                           // Note: Only 'digits' number of digits are drawn regardless if the number has more digits
                                                           //       The sign character is not counted as a digit.

    static void drawNumber(uint32_t n, int8_t digits = 0); // draw unsigned 32-bit number with a fixed number of digits
                                                           // digits == 0: No leading characters are added, all digits are drawn
                                                           // digits >  0: Leading zeros are added if the number has less than 'digit' digits
                                                           // digits <  0: Leading spaces are added if the number has less than 'digit' digits
                                                           // Note: Only 'digits' number of digits are drawn regardless if the number has more digits

    /* general optimized functions */

    [[gnu::always_inline]]
    static inline uint16_t multiplyUInt8 (uint8_t a, uint8_t b)
    {
     #ifdef ARDUINO_ARCH_AVR
      uint16_t result;
      asm volatile(
        "mul    %[a], %[b]      \n"
        "movw   %A[result], r0  \n"
        "clr    r1              \n"
        : [result] "=&r" (result)
        : [a]      "r"   (a),
          [b]      "r"   (b)
        :
      );
      return result;
     #else
      return (a * b);
     #endif
    }

    [[gnu::always_inline]]
    static inline uint8_t bitShiftLeftUInt8(uint8_t bit) //fast (1 << (bit & 7))
    {
     #ifdef ARDUINO_ARCH_AVR
      uint8_t result;
      asm volatile(
        "ldi    %[result], 1    \n" // 0 = 000 => 0000 0001
        "sbrc   %[bit], 1       \n" // 1 = 001 => 0000 0010
        "ldi    %[result], 4    \n" // 2 = 010 => 0000 0100
        "sbrc   %[bit], 0       \n" // 3 = 011 => 0000 1000
        "lsl    %[result]       \n"
        "sbrc   %[bit], 2       \n" // 4 = 100 => 0001 0000
        "swap   %[result]       \n" // 5 = 101 => 0010 0000
        :[result] "=&d" (result)    // 6 = 110 => 0100 0000
        :[bit]    "r"   (bit)       // 7 = 111 => 1000 0000
        :
      );
      return result;
     #else
      return 1 << (bit & 7);
     #endif
    }

    [[gnu::always_inline]]
    static inline uint8_t bitShiftRightUInt8(uint8_t bit) //fast (0x80 >> (bit & 7))
    {
     #ifdef ARDUINO_ARCH_AVR
      uint8_t result;
      asm volatile(
        "ldi    %[result], 1    \n" // 0 = 000 => 1000 0000
        "sbrs   %[bit], 1       \n" // 1 = 001 => 0100 0000
        "ldi    %[result], 4    \n" // 2 = 010 => 0010 0000
        "sbrs   %[bit], 0       \n" // 3 = 011 => 0001 0000
        "lsl    %[result]       \n"
        "sbrs   %[bit], 2       \n" // 4 = 100 => 0000 1000
        "swap   %[result]       \n" // 5 = 101 => 0000 0100
        :[result] "=&d" (result)    // 6 = 110 => 0000 0010
        :[bit]    "r"   (bit)       // 7 = 111 => 0000 0001
        :
      );
      return result;
     #else
      return 0x80 >> (bit & 7);
     #endif
    }

    [[gnu::always_inline]]
    static inline uint8_t bitShiftLeftMaskUInt8(uint8_t bit) //fast (0xFF << (bit & 7) & 0xFF)
    {
     #ifdef ARDUINO_ARCH_AVR
      uint8_t result;
      asm volatile(
        "ldi    %[result], 1    \n" // 0 = 000 => 1111 1111 = -1
        "sbrc   %[bit], 1       \n" // 1 = 001 => 1111 1110 = -2
        "ldi    %[result], 4    \n" // 2 = 010 => 1111 1100 = -4
        "sbrc   %[bit], 0       \n" // 3 = 011 => 1111 1000 = -8
        "lsl    %[result]       \n"
        "sbrc   %[bit], 2       \n" // 4 = 100 => 1111 0000 = -16
        "swap   %[result]       \n" // 5 = 101 => 1110 0000 = -32
        "neg    %[result]       \n" // 6 = 110 => 1100 0000 = -64
        :[result] "=&d" (result)    // 7 = 111 => 1000 0000 = -128
        :[bit]    "r"   (bit)
        :
      );
      return result;
     #else
      return (0xFF << (bit & 7)) & 0xFF;
     #endif
    }

    [[gnu::always_inline]]
    static inline uint8_t bitShiftRightMaskUInt8(uint8_t bit) //fast (0xFF >> (bit & 7))
    {
     #ifdef ARDUINO_ARCH_AVR
      uint8_t result;
      asm volatile(
        "ldi    %[result], 2    \n" // 0 = 000 => 1111 1111 = 0x00 - 1
        "sbrs   %A[bit], 1      \n" // 1 = 001 => 0111 1111 = 0x80 - 1
        "ldi    %[result], 8    \n" // 2 = 010 => 0011 1111 = 0x40 - 1
        "sbrs   %A[bit], 2      \n" // 3 = 011 => 0001 1111 = 0x20 - 1
        "swap   %[result]       \n"
        "sbrs   %A[bit], 0      \n" // 4 = 100 => 0000 1111 = 0x10 - 1
        "lsl    %[result]       \n" // 5 = 101 => 0000 0111 = 0x08 - 1
        "dec    %[result]       \n" // 6 = 110 => 0000 0011 = 0x04 - 1
        :[result] "=&d" (result)    // 7 = 111 => 0000 0001 = 0x02 - 1
        :[bit]    "r"   (bit)
        :
      );
      return result;
     #else
      return 0xFF >> (bit & 7);
     #endif
    }

    [[gnu::always_inline]]
    static inline int16_t fastDiv8(int16_t i)
    {
     #ifdef ARDUINO_ARCH_AVR
      asm volatile(
        "asr  %B[i] \n"
        "ror  %A[i] \n"
        "asr  %B[i] \n"
        "ror  %A[i] \n"
        "asr  %B[i] \n"
        "ror  %A[i] \n"
      : [i] "+r" (i)
      :     "0" (i)
      );
      return i;
     #else
      return i >> 3;
     #endif
    };

    [[gnu::always_inline]]
    static inline uint16_t fastDiv8(uint16_t i)
    {
     #ifdef ARDUINO_ARCH_AVR
      asm volatile(
        "lsr  %B[i] \n"
        "ror  %A[i] \n"
        "lsr  %B[i] \n"
        "ror  %A[i] \n"
        "lsr  %B[i] \n"
        "ror  %A[i] \n"
      : [i] "+r" (i)
      :     "0"  (i)
      );
      return i;
     #else
      return i >> 3;
     #endif
    };

    static uint16_t programDataPage; // program read only data area in flash memory
    static uint16_t programSavePage; // program read and write data area in flash memory
    static Font     font;
    static Cursor   cursor;

    static FrameControl frameControl;
};
#endif
