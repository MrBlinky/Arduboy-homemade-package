#include "ATMlib.h"

uint16_t __attribute__((used)) cia, __attribute__((used)) cia_count;
#ifdef __AVR_ARCH__
ISR(TIMER4_OVF_vect, ISR_NAKED) {
  asm volatile(
    "push r18                                           \n"
    "lds  r18,                   half                   \n" // half = !half;
    "sbrc r18,                   0                      \n" // if (half) return;
    "rjmp 2f                                            \n"
    "ldi  r18,                   0xFF                   \n"
    "1:                                                 \n"
    "sts  half,                  r18                    \n"
    "pop  r18                                           \n"
    "reti                                               \n"
    "2:                                                 \n"
    "push r0                                            \n"
    "push r1                                            \n"
    "push r2                                            \n"
    "in   r2,                    __SREG__               \n"
    "push r30                                           \n"
    "push r31                                           \n"
    "ldi  r30,                   lo8(osc)               \n"
    "ldi  r31,                   hi8(osc)               \n"

    "ldi  r18,                   1                      \n"
    "ldd  r0,                    Z+3*%[mul]+%[fre]      \n" // uint16_t freq = osc[3].freq; //noise frequency
    "ldd  r1,                    Z+3*%[mul]+%[fre]+1    \n"
    "add  r0,                    r0                     \n" // freq <<= 1;
    "adc  r1,                    r1                     \n"
    "sbrc r1,                    7                      \n" // if (freq & 0x8000) freq ^= 1;
    "eor  r0,                    r18                    \n"
    "sbrc r1,                    6                      \n" // if (freq & 0x4000) freq ^= 1;
    "eor  r0,                    r18                    \n"
    "std  Z+3*%[mul]+%[fre],     r0                     \n" // osc[3].freq = freq;
    "std  Z+3*%[mul]+%[fre]+1,   r1                     \n"

    "ldd  r18,                   Z+3*%[mul]+%[vol]      \n" // int8_t vol = osc[3].vol;
    "sbrc r1,                    7                      \n" // if (freq & 0x8000) vol = -vol;
    "neg  r18                                           \n"
    "mov  r1,                    r18                    \n"

    "ldd  r0,                    Z+0*%[mul]+%[fre]      \n" // osc[0].phase += osc[0].freq; // update pulse phase
    "ldd  r18,                   Z+0*%[mul]+%[pha]      \n"
    "add  r18,                   r0                     \n"
    "std  Z+0*%[mul]+%[pha],     r18                    \n"
    "ldd  r0,                    Z+0*%[mul]+%[fre]+1    \n"
    "ldd  r18,                   Z+0*%[mul]+%[pha]+1    \n"
    "adc  r18,                   r0                     \n"
    "std  Z+0*%[mul]+%[pha]+1,   r18                    \n"

    "ldd  r0,                    Z+0*%[mul]+%[vol]      \n" // int8_t vol0 = osc[0].vol;
    "cpi  r18,                   192                    \n" // if (uint8_t(osc[0].phase >> 8) >= 192) vol0 = -vol0;
    "brcs 3f                                            \n"
    "neg  r0                                            \n"
    "add  r1,                    r0                     \n" // int8_t vol += vol0;
    "3:                                                 \n"

    "ldd  r18,                   Z+1*%[mul]+%[fre]      \n" // osc[1].phase += osc[1].freq; // update square phase
    "ldd  r0,                    Z+1*%[mul]+%[pha]      \n"
    "add  r0,                    r18                    \n"
    "std  Z+1*%[mul]+%[pha],     r0                     \n"
    "ldd  r18,                   Z+1*%[mul]+%[fre]+1    \n"
    "ldd  r0,                    Z+1*%[mul]+%[pha]+1    \n"
    "adc  r0,                    r18                    \n"
    "std  Z+1*%[mul]+%[pha]+1,   r0                     \n"

    "ldd  r18,                   Z+1*%[mul]+%[vol]      \n" // int8_t vol1 = osc[1].vol;
    "sbrc r0,                    7                      \n" // if (osc[1].phase & 0x8000) vol1 = -vol1;
    "neg  r18                                           \n"
    "add  r1,                    r18                    \n" // vol += vol1;

    "ldd  r18,                   Z+2*%[mul]+%[fre]      \n" // osc[2].phase += osc[2].freq;// update triangle phase
    "ldd  r0,                    Z+2*%[mul]+%[pha]      \n"
    "add  r0,                    r18                    \n"
    "std  Z+2*%[mul]+%[pha],     r0                     \n"
    "ldd  r0,                    Z+2*%[mul]+%[fre]+1    \n"
    "ldd  r18,                   Z+2*%[mul]+%[pha]+1    \n"
    "adc  r18,                   r0                     \n"
    "std  Z+2*%[mul]+%[pha]+1,   r18                    \n"
                                                            // int8_t phase2 = osc[2].phase >> 8;
    "ldd  r30,                   Z+2*%[mul]+%[vol]      \n" // int8_t vol2 = osc[2].vol;
    "lds  r31,                   pcm                    \n" // int8_t tmp = pcm + vol;
    "add  r31,                   r1                     \n"
    "sbrc r18,                   7                      \n" // if (phase2 < 0) phase2 = ~phase2;
    "com  r18                                           \n"
    "lsl  r18                                           \n" // phase2 <<= 1;
    "subi r18,                   128                    \n" // phase2 -= 128;
    "muls r18,                   r30                    \n" // vol = ((phase2 * vol2) << 1) >> 8 + tmp;
    "lsl  r1                                            \n"
    "add  r1,                    r31                    \n"

    "sts  %[reg],                r1                     \n" // reg = vol;
  #ifdef AB_ALTERNATE_WIRING
    "sts  %[reg2],               r1                     \n" // reg2 = vol;
  #endif
    "lds  r31,                   cia_count+1            \n" // if (--cia_count) return;
    "lds  r30,                   cia_count              \n"
    "sbiw r30,                   1                      \n"
    "brne 4f                                            \n"
    "lds  r31, cia+1                                    \n" // cia_count = cia;
    "lds  r30, cia                                      \n"
    "4:                                                 \n"
    "sts  cia_count+1,           r31                    \n"
    "sts  cia_count,             r30                    \n"
    "brne 5f                                            \n"

    "sei                                                \n" // sei();
    "push r19                                           \n"
    "push r20                                           \n"
    "push r21                                           \n"
    "push r22                                           \n"
    "push r23                                           \n"
    "push r24                                           \n"
    "push r25                                           \n"
    "push r26                                           \n"
    "push r27                                           \n"

    "clr  r1                                            \n"
    "call ATM_playroutine                               \n" // ATM_playroutine();

    "pop  r27                                           \n" // }
    "pop  r26                                           \n"
    "pop  r25                                           \n"
    "pop  r24                                           \n"
    "pop  r23                                           \n"
    "pop  r22                                           \n"
    "pop  r21                                           \n"
    "pop  r20                                           \n"
    "pop  r19                                           \n"
    "5:                                                 \n"
    "pop  r31                                           \n"
    "pop  r30                                           \n"
    "out  __SREG__,              r2                     \n"
    "pop  r2                                            \n"
    "pop  r1                                            \n"
    "pop  r0                                            \n"
    "ldi  r18,                   0x00                   \n"
    "rjmp 1b                                            \n"
    :
    : [reg]  "M" _SFR_MEM_ADDR(OCR4A),
  #ifdef AB_ALTERNATE_WIRING
    [reg2]  "M" _SFR_MEM_ADDR(OCR4D),
  #endif
    [mul]  "M" (sizeof(Oscillator)),
    [pha]  "M" (offsetof(Oscillator, phase)),
    [fre]  "M" (offsetof(Oscillator, freq)),
    [vol]  "M" (offsetof(Oscillator, vol))
  );
}
#else
ISR(TIMER4_OVF_vect)
{
  half = !half;
  if (half) return;

  osc[2].phase += osc[2].freq;       // update triangle phase
  int8_t phase2 = osc[2].phase >> 8;
  if (phase2 < 0) phase2 = ~phase2;
  phase2 <<= 1;
  phase2 -= 128;
  int8_t vol = ((phase2 * int8_t(osc[2].vol)) << 1) >> 8;

  osc[0].phase += osc[0].freq; // update pulse phase
  int8_t vol0 = osc[0].vol;
  if (osc[0].phase >= 0xC000) vol0 = -vol0;
  vol += vol0;

  osc[1].phase += osc[1].freq; // update square phase
  int8_t vol1 = osc[1].vol;
  if (osc[1].phase & 0x8000) vol1 = -vol1;
  vol += vol1;

  uint16_t freq = osc[3].freq; //noise frequency
  freq <<= 1;
  if (freq & 0x8000) freq ^= 1;
  if (freq & 0x4000) freq ^= 1;
  osc[3].freq = freq;
  int8_t vol3 = osc[3].vol;
  if (freq & 0x8000) vol3 = -vol3;
  vol += vol3;

  OCR4A = vol + pcm;
  if (--cia_count) return;

  cia_count = cia;
  sei();
  ATM_playroutine();
}
#endif

