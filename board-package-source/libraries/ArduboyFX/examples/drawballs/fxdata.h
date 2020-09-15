#ifndef FXDATA_H
#define FXDATA_H

using uint24_t = __uint24;

constexpr uint16_t FX_DATA_PAGE = 0xFFFE;       //value given by flashcart-writer.py script using -d option
constexpr uint24_t FX_DATA_TILES   = 0x000000;  // Background tiles offset in external flash
constexpr uint24_t FX_DATA_TILEMAP = 0x000044;  // 16 x 16 tilemap offset in external flash
constexpr uint24_t FX_DATA_BALLS   = 0x000144;  // masked ball sprite offset in external flash

#endif
