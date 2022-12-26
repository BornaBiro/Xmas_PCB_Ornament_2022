#include <tinyNeoPixel_Static.h>
#include "defines.h"
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <avr/power.h>

// Custom library for LED effects
#include "effects.h"

// Melodies
#include "melodies.h"

volatile uint8_t flag = 0;

// 0 - Off, 1 - Only LEDs, 2 - LEDs & Music
uint8_t currentDeviceState = 1;

// Song stuff.
unsigned long melodyTimestamp = 0;
unsigned long melodyDelay = 0;
int currentNote = 0;
uint8_t currentMelody = 0;

// LED framebuffer
byte pixels[NUMPIXELS * 3];

// WS2812B LED object
tinyNeoPixel leds = tinyNeoPixel(NUMPIXELS, PIN, NEO_GRB, pixels);

// LED effect library object
Effects blinky;

void setup()
{
    // NO INIT FOR TINYNEOPIXEL LIBRARY
    pinMode(PIN, OUTPUT);

    // Pin for the button
    pinMode(BTN_PIN, INPUT_PULLUP);

    // Pin for the buzzer
    pinMode(BUZZER, OUTPUT);

    // Pin for the WS supply enable
    pinMode(WS_ENABLE, OUTPUT);
    
    // Enable the LEDs rightaway
    digitalWrite(WS_ENABLE, LOW);

    // Disable ADC to reduce power.
    ADCSRA &= ~(1 << ADEN);

    // Enable interrupt on INT0
    attachInterrupt(digitalPinToInterrupt(BTN_PIN), myIsr, FALLING);

    // Enable global iterrupts
    sei();

    // Init the library for the effects.
    blinky.init();
    // Set current effect to be first effect.
    blinky.setEffect(0);
    // Delay (can be removed, use only as a backup for the low power testing).
    delay(1000);
}

void loop()
{
    // Update the library for the LED effects and for the melody.
    // NO NOT USE DELAY IN THE LOOP!!! It will mess up the for of the these two libraries.
    blinky.update(leds);
    melodyUpdate();

    // If the interrupt is detected, update the device state.
    if (flag)
    {
        flag = 0;
        currentDeviceState++;
        if (currentDeviceState > 2)
            currentDeviceState = 0;

        switch (currentDeviceState)
        {
            case 0:
            {
                // Power down mode
                // First disable the buzzer
                noTone(BUZZER);

                // Turn the WS LEDs off
                blinky.ledsOff(leds);
                digitalWrite(WS_ENABLE, HIGH);

                // Wait until button is released.
                while (!digitalRead(BTN_PIN));

                // Set enable INT on low state (other Interrupt modes are not working in the sleep)
                attachInterrupt(digitalPinToInterrupt(BTN_PIN), myIsr, LOW);             

                // Set sleep mode
                set_sleep_mode(SLEEP_MODE_PWR_DOWN);

                // Enable sleep mode
                sleep_enable();

                // Put MCU to sleep here
                sleep_mode();

                // After wake up, disable sleep
                sleep_disable();

                // Wait until button is released.
                while (!digitalRead(BTN_PIN));
                // Enable INT on falling edge.
                attachInterrupt(digitalPinToInterrupt(BTN_PIN), myIsr, FALLING);

                // Enable global Interrupts
                sei();

                // Clear the lag for the interrupts
                flag = 0;

                // Enable the supply for the LEDs
                digitalWrite(WS_ENABLE, LOW);
                currentDeviceState = 1;
                blinky.setEffect(0);
                break;
            }
            case 1:
            {
                // Power on a LEDs
                blinky.setEffect(0);
                break;
            }
            case 2:
            {
                // Make music!
                currentMelody = 0;
                currentNote = 0;
                melodyTimestamp = millis() - 500;
            }
        }
    }
}

// Button is connected to the INT0 pin of the ATtiny.
void myIsr()
{
    flag = 1;
}

// This should be made like a library. It updates the note of the melody.
void melodyUpdate()
{
    // Update it only in mode where melody is allowed (mode 2)
    if (currentDeviceState != 2) return;

    // Check if the note needs to be updated.
    if ((unsigned long)(millis() - melodyTimestamp) > melodyDelay)
    {
        // If yes, then update the note and set an new note duration and a timestamp.
        tone(BUZZER, pgm_read_word((currentMelodyNotes[currentMelody] + currentNote)));
        melodyDelay = pgm_read_word((currentMelodyDurations[currentMelody] + currentNote));
        melodyTimestamp = millis();

        // Increment the current note position in the array.
        currentNote++;

        // Check if you got till end of the song. If yes, then set a new song.
        if (currentNote > pgm_read_byte(&melodyElements[currentMelody]))
        {
            // Reset a current note and increment the melody.
            currentNote = 0;
            currentMelody++;

            // Disable the buzzer (make a silence of about 500ms)
            noTone(BUZZER);
            melodyDelay = 500;

            // No more sogns? Go back to the first one and start everything all over.
            if (currentMelody > 7)
            {
                currentMelody = 0;
            }
        }
    }
}
