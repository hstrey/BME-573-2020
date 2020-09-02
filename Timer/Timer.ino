#include <nrf.h>
#include "nrf_timer.h"
#include "Timer.h"

#define OUT_PIN  7

#define nrf_timer_num   (1)
#define cc_channel_num  (0)
TimerClass timer(nrf_timer_num, cc_channel_num);

int led_state=0;

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");

    pinMode(OUT_PIN, OUTPUT);
    digitalWrite(OUT_PIN, 0);

    timer.attachInterrupt(&Timer_callback, 300000); // microseconds
}

void Timer_callback() {
    timer.attachInterrupt(&Timer_callback, 300000); // microseconds
    Serial.println("timer called...");
    if (led_state) {
      digitalWrite(OUT_PIN, 1); // to visualize when it was called
      led_state = 0;
    } else {
      digitalWrite(OUT_PIN, 0);
      led_state = 1;
    }
}

void loop() {
    // Sleep
    __WFE(); // Enter System ON sleep mode (OFF mode would stop timers)
    __SEV(); // Make sure any pending events are cleared
    __WFE(); // More info about this sequence:
// devzone.nordicsemi.com/f/nordic-q-a/490/how-do-you-put-the-nrf51822-chip-to-sleep/2571
}