byte trackCount;
byte tickRate;
const word *trackList;
const byte *trackBase;
uint8_t pcm __attribute__((used)) = 128;
bool half __attribute__((used));

byte ChannelActiveMute = 0b11110000;
//                         ||||||||
//                         |||||||└->  0  channel 0 is muted (0 = false / 1 = true)
//                         ||||||└-->  1  channel 1 is muted (0 = false / 1 = true)
//                         |||||└--->  2  channel 2 is muted (0 = false / 1 = true)
//                         ||||└---->  3  channel 3 is muted (0 = false / 1 = true)
//                         |||└----->  4  channel 0 is Active (0 = false / 1 = true)
//                         ||└------>  5  channel 1 is Active (0 = false / 1 = true)
//                         |└------->  6  channel 2 is Active (0 = false / 1 = true)
//                         └-------->  7  channel 3 is Active (0 = false / 1 = true)

//Imports
extern uint16_t cia;

// Exports
osc_t __attribute__((used)) osc[4];


const word noteTable[64] PROGMEM = {
  0,
  262,  277,  294,  311,  330,  349,  370,  392,  415,  440,  466,  494,
  523,  554,  587,  622,  659,  698,  740,  784,  831,  880,  932,  988,
  1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976,
  2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,
  4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902,
  8372, 8870, 9397,
};


