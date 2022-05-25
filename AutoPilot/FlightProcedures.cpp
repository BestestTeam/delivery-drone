#include "FlightProcedures.h"
#include "Gps.h"
#include "OsdHandler.h"
#include "Barometer.h"
#include "Ultrasonic.h"
#include "Compass.h"
#include "Arduino.h"
#include "math.h"
#include "NMEACoordinates.h"

#define TARGET_ALTITUDE 50 // in meters
#define ULTRASONIC_SENSOR_DISTANCE_FROM_GROUND 25

#define ASCENT_THROTTLE 1500
#define CRUISING_ASCENT_THROTTLE 1400
#define CRUSING_THROTTLE 1350
#define HOVER_THROTTLE 1300
#define CRUISING_DESCENT_THROTTLE 1250
#define SLOW_DESCENT_TRHOTTLE 1200
#define FAST_DESCENT_THROTTLE 1100
#define ZERO_THROTTLE 1000

#define CRUISING_PITCH 1800
#define LEVEL_PITCH 1500
#define POSITION_CORRECTION_PITCH_FORWARD  1600
#define POSITION_CORRECTION_PITCH_BACKWARD  1400

#define LEVEL_ROLL 1500
#define POSITION_CORRECTION_ROLL_LEFT 1400
#define POSITION_CORRECTION_ROLL_RIGHT 1600

#define ROTATION_YAW_VALUE_RIGHT 1600
#define ZERO_YAW 1500
#define ROTATION_YAW_VALUE_LEFT 1400

#define ARMED_CHANNEL_VALUE 1900
#define DISARMED_CHANNEL_VALUE 1000

#define COMPASS_SENSOR_ERROR_BOUNDARY 2 // degrees
#define COMPASS_CORRECTION_BOUNDARY 5 // degrees
#define ALTITUDE_SENSOR_ERROR_BOUNDARY 2 // meters
#define ALTITUDE_CORRECTION_BOUNDARY 5 // meters

// Used to handle every polling request before each loop iteration executes
template <typename F, typename G>
void pollingWhile (
    F&& condition,
    G&& whileBody
) {
    while (condition()) {
        Gps::poll();
		OsdHandler::poll();
        whileBody();
    }
}

FlightProcedures* FlightProcedures::instance;

FlightProcedures* FlightProcedures::getInstance() {
	if(instance == NULL) {
		instance = new FlightProcedures(PPMHandler::getInstance());
	}
	return instance;
}

FlightProcedures::FlightProcedures(PPMHandler* ppmhandler)
  : ppmhandler{ppmhandler}{}


// The algorithm for take off:
// 1. record the coordinates from which take off begins to avoid drifting
// 2. arm the drone and set throttle to a value where it starts ascending
// 3. once the drone is in the air, head north for easier manipulation of drone position for keeping on the target location
// 4. until the target altitude is reached, keep on the recorded initial coordinates
// 5. once the target altitude is reached, set throttle to a value which keeps the drone hovering in place
void FlightProcedures::takeOff() {
	Coordinate takeOffCoordinates = this->getCurrentCoordinates();
	this->ppmhandler->setAux1(ARMED_CHANNEL_VALUE);
	this->ppmhandler->setThrottle(ASCENT_THROTTLE);
	pollingWhile([] {
		return Barometer::poll() < TARGET_ALTITUDE;
	}, [this, takeOffCoordinates] {
		this->headNorth();
		this->keepOnPosition(takeOffCoordinates);
	});
	this->ppmhandler->setThrottle(HOVER_THROTTLE);
}


// The algorithm for landing:
// 1. head north for easier manipulation of drone position for keeping on the target location
// 2. checks to see if we are already in range of the ultrasonic sensor
// 3. if not, descend fastly until it is in range
// 4. now it's in range, thus starts descending slowly, so that the drone won't damage itself
//    once it touches down
// 5. once it reaches the height at which the ultrasonic sensor is placed at the bottom of
//    the drone, it disarms
void FlightProcedures::land() {
	Coordinate landingCoordinates = this->getCurrentCoordinates();

	this->ppmhandler->setThrottle(FAST_DESCENT_THROTTLE);
	pollingWhile([] {
		return Ultrasonic::poll() == -1;
	}, [this, landingCoordinates] {
		this->headNorth();
		this->keepOnPosition(landingCoordinates);
	});
	
	this->ppmhandler->setThrottle(SLOW_DESCENT_TRHOTTLE);
	pollingWhile([] {
		return Ultrasonic::poll() > ULTRASONIC_SENSOR_DISTANCE_FROM_GROUND;
	}, [this, landingCoordinates] {
		this->headNorth();
		this->keepOnPosition(landingCoordinates);
	});

	this->ppmhandler->setThrottle(ZERO_THROTTLE);
	this->ppmhandler->setAux1(DISARMED_CHANNEL_VALUE);
}


