#ifndef OSD_DATA_H
#define OSD_DATA_H

#include <Arduino.h>

class OsdData {
    public:
        OsdData ();

        double destinationLatitude;
        double destinationLongitude;
        double currentLatitude;
        double currentLongitude;
        double currentAltitude;
        double speed;
        double directionToGo;
        double facingDirection;
        int travelledDistance; 
        int remainingDistance;

        void getPacket();
        void startTimer();    
};



#endif