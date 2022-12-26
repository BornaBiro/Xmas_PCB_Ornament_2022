#include "effects.h"

// Empty constructor
Effects::Effects()
{
  
}

// Init the class (must get new timestamps for the pattern and for the animation change.)
void Effects::init()
{
    effectTimeoutTimestamp = millis();
    effectPatternTimestamp = millis();
    currentEffect = 0;
}

// Set the current effect
void Effects::setEffect(uint8_t _effect)
{
    currentEffect = _effect;
    init();
}

// Get the current effect
uint8_t Effects::getEffect()
{
    return currentEffect;
}

// This must be called constantly in order to update the effect class.
void Effects::update(tinyNeoPixel &leds)
{
    // Check if the effect needs to be updated
    if ((unsigned long)(millis() - effectTimeoutTimestamp) > effectTimeout)
    {
        // Update the variable for the effect change.
        effectTimeoutTimestamp = millis();

        // Update the effect variable
        currentEffect++;

        // Effect needs to be changed? Turn off all LEDs.
        ledsOff(leds);

        // Set up variables according to the effect
        switch (currentEffect)
        {
            case 0:
            {
                effectPatternTimeout = 15;
                patternVar = 0;
                break;
            }
            case 1:
            {
                effectPatternTimeout = 500;
                patternVar = 0;
                break;
            }
            case 2:
            {
                effectPatternTimeout = 50;
                patternVar = 0;
                tempLeds[0] = random(0, 1);
                break;
            }
            case 3:
            {
                effectPatternTimeout = 500;
                patternVar = 0;
                break;
            }
        }

        // Got no more effects? Reset counter to zero.
        if (currentEffect > 3)
        {
            currentEffect = 0;
        }
    }

    // Check if the pattern needs to be updated
    if ((unsigned long)(millis() - effectPatternTimestamp) > effectPatternTimeout)
    {
        switch (currentEffect)
        {
            case 0:
            {
                randomFlashes(leds);
                break;
            }
            case 1:
            {
                redAndGreen(leds);
                break;
            }
            case 2:
            {
                runnigLights(leds);
                break;
            }
            case 3:
            {
                alternateLights(leds);
                break;
            }

        }
        effectPatternTimestamp = millis();
    }
}

// Effects...

// Random flashes...like the name says, it makes random LED glow white for a short period of the time.
void Effects::randomFlashes(tinyNeoPixel &leds)
{
    // Check if the LEDs needs to be turned off or on
    if (patternVar % 2)
    {
        // Generate random number
        tempLeds[0] = random(0, 7);
    
        // Turn on the specific LED
        leds.setPixelColor(tempLeds[0] , leds.Color(LED_BRIGHT, LED_BRIGHT, LED_BRIGHT));
        leds.show();

        // Make a delay (using millis) - 15 milliseconds-ish
        effectPatternTimeout = 15;
    }
    else
    {
        // Turn off LED
        leds.setPixelColor(tempLeds[0] , leds.Color(0, 0, 0));
        leds.show();

        // Much bigger delay
        effectPatternTimeout = random(5, 250);
    }
    patternVar++;
}

void Effects::redAndGreen(tinyNeoPixel &leds)
{
    // Mask everything except last bit
    patternVar &= 1;

    // Turn on red LEDs
    for (int i = patternVar; i < NUMPIXELS; i+= 2)
    {
        leds.setPixelColor(i, leds.Color(LED_BRIGHT, 0, 0));
    }

    // Increment variable so the first LED in a loop is shifted by one place
    // And once again, mask everything except LSB
    patternVar = (patternVar + 1) & 1;

    // Turn on green LEDs
    for (int i = patternVar; i < NUMPIXELS; i += 2)
    {
        leds.setPixelColor(i, leds.Color(0, LED_BRIGHT, 0));
    }

    // Update the LEDs
    leds.show();
}

void Effects::runnigLights(tinyNeoPixel &leds)
{
    uint8_t _chaseMask[] = {0, 0, 0, 0, 0, 0, 0, 0, 60, 100, 80, 40, 20, 10, 5, 0, 0, 0, 0, 0, 0, 0, 0};

    if (tempLeds[0] % 2)
    {
        for (int i = 0; i < NUMPIXELS; i++)
        {
            leds.setPixelColor(i, ColorHSV(32, 32, _chaseMask[i + patternVar]));
        }
    }
    else
    {
        for (int i = 0; i < NUMPIXELS; i++)
        {
            leds.setPixelColor(NUMPIXELS - i - 1, ColorHSV(32, 32, _chaseMask[i + patternVar]));
        }
    }
    leds.show();

    if (patternVar > 14)
    {
        patternVar = 0;
        effectPatternTimeout = 1000;
        tempLeds[0] = random(0, 255);
    }
    else
    {
        effectPatternTimeout = 50;
        patternVar++;
    }
}

