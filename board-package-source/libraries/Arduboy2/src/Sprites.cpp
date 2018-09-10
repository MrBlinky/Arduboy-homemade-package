/**
 * @file Sprites.cpp
 * \brief
 * A class for drawing animated sprites from image and mask bitmaps.
 */

#include "Sprites.h"

void Sprites::drawExternalMask(int16_t x, int16_t y, const uint8_t *bitmap,
                               const uint8_t *mask, uint8_t frame, uint8_t mask_frame)
{
  draw(x, y, bitmap, frame, mask, mask_frame, SPRITE_MASKED);
}

void Sprites::drawOverwrite(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
  draw(x, y, bitmap, frame, NULL, 0, SPRITE_OVERWRITE);
}

void Sprites::drawErase(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
  draw(x, y, bitmap, frame, NULL, 0, SPRITE_IS_MASK_ERASE);
}

void Sprites::drawSelfMasked(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
  draw(x, y, bitmap, frame, NULL, 0, SPRITE_IS_MASK);
}

void Sprites::drawPlusMask(int16_t x, int16_t y, const uint8_t *bitmap, uint8_t frame)
{
  draw(x, y, bitmap, frame, NULL, 0, SPRITE_PLUS_MASK);
}


//common functions
void Sprites::draw(int16_t x, int16_t y,
                   const uint8_t *bitmap, uint8_t frame,
                   const uint8_t *mask, uint8_t sprite_frame,
                   uint8_t drawMode)
{
  unsigned int frame_offset;

  if (bitmap == NULL)
    return;

//  uint8_t width = pgm_read_byte(bitmap);
//  uint8_t height = pgm_read_byte(++bitmap);
//  bitmap++;
//  if (frame > 0 || sprite_frame > 0) {
//    frame_offset = (width * ( height / 8 + ( height % 8 == 0 ? 0 : 1)));
//    // sprite plus mask uses twice as much space for each frame
//    if (drawMode == SPRITE_PLUS_MASK) {
//      frame_offset *= 2;
//    } else if (mask != NULL) {
//      mask += sprite_frame * frame_offset;
//    }
//    bitmap += frame * frame_offset;
//  }
//  // if we're detecting the draw mode then base it on whether a mask
//  // was passed as a separate object
//  if (drawMode == SPRITE_AUTO_MODE) {
//    drawMode = mask == NULL ? SPRITE_UNMASKED : SPRITE_MASKED;
//  }
  // assembly optimisation of above code saving 20(+) bytes
  uint8_t width;
  uint8_t height;
  asm volatile(
    "    lpm   %[width], Z+                 \n\t" // width  = pgm_read_byte(bitmap++);
    "    lpm   %[height], Z+                \n\t" // height = pgm_read_byte(bitmap++);
    "    cp    %[frame], __zero_reg__       \n\t" // if (frame > 0 || sprite_frame > 0)
    "    brne  1f                           \n\t" 
    "    cp    %[spr_frame], __zero_reg__   \n\t" 
    "    breq  3f                           \n\t" 
    "1:                                     \n\t" 
    "    ldi   r20, 7                       \n\t" //rows = ((height+7)
    "    add   r20, %[height]               \n\t" 
    "    ror   r20                          \n\t" //include carry for heights > 248
    "    lsr   r20                          \n\t" //rows = (height+7) / 8
    "    lsr   r20                          \n\t" 
    "    cpi   %[mode], %[sprite_plus_mask] \n\t" //if (drawMode == SPRITE_PLUS_MASK) rows *= 2
    "    brne  2f                           \n\t"
    "    lsl   r20                          \n\t" 
    "2:                                     \n\t" 
    "    mul   r20, %[width]                \n\t" //frame offset = rows * width
    "    movw  r20, r0                      \n\t" 
    "    mul   %[frame] , r20               \n\t" 
    "    add   %A[bitmap], r0               \n\t" //bitmap += frame * (frame offset & 0xFF)
    "    adc   %B[bitmap], r1               \n\t" 
    "    mul   %[frame] , r21               \n\t" //bitmap += frame * (frame_offset >> 8 )) << 8
    "    add   %B[bitmap], r0               \n\t" 
    "                                       \n\t" 
    "    adiw %A[mask], 0                   \n\t" //if (mask != NULL)
    "    breq  3f                           \n\t" 
    "                                       \n\t" 
    "    mul   %[spr_frame] , r20           \n\t"  
    "    add   %A[mask], r0                 \n\t" //mask += sprite_frame * (frame offset & 0xFF)
    "    adc   %B[mask], r1                 \n\t" 
    "    mul   %[spr_frame] , r21           \n\t" //mask += (sprite_frame * (frame_offset >> 8 )) << 8
    "    add   %B[mask], r0                 \n\t" 
    "3:                                     \n\t" 
    "    clr   __zero_reg__                 \n\t" 
    "4:                                     \n\t" 
    "    cpi   %[mode], %[sprite_auto_mode] \n\t" //if (drawMode == SPRITE_AUTO_MODE)
    "    brne  5f                           \n\t" 
    "    adiw  %A[mask], 0                  \n\t" //if (mask = NULL) drawMode = SPRITE_UNMASKED
    "    ldi   %[mode], %[sprite_unmasked]  \n\t" 
    "    breq  5f                           \n\t" 
    "    ldi   %[mode], %[sprite_masked]    \n\t" //else drawMode = SPRITE_PLUS_MASK
    "5:                                     \n\t" 
    : [width]  "=&r" (width),
      [height] "=&r" (height),
      [mask]   "+x"  (mask),
      [bitmap] "+z"  (bitmap),
      [mode]   "+d"  (drawMode)
    : [frame]            "r" (frame),
      [spr_frame]        "r" (sprite_frame),
      [sprite_plus_mask] "M" (SPRITE_PLUS_MASK),
      [sprite_auto_mode] "M" (SPRITE_AUTO_MODE),
      [sprite_unmasked]  "M" (SPRITE_UNMASKED),
      [sprite_masked]    "M" (SPRITE_MASKED)
    : "r20", "r21"
  );
  drawBitmap(x, y, bitmap, mask, width, height, drawMode);
}

