#include "OsdHandler.h"
#include "Arduino.h"
#include "Gps.h"
#include "Barometer.h"
#include "Compass.h"
#include "FlightProcedures.h"
#include "Coordinate.h"

namespace OsdHandler {
    void setup() {

    }

    void poll() {
        if(Serial3.available()) {
            Serial.println("Osd handler found data");
            String data = String(11) + " " +
                String(12) + " " +
                String(Gps::latitude, 4) + " " +
                String(Gps::longitude * -1, 4) + " " +
                String(Barometer::poll()) + " " +
                String(Gps::speedKN) + " " +
                String(FlightProcedures::getInstance()->getCorrectDirection({11,12})) + " " + // direction to go
                String(Compass::poll()) + " " + // facing direction
                String(69) + " " + // travelled distance
                String(420) + "/"; // remaining distance
            Serial.println(data);
            Serial3.print(data);
            
            int something;
            while((something = Serial3.read()) != -1){
                Serial.println("Emptying buffer: " + String(something));
            }
        }

    }
}
