#include "Compass.h"
#include "Adafruit_Sensor.h"
#include "Adafruit_HMC5883_U.h"

#define MAGNETOMETER_ID 12345

namespace Compass {
    auto magnetometer = Adafruit_HMC5883_Unified(MAGNETOMETER_ID);

    void setup() {
    }

    float poll() {
        sensor_t sensor;
        magnetometer.getSensor(&sensor);
        if (magnetometer.begin())
        {
            sensors_event_t event;
            magnetometer.getEvent(&event);

            float bearing = atan2(event.magnetic.y, event.magnetic.x);

            if (bearing < 0)
            {
                bearing += 2 * PI;
            }

            if (bearing > 2 * PI)
            {
                bearing -= 2 * PI;
            }

            float headingDegrees = bearing * 180 / M_PI;

            return headingDegrees;
        }
        return -1;
    }
}