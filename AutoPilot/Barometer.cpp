#include "Adafruit_BMP085.h"

Adafruit_BMP085 barometer;

namespace Barometer {
    /* internal */
    float homeAltitude;

    /* external */
    void setup () {
        barometer.begin();
        homeAltitude = barometer.readAltitude();
    }

    float poll () {
        return barometer.readAltitude() - homeAltitude;
    }
}