void Sprites::drawBitmap(int16_t x, int16_t y,
                         const uint8_t *bitmap, const uint8_t *mask,
                         uint8_t w, uint8_t h, uint8_t draw_mode)
{
  // no need to draw at all if we're offscreen
  if (x + w <= 0 || x > WIDTH - 1 || y + h <= 0 || y > HEIGHT - 1)
    return;

  if (bitmap == NULL)
    return;

  // xOffset technically doesn't need to be 16 bit but the math operations
  // are measurably faster if it is
  uint16_t xOffset, ofs;
  int8_t yOffset = y & 7;
  int8_t sRow = y / 8;
  uint8_t loop_h, start_h, rendered_width;

  if (y < 0 && yOffset > 0) {
    sRow--;
  }

  // if the left side of the render is offscreen skip those loops
  if (x < 0) {
    xOffset = abs(x);
  } else {
    xOffset = 0;
  }

  // if the right side of the render is offscreen skip those loops
  if (x + w > WIDTH - 1) {
    rendered_width = ((WIDTH - x) - xOffset);
  } else {
    rendered_width = (w - xOffset);
  }

  // if the top side of the render is offscreen skip those loops
  if (sRow < -1) {
    start_h = abs(sRow) - 1;
  } else {
    start_h = 0;
  }

  loop_h = h / 8 + (h % 8 > 0 ? 1 : 0); // divide, then round up

  // if (sRow + loop_h - 1 > (HEIGHT/8)-1)
  if (sRow + loop_h > (HEIGHT / 8)) {
    loop_h = (HEIGHT / 8) - sRow;
  }

  // prepare variables for loops later so we can compare with 0
  // instead of comparing two variables
  loop_h -= start_h;

  sRow += start_h;
  ofs = (sRow * WIDTH) + x + xOffset;

  uint8_t mul_amt = 1 << yOffset;
  uint16_t mask_data;
  uint16_t bitmap_data;

  const uint8_t ofs_step = draw_mode == SPRITE_PLUS_MASK ? 2 : 1;
  const uint8_t ofs_stride = (w - rendered_width)*ofs_step;
  const uint16_t initial_bofs = ((start_h * w) + xOffset)*ofs_step;

  const uint8_t *bofs = bitmap + initial_bofs;
  const uint8_t *mask_ofs = !mask ? bitmap : mask;
  mask_ofs += initial_bofs + ofs_step - 1;

  for (uint8_t a = 0; a < loop_h; a++) {
    for (uint8_t iCol = 0; iCol < rendered_width; iCol++) {
      uint8_t data;

      bitmap_data = pgm_read_byte(bofs) * mul_amt;
      mask_data = ~bitmap_data;

      if (draw_mode == SPRITE_UNMASKED) {
        mask_data = ~(0xFF * mul_amt);
      } else if (draw_mode == SPRITE_IS_MASK_ERASE) {
        bitmap_data = 0;
      } else {
        mask_data = ~(pgm_read_byte(mask_ofs) * mul_amt);
      }

      if (sRow >= 0) {
        data = Arduboy2Base::sBuffer[ofs];
        data &= (uint8_t)(mask_data);
        data |= (uint8_t)(bitmap_data);
        Arduboy2Base::sBuffer[ofs] = data;
      }
      if (yOffset != 0 && sRow < (HEIGHT / 8 - 1)) {
        data = Arduboy2Base::sBuffer[ofs + WIDTH];
        data &= (*((unsigned char *) (&mask_data) + 1));
        data |= (*((unsigned char *) (&bitmap_data) + 1));
        Arduboy2Base::sBuffer[ofs + WIDTH] = data;
      }
      ofs++;
      mask_ofs += ofs_step;
      bofs += ofs_step;
    }
    sRow++;
    bofs += ofs_stride;
    mask_ofs += ofs_stride;
    ofs += WIDTH - rendered_width;
  }
}
