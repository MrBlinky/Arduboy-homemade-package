#ifndef _ATMLIB_H_
#define _ATMLIB_H_
#include <stddef.h>
#include <inttypes.h>
#include <Arduino.h>

#define CH_ZERO             0
#define CH_ONE              1
#define CH_TWO              2
#define CH_THREE            3

extern byte trackCount;
extern const word *trackList;
extern const byte *trackBase;
extern uint8_t pcm;

extern bool half;

class ATMsynth {

  public:
    ATMsynth() {};

    // Load and play specified song
    static void play(const byte *song);

    // Play or Pause playback
    static void playPause();

    // Stop playback (unloads song)
    static void stop();

    static void muteChannel(byte ch);

    static void unMuteChannel(byte ch);
};


// oscillator structure
typedef struct {
  uint8_t  vol;
  uint16_t freq;
  uint16_t phase;
} osc_t;

typedef osc_t Oscillator;

extern osc_t osc[4];


uint16_t read_vle(const byte **pp);
static inline const byte *getTrackPointer(byte track);

extern void ATM_playroutine() asm("ATM_playroutine");
#endif
