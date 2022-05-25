#include "Ultrasonic.h"
#include "Arduino.h"

namespace Ultrasonic {
    /* internal */
    enum class State {
        STOPPED,
        STARTED,
        HIGH_LEVEL
    };

    volatile unsigned long highStartTime = 0;
    volatile unsigned long time = 0;
    volatile State state = State::STOPPED;
    unsigned long startTime = 0;

    void echoPinChangeCallback () {
        switch (state) {
            case State::STARTED:
                state = State::HIGH_LEVEL;
                highStartTime = micros();
                break;
            case State::HIGH_LEVEL:
                state = State::STOPPED;
                time = micros() - highStartTime;
                break;
        }
    }

    /* external */
    void setup () {
        attachInterrupt(digitalPinToInterrupt(ECHO_PIN), echoPinChangeCallback, CHANGE);
        pinMode(TRIGGER_PIN, OUTPUT);
    }

    double poll () {
        cli();
        auto timeLocal = time;
        auto stateLocal = state;
        sei();

        if (stateLocal == State::STOPPED) {
            digitalWrite(TRIGGER_PIN, HIGH);
            delayMicroseconds(10);
            digitalWrite(TRIGGER_PIN, LOW);
            startTime = millis();
            state = State::STARTED;
        }

        return timeLocal > 24000 || startTime + 24 < millis()
            ? -1
            : double(timeLocal) / 58.0;
    }
}