void Effects::alternateLights(tinyNeoPixel &leds)
{   
    for (int i = (patternVar % 2); i < NUMPIXELS; i+=2)
    {
        leds.setPixelColor(i, leds.Color(LED_BRIGHT, LED_BRIGHT / 2, 0));
    }

    // Trun off LEDs
    for (int i = ((patternVar + 1) % 2); i < NUMPIXELS; i+=2)
    {
        leds.setPixelColor(i, leds.Color(0, 0, 0));
    }
    leds.show();

    patternVar++;
}

void Effects::ledsOff(tinyNeoPixel &leds)
{
    for (int i = 0; i < NUMPIXELS; i++)
    {
        leds.setPixelColor(i, leds.Color(0, 0, 0));   
    }
    leds.show();
}

uint32_t Effects::ColorHSV(uint16_t hue, uint8_t sat, uint8_t val)
{

    uint8_t r, g, b;

    // Remap 0-65535 to 0-1529. Pure red is CENTERED on the 64K rollover;
    // 0 is not the start of pure red, but the midpoint...a few values above
    // zero and a few below 65536 all yield pure red (similarly, 32768 is the
    // midpoint, not start, of pure cyan). The 8-bit RGB hexcone (256 values
    // each for red, green, blue) really only allows for 1530 distinct hues
    // (not 1536, more on that below), but the full unsigned 16-bit type was
    // chosen for hue so that one's code can easily handle a contiguous color
    // wheel by allowing hue to roll over in either direction.
    hue = (hue * 1530L + 32768) / 65536;
    // Because red is centered on the rollover point (the +32768 above,
    // essentially a fixed-point +0.5), the above actually yields 0 to 1530,
    // where 0 and 1530 would yield the same thing. Rather than apply a
    // costly modulo operator, 1530 is handled as a special case below.

    // So you'd think that the color "hexcone" (the thing that ramps from
    // pure red, to pure yellow, to pure green and so forth back to red,
    // yielding six slices), and with each color component having 256
    // possible values (0-255), might have 1536 possible items (6*256),
    // but in reality there's 1530. This is because the last element in
    // each 256-element slice is equal to the first element of the next
    // slice, and keeping those in there this would create small
    // discontinuities in the color wheel. So the last element of each
    // slice is dropped...we regard only elements 0-254, with item 255
    // being picked up as element 0 of the next slice. Like this:
    // Red to not-quite-pure-yellow is:        255,   0, 0 to 255, 254,   0
    // Pure yellow to not-quite-pure-green is: 255, 255, 0 to   1, 255,   0
    // Pure green to not-quite-pure-cyan is:     0, 255, 0 to   0, 255, 254
    // and so forth. Hence, 1530 distinct hues (0 to 1529), and hence why
    // the constants below are not the multiples of 256 you might expect.

    // Convert hue to R,G,B (nested ifs faster than divide+mod+switch):
    if (hue < 510)
    { // Red to Green-1
        b = 0;
        if (hue < 255)
        { //   Red to Yellow-1
            r = 255;
            g = hue; //     g = 0 to 254
        }
        else
        {                  //   Yellow to Green-1
            r = 510 - hue; //     r = 255 to 1
            g = 255;
        }
    }
    else if (hue < 1020)
    { // Green to Blue-1
        r = 0;
        if (hue < 765)
        { //   Green to Cyan-1
            g = 255;
            b = hue - 510; //     b = 0 to 254
        }
        else
        {                   //   Cyan to Blue-1
            g = 1020 - hue; //     g = 255 to 1
            b = 255;
        }
    }
    else if (hue < 1530)
    { // Blue to Red-1
        g = 0;
        if (hue < 1275)
        {                   //   Blue to Magenta-1
            r = hue - 1020; //     r = 0 to 254
            b = 255;
        }
        else
        { //   Magenta to Red-1
            r = 255;
            b = 1530 - hue; //     b = 255 to 1
        }
    }
    else
    { // Last 0.5 Red (quicker than % operator)
        r = 255;
        g = b = 0;
    }

    // Apply saturation and value to R,G,B, pack into 32-bit result:
    uint32_t v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
    uint16_t s1 = 1 + sat;  // 1 to 256; same reason
    uint8_t s2 = 255 - sat; // 255 to 0
    return ((((((r * s1) >> 8) + s2) * v1) & 0xff00) << 8) | (((((g * s1) >> 8) + s2) * v1) & 0xff00) |
           (((((b * s1) >> 8) + s2) * v1) >> 8);
}