struct ch_t {
  const byte *ptr;
  byte note;

  // Nesting
  word stackPointer[7];
  byte stackCounter[7];
  byte stackTrack[7]; // note 1
  byte stackIndex;
  byte repeatPoint;

  // Looping
  word delay;
  byte counter;
  byte track;

  // External FX
  word freq;
  byte vol;

  // Volume & Frequency slide FX
  char volFreSlide;
  byte volFreConfig;
  byte volFreCount;

  // Arpeggio or Note Cut FX
  byte arpNotes;       // notes: base, base+[7:4], base+[7:4]+[3:0], if FF => note cut ON
  byte arpTiming;      // [7] = reserved, [6] = not third note ,[5] = retrigger, [4:0] = tick count
  byte arpCount;

  // Retrig FX
  byte reConfig;       // [7:2] = , [1:0] = speed // used for the noise channel
  byte reCount;        // also using this as a buffer for volume retrig on all channels

  // Transposition FX
  char transConfig;

  // Tremolo or Vibrato FX
  byte treviDepth;
  byte treviConfig;
  byte treviCount;

  // Glissando FX
  char glisConfig;
  byte glisCount;

};

ch_t channel[4];

uint16_t read_vle(const byte **pp) {
  word q = 0;
  byte d;
  do {
    q <<= 7;
    d = pgm_read_byte(*pp++);
    q |= (d & 0x7F);
  } while (d & 0x80);
  return q;
}

static inline const byte *getTrackPointer(byte track) {
  return trackBase + pgm_read_word(&trackList[track]);
}


