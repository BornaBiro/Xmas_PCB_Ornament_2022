#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include <tinyNeoPixel_Static.h>
#include "defines.h"

class Effects
{
  public:
    Effects();
    void init();
    void update(tinyNeoPixel &leds);
    void setEffect(uint8_t _effect);
    uint8_t getEffect();

    void randomFlashes(tinyNeoPixel &leds);
    void redAndGreen(tinyNeoPixel &leds);
    void runnigLights(tinyNeoPixel &leds);
    void alternateLights(tinyNeoPixel &leds);

    // Function turns off all LEDs.
    void ledsOff(tinyNeoPixel &leds);
    uint32_t ColorHSV(uint16_t hue, uint8_t sat, uint8_t val);

   private:

    // Track current effect
    uint8_t currentEffect = 0;
   
    // General purpose array for LEDs
    uint8_t tempLeds[8];

    // General purpose variable
    uint8_t patternVar;

    // Timecapture for effect
    unsigned long effectTimeoutTimestamp = 0;

    // Timeout value for the current effect
    unsigned long effectTimeout = 60000ULL;

    // Effect pattern update timestap
    unsigned long effectPatternTimestamp = 0;

    // Effect pattern update timeout
    unsigned long effectPatternTimeout = 500ULL;
};

#endif
