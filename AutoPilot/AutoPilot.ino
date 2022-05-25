#include "Arduino.h"
#include "PPMHandler.h"
#include "Coordinate.h"
#include "FlightProcedures.h"
#include "Barometer.h"
#include "Compass.h"
#include "Gps.h"
#include "Ultrasonic.h"


Coordinate offshoreSiteLocation = {
    0,
    0
};


void setup()
{
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial3.begin(9600);
    
    // This part gets the OSD working,
    // no clue why.
    pinMode(8, OUTPUT);
    delay(2000);
    digitalWrite(8, HIGH);

    Barometer::setup();
    Compass::setup();
    Gps::setup();
    Ultrasonic::setup();
    Serial.println("Take off initiated");
    FlightProcedures::getInstance()->takeOff();
}

void loop()
{
    
}

ISR(TIMER1_COMPA_vect) 
{
    Serial.println("Sending data to FC");
    PPMHandler::getInstance()->sendData();
}