void ATMsynth::play(const byte *song) {
  stop();
  cia_count = 1;

  // Initializes ATMsynth
  // Sets sample rate and tick rate
  tickRate = 25;
  cia = 15625 / tickRate;
  // Sets up the ports, and the sample grinding ISR

  osc[3].freq = 0x0001; // Seed LFSR
  channel[3].freq = 0x0001; // xFX

  TCCR4A = 0b01000010;    // Fast-PWM 8-bit
  TCCR4B = 0b00000001;    // 62500Hz
  OCR4C  = 0xFF;          // Resolution to 8-bit (TOP=0xFF)
  OCR4A  = 0x80;
#ifdef AB_ALTERNATE_WIRING
  TCCR4C = 0b01000101;
  OCR4D  = 0x80;
#endif

  // Load a melody stream and start grinding samples
  // Read track count
  trackCount = pgm_read_byte(song++);
  // Store track list pointer
  trackList = (word*)song;
  // Store track pointer
  trackBase = (song += (trackCount << 1)) + 4;
  // Fetch starting points for each track
  for (byte n = 0; n < 4; n++) {
    channel[n].ptr = getTrackPointer(pgm_read_byte(song++));
  }
  TIMSK4 = 0b00000100;// enable interrupt as last
}

// Stop playing, unload melody
void ATMsynth::stop() {
  TIMSK4 = 0; // Disable interrupt
  memset(channel, 0, sizeof(channel));
  ChannelActiveMute = 0b11110000;
}

// Start grinding samples or Pause playback
void ATMsynth::playPause() {
  TIMSK4 = TIMSK4 ^ 0b00000100; // toggle disable/enable interrupt
}

// Toggle mute on/off on a channel, so it can be used for sound effects
// So you have to call it before and after the sound effect
void ATMsynth::muteChannel(byte ch) {
  ChannelActiveMute += (1 << ch );
}

void ATMsynth::unMuteChannel(byte ch) {
  ChannelActiveMute &= (~(1 << ch ));
}


