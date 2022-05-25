#pragma once

#include "Arduino.h"

namespace Gps {
    extern String time;
    extern char positionStatus;
    extern float latitude;
    extern char latitudeHemisphere;
    extern float longitude;
    extern char longitudeHemisphere;
    extern float speedKN;
    extern float track;
    extern String date;
    extern float magneticVariation;
    extern char magneticVariationDirection;

    void setup ();
    void poll ();
}