// The algorithm for navigation:
// 1. turn in the direction of the target destination
// 2. pitch the drone and set the throttle to cruise value, to start moving forward
// 3. until the drone's location is not equal to the target destination,
//    keep looking towards the destination and keep flying at the target altitude
// 4. once its destination is reached, set the pitch to the value where it levels,
//	  then set throttle to a value which keeps it in the current altitude
void FlightProcedures::navigate(Coordinate destination) {
	this->headInCorrectDirection(destination);
	this->ppmhandler->setPitch(CRUISING_PITCH);
	this->ppmhandler->setThrottle(CRUSING_THROTTLE);
	pollingWhile([this, destination] {
		return !this->destinationReached(destination);
	}, [this, destination] {
		this->keepOnDirection(destination);
		this->keepOnAltitude(TARGET_ALTITUDE);
	});
	this->ppmhandler->setPitch(LEVEL_PITCH);
	this->ppmhandler->setThrottle(HOVER_THROTTLE);
}

unsigned long FlightProcedures::elapsedTimeInMicroseconds(unsigned long startTime) {
	return micros() - startTime;
}

Coordinate FlightProcedures::getCurrentCoordinates() {

	NMEACoordinates nmeacoordinate;
	nmeacoordinate.latitude = Gps::latitude;
	nmeacoordinate.longitude = Gps::longitude;
	Coordinate coordinate = GPS::parseNMEA(nmeacoordinate);
	return coordinate;
}

void FlightProcedures::headNorth() {
	Coordinate coordinatesDirectlyNorthFromCurrent = this->getCurrentCoordinates();
	coordinatesDirectlyNorthFromCurrent.latitude += 1;
	this->headInCorrectDirection(coordinatesDirectlyNorthFromCurrent);
}


// Method that allows the drone to correct course to keep flying towards it's destination
// The algorithm:
// 1. compare the direction where the drone is headed to the direction it should be heading
// 2. if the two values are different enough to correct the direction:
// 3. first, make sure the drone is level, since yawing while being pitched does not fully correlate to turning
// 4. correct the drone's direction so that it's headed towards the destination again
// 5. start moving forward again
void FlightProcedures::keepOnDirection(Coordinate destination) {
	float currentDirection = Compass::poll();
	float correctDirection = this->getCorrectDirection(destination);
	if(!this->directionWithinCorrectionBoundary(currentDirection, correctDirection)) {
		this->ppmhandler->setPitch(LEVEL_PITCH);
		this->ppmhandler->setThrottle(HOVER_THROTTLE);
		this->headInCorrectDirection(destination);
		this->ppmhandler->setPitch(CRUISING_PITCH);
		this->ppmhandler->setThrottle(CRUSING_THROTTLE);
	}
}


// Method that makes sure that the drone keeps flying at the desired altitude
// The algorithm:
// 1. compare the altitude the drone is at currently to the desired altitude
// 2. if the the values are different enough to correct the altitude:
// 3. if the drone is flying above the desired altitude, decrease throttle to slowly lose altitude,
//    if the drone is flying below the desired altitude, increase throttle to slowly gain altitude
// 4. once the current and desired altitudes are sufficiently close, we reset the throttle to the cruising value
void FlightProcedures::keepOnAltitude(float desiredAltitude) {
	float currentAltitude = Barometer::poll();
	if(!this->altitudeWithinCorrectionBoundary(currentAltitude, desiredAltitude)) {
		this->ppmhandler->setThrottle(currentAltitude < desiredAltitude
			? CRUISING_ASCENT_THROTTLE
			: CRUISING_DESCENT_THROTTLE);
		pollingWhile([this, desiredAltitude] {
			return !this->altitudeWithinSensorErrorBoundary(Barometer::poll(), desiredAltitude);
		}, [] { /* do nothing */});
		this->ppmhandler->setThrottle(CRUSING_THROTTLE);
	}
}

// Important: this function should only be called after the drone is set to look directly north
// The algorithm:
// 1. to correct for latitude difference, check if the target location is to the North or to the South
// 2. to go North, pitch forward, to go South, pitch backward (this is one place where it's important
//    that the drone is facing North)
// 3. once the latitude difference is corrected, set pitch to level value
// 3. to correct for longitude difference, check if the target location is to the West or the East
// 4. to go West, roll left, to go South, roll right (this is the other place where it's important that
//    the drone is facing North)
// 5. once the longitude difference is corrected, set the roll to level value
void FlightProcedures::keepOnPosition(Coordinate desiredCoordinate) {
	Coordinate currentCoordinate = this->getCurrentCoordinates();
	float latitudeDifference = desiredCoordinate.latitude - currentCoordinate.latitude;
	if(latitudeDifference != 0) {
		this->ppmhandler->setPitch(latitudeDifference > 0
			? POSITION_CORRECTION_PITCH_FORWARD
			: POSITION_CORRECTION_PITCH_BACKWARD
		);
		pollingWhile([this, desiredCoordinate] {
			return this->getCurrentCoordinates().latitude - desiredCoordinate.latitude != 0;
		}, [] { /* do nothing */ });
		this->ppmhandler->setPitch(LEVEL_PITCH);
	}
	float longitudeDifference = desiredCoordinate.longitude - currentCoordinate.longitude;
	if(longitudeDifference != 0) {
		this->ppmhandler->setRoll(longitudeDifference > 0
			? POSITION_CORRECTION_ROLL_LEFT
			: POSITION_CORRECTION_ROLL_RIGHT
		);
		pollingWhile([this, desiredCoordinate] {
			return this->getCurrentCoordinates().longitude - desiredCoordinate.longitude != 0;
		}, [] { /* do nothing */ });
		this->ppmhandler->setRoll(LEVEL_ROLL);
	}
}


// This method is used to turn the drone in the direction of the destination
// The algorithm:
// 1. check to see if the drone's current direction and the correct direction are different enough to start correcting it
// 2. if they are, decide if the desired direction is closer to the left or the right
// 3. start turning either left or right
// 4. keep turning until the current direction is sufficiently close to the correct direction
// 5. stop turning
void FlightProcedures::headInCorrectDirection(Coordinate destination) {
	float currentDirection = Compass::poll();
	float correctDirection = this->getCorrectDirection(destination);
	float difference = correctDirection - currentDirection;
	if(!this->directionWithinCorrectionBoundary(currentDirection, correctDirection)) {
		pollingWhile([this, correctDirection] {
			return !this->directionWithinSensorErrorBoundary(Compass::poll(), correctDirection);
		}, [this, correctDirection] {
			float currentDirection = Compass::poll();
			float difference = correctDirection - currentDirection;
			this->ppmhandler->setYaw((difference > 0 && difference < 180 || difference < -180 && difference > -360)
				? ROTATION_YAW_VALUE_RIGHT
				: ROTATION_YAW_VALUE_LEFT);
		});
		this->ppmhandler->setYaw(ZERO_YAW);
	}
}

bool FlightProcedures::destinationReached(Coordinate destination) {
	Coordinate currentCoordinates = this->getCurrentCoordinates();
	return currentCoordinates.latitude == destination.latitude &&
		currentCoordinates.longitude == destination.longitude;
}


// This function tells us which direction the drone should be facing
// to look in the direction of the destination.
// It uses the compass' convention: 0° is North, goes from 0 to 360
// counterclockwise
float FlightProcedures::getCorrectDirection(Coordinate destination) {
	Coordinate currentCoordinates = this->getCurrentCoordinates();
	float deltaX = destination.longitude - currentCoordinates.longitude;
	float deltaY = destination.latitude - currentCoordinates.latitude;
	float theta = atan2(-deltaX, deltaY) * 180 / M_PI;
	theta = theta < 0 
		? 360 + theta
		: theta;
	return theta;
}


// returns true if the difference between current and correct altitudes are close
// enough so that it doesn't require correcting, returns false if the difference
// is great enough to start correcting it
bool FlightProcedures::altitudeWithinCorrectionBoundary(float currentAltitude, float desiredAltitude) {
	return abs(currentAltitude - desiredAltitude) <= ALTITUDE_CORRECTION_BOUNDARY;
}

// returns true if the difference between current and correct altitudes are close
// enough so that they can be considered the same(because of error/sensitivity of
// the sensor), returns false if the difference is great enough to treat them as
// different values
bool FlightProcedures::altitudeWithinSensorErrorBoundary(float currentAltitude, float desiredAltitude) {
	return abs(currentAltitude - desiredAltitude) <= ALTITUDE_SENSOR_ERROR_BOUNDARY;
}

// returns true if the difference between current and correct directions are close
// enough so that it doesn't require correcting, returns false if the difference
// is great enough to start correcting it
bool FlightProcedures::directionWithinCorrectionBoundary(float currentDirection, float desiredDirection) {
	return abs(currentDirection - desiredDirection) <= COMPASS_CORRECTION_BOUNDARY;
}

// returns true if the difference between current and correct directions are close
// enough so that they can be considered the same(because of error/sensitivity of
// the sensor), returns false if the difference is great enough to treat them as
// different values
bool FlightProcedures::directionWithinSensorErrorBoundary(float currentDirection, float desiredDirection) {
	return abs(currentDirection - desiredDirection) <= COMPASS_SENSOR_ERROR_BOUNDARY;
}