__attribute__((used))
void ATM_playroutine() {
  ch_t *ch;

  // for every channel start working
  for (byte n = 0; n < 4; n++)
  {
    ch = &channel[n];

    // Noise retriggering
    if (ch->reConfig) {
      if (ch->reCount >= (ch->reConfig & 0x03)) {
        osc[n].freq = pgm_read_word(&noteTable[ch->reConfig >> 2]);
        ch->reCount = 0;
      }
      else ch->reCount++;
    }


    //Apply Glissando
    if (ch->glisConfig) {
      if (ch->glisCount >= (ch->glisConfig & 0x7F)) {
        if (ch->glisConfig & 0x80) ch->note -= 1;
        else ch->note += 1;
        if (ch->note < 1) ch->note = 1;
        else if (ch->note > 63) ch->note = 63;
        ch->freq = pgm_read_word(&noteTable[ch->note]);
        ch->glisCount = 0;
      }
      else ch->glisCount++;
    }


    // Apply volume/frequency slides
    if (ch->volFreSlide) {
      if (!ch->volFreCount) {
        int16_t vf = ((ch->volFreConfig & 0x40) ? ch->freq : ch->vol);
        vf += (ch->volFreSlide);
        if (!(ch->volFreConfig & 0x80)) {
          if (vf < 0) vf = 0;
          else if (ch->volFreConfig & 0x40) if (vf > 9397) vf = 9397;
            else if (!(ch->volFreConfig & 0x40)) if (vf > 63) vf = 63;
        }
        (ch->volFreConfig & 0x40) ? ch->freq = vf : ch->vol = vf;
      }
      if (ch->volFreCount++ >= (ch->volFreConfig & 0x3F)) ch->volFreCount = 0;
    }


    // Apply Arpeggio or Note Cut
    if (ch->arpNotes && ch->note) {
      if ((ch->arpCount & 0x1F) < (ch->arpTiming & 0x1F)) ch->arpCount++;
      else {
        if ((ch->arpCount & 0xE0) == 0x00) ch->arpCount = 0x20;
        else if ((ch->arpCount & 0xE0) == 0x20 && !(ch->arpTiming & 0x40) && (ch->arpNotes != 0xFF)) ch->arpCount = 0x40;
        else ch->arpCount = 0x00;
        byte arpNote = ch->note;
        if ((ch->arpCount & 0xE0) != 0x00) {
          if (ch->arpNotes == 0xFF) arpNote = 0;
          else arpNote += (ch->arpNotes >> 4);
        }
        if ((ch->arpCount & 0xE0) == 0x40) arpNote += (ch->arpNotes & 0x0F);
        ch->freq = pgm_read_word(&noteTable[arpNote + ch->transConfig]);
      }
    }


    // Apply Tremolo or Vibrato
    if (ch->treviDepth) {
      int16_t vt = ((ch->treviConfig & 0x40) ? ch->freq : ch->vol);
      vt = (ch->treviCount & 0x80) ? (vt + ch->treviDepth) : (vt - ch->treviDepth);
      if (vt < 0) vt = 0;
      else if (ch->treviConfig & 0x40) if (vt > 9397) vt = 9397;
        else if (!(ch->treviConfig & 0x40)) if (vt > 63) vt = 63;
      (ch->treviConfig & 0x40) ? ch->freq = vt : ch->vol = vt;
      if ((ch->treviCount & 0x1F) < (ch->treviConfig & 0x1F)) ch->treviCount++;
      else {
        if (ch->treviCount & 0x80) ch->treviCount = 0;
        else ch->treviCount = 0x80;
      }
    }


    if (ch->delay) {
      if (ch->delay != 0xFFFF) ch->delay--;
    }
    else {
      do {
        byte cmd = pgm_read_byte(ch->ptr++);
        if (cmd < 64) {
          // 0 … 63 : NOTE ON/OFF
          if (ch->note = cmd) ch->note += ch->transConfig;
          ch->freq = pgm_read_word(&noteTable[ch->note]);
          if (!ch->volFreConfig) ch->vol = ch->reCount;
          if (ch->arpTiming & 0x20) ch->arpCount = 0; // ARP retriggering
        }
        else if (cmd < 160) {
          // 64 … 159 : SETUP FX
          switch (cmd - 64) {
            case 0: // Set volume
              ch->vol = pgm_read_byte(ch->ptr++);
              ch->reCount = ch->vol;
              break;
            case 1: case 4: // Slide volume/frequency ON
              ch->volFreSlide = pgm_read_byte(ch->ptr++);
              ch->volFreConfig = (cmd - 64) == 1 ? 0x00 : 0x40;
              break;
            case 2: case 5: // Slide volume/frequency ON advanced
              ch->volFreSlide = pgm_read_byte(ch->ptr++);
              ch->volFreConfig = pgm_read_byte(ch->ptr++);
              break;
            case 3: case 6: // Slide volume/frequency OFF (same as 0x01 0x00)
              ch->volFreSlide = 0;
              break;
            case 7: // Set Arpeggio
              ch->arpNotes = pgm_read_byte(ch->ptr++);    // 0x40 + 0x03
              ch->arpTiming = pgm_read_byte(ch->ptr++);   // 0x40 (no third note) + 0x20 (toggle retrigger) + amount
              break;
            case 8: // Arpeggio OFF
              ch->arpNotes = 0;
              break;
            case 9: // Set Retriggering (noise)
              ch->reConfig = pgm_read_byte(ch->ptr++);    // RETRIG: point = 1 (*4), speed = 0 (0 = fastest, 1 = faster , 2 = fast)
              break;
            case 10: // Retriggering (noise) OFF
              ch->reConfig = 0;
              break;
            case 11: // ADD Transposition
              ch->transConfig += (char)pgm_read_byte(ch->ptr++);
              break;
            case 12: // SET Transposition
              ch->transConfig = pgm_read_byte(ch->ptr++);
              break;
            case 13: // Transposition OFF
              ch->transConfig = 0;
              break;
            case 14: case 16: // SET Tremolo/Vibrato
              ch->treviDepth = pgm_read_word(ch->ptr++);
              ch->treviConfig = pgm_read_word(ch->ptr++) + ((cmd - 64) == 14 ? 0x00 : 0x40);
              break;
            case 15: case 17: // Tremolo/Vibrato OFF
              ch->treviDepth = 0;
              break;
            case 18: // Glissando
              ch->glisConfig = pgm_read_byte(ch->ptr++);
              break;
            case 19: // Glissando OFF
              ch->glisConfig = 0;
              break;
            case 20: // SET Note Cut
              ch->arpNotes = 0xFF;                        // 0xFF use Note Cut
              ch->arpTiming = pgm_read_byte(ch->ptr++);   // tick amount
              break;
            case 21: // Note Cut OFF
              ch->arpNotes = 0;
              break;
            case 92: // ADD tempo
              tickRate += pgm_read_byte(ch->ptr++);
              cia = 15625 / tickRate;
              break;
            case 93: // SET tempo
              tickRate = pgm_read_byte(ch->ptr++);
              cia = 15625 / tickRate;
              break;
            case 94: // Goto advanced
              for (byte i = 0; i < 4; i++) channel[i].repeatPoint = pgm_read_byte(ch->ptr++);
              break;
            case 95: // Stop channel
              ChannelActiveMute = ChannelActiveMute ^ (1 << (n + 4));
              ch->vol = 0;
              ch->delay = 0xFFFF;
              break;
          }
        } else if (cmd < 224) {
          // 160 … 223 : DELAY
          ch->delay = cmd - 159;
        } else if (cmd == 224) {
          // 224: LONG DELAY
          ch->delay = read_vle(&ch->ptr) + 65;
        } else if (cmd < 252) {
          // 225 … 251 : RESERVED
        } else if (cmd == 252 || cmd == 253) {
          // 252 (253) : CALL (REPEATEDLY)
          byte new_counter = cmd == 252 ? 0 : pgm_read_byte(ch->ptr++);
          byte new_track = pgm_read_byte(ch->ptr++);

          if (new_track != ch->track) {
            // Stack PUSH
            ch->stackCounter[ch->stackIndex] = ch->counter;
            ch->stackTrack[ch->stackIndex] = ch->track; // note 1
            ch->stackPointer[ch->stackIndex] = ch->ptr - trackBase;
            ch->stackIndex++;
            ch->track = new_track;
          }

          ch->counter = new_counter;
          ch->ptr = getTrackPointer(ch->track);
        } else if (cmd == 254) {
          // 254 : RETURN
          if (ch->counter > 0 || ch->stackIndex == 0) {
            // Repeat track
            if (ch->counter) ch->counter--;
            ch->ptr = getTrackPointer(ch->track);
            //asm volatile ("  jmp 0"); // reboot
          } else {
            // Check stack depth
            if (ch->stackIndex == 0) {
              // Stop the channel
              ch->delay = 0xFFFF;
            } else {
              // Stack POP
              ch->stackIndex--;
              ch->ptr = ch->stackPointer[ch->stackIndex] + trackBase;
              ch->counter = ch->stackCounter[ch->stackIndex];
              ch->track = ch->stackTrack[ch->stackIndex]; // note 1
            }
          }
        } else if (cmd == 255) {
          // 255 : EMBEDDED DATA
          ch->ptr += read_vle(&ch->ptr);
        }
      } while (ch->delay == 0);

      if (ch->delay != 0xFFFF) ch->delay--;
    }

    if (!(ChannelActiveMute & (1 << n))) {
      if (n == 3) {
        // Half volume, no frequency for noise channel
        osc[n].vol = ch->vol >> 1;
      } else {
        osc[n].freq = ch->freq;
        osc[n].vol = ch->vol;
      }
    }
    // if all channels are inactive, stop playing or check for repeat

    if (!(ChannelActiveMute & 0xF0))
    {
      byte repeatSong = 0;
      for (byte j = 0; j < 4; j++) repeatSong += channel[j].repeatPoint;
      if (repeatSong) {
        for (byte k = 0; k < 4; k++) {
          channel[k].ptr = getTrackPointer(channel[k].repeatPoint);
          channel[k].delay = 0;
        }
        ChannelActiveMute = 0b11110000;
      }
      else
      {
        ATMsynth::stop();
      }
    }
  }
}
