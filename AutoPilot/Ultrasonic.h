#pragma once

#define TRIGGER_PIN 5
#define ECHO_PIN 2

namespace Ultrasonic {
    void setup ();
    double poll ();
}