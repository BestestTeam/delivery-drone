#pragma once

#include "PPMHandler.h"
#include "Coordinate.h"

class FlightProcedures
{
  private:
    static FlightProcedures* instance;
    PPMHandler* ppmhandler;
    FlightProcedures(PPMHandler* ppmhandler);

    float readUltrasonicBlocking();
    unsigned long elapsedTimeInMicroseconds(unsigned long startTime);
    Coordinate getCurrentCoordinates();
    void headNorth();
    void keepOnAltitude(float desiredAltitude);
    void keepOnPosition(Coordinate desiredCoordinate);
    void keepOnDirection(Coordinate destination);
    void headInCorrectDirection(Coordinate destination);
    bool destinationReached(Coordinate destination);
    bool altitudeWithinCorrectionBoundary(float currentAltitude, float desiredAltitude);
    bool altitudeWithinSensorErrorBoundary(float currentAltitude, float desiredAltitude);
    bool directionWithinCorrectionBoundary(float currentDirection, float desiredDirection);
    bool directionWithinSensorErrorBoundary(float currentDirection, float desiredDirection);

  public:
    static FlightProcedures* getInstance();
    float getCorrectDirection(Coordinate destination);

  	void takeOff();
  	void land();
    void navigate(Coordinate destination);
};
