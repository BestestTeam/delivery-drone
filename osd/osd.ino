#include "OsdHandler.h"
#include "OsdData.h"
#include "spi.h"

OsdHandler osd;
OsdData osdData;

void setup () {
    Serial.begin(9600);
    Serial.setTimeout(1000);
}

void loop () {
    osdData.getPacket();
    updateDisplay();
    delay(100);
}

void updateDisplay(){
    osd.waitForVsyncInactive();

    //Display the direction of destination
    char arrowOffsetForDirectionToGo = osd.mapArrow(osdData.directionToGo); 
    osd.writeString(15, 1, String(arrowOffsetForDirectionToGo));
    //Display the direction the drone is facing
    char arrowOffsetForFacingDirection = osd.mapArrow(osdData.facingDirection); 
    osd.writeString(28, 1, String(arrowOffsetForFacingDirection));
    //Display the remaining and travelled distance
    osd.writeString(22, 11, String(DESTINATION_CHARACTER_ASCII)+ " " + padString(String(osdData.remainingDistance), 5));
    osd.writeString(22, 12, String(HOME_CHARACTER_ASCII)+ " " + padString(String(osdData.travelledDistance), 5));
    //Display current latitude, longitude and altitude
    osd.writeString(1, 10, String("lat ") + padString(String(osdData.currentLatitude, 4), 10));
    osd.writeString(1, 11, String("long ") + padString(String(osdData.currentLongitude, 4), 10));
    osd.writeString(1, 12, String("alt ") + padString(String(osdData.currentAltitude, 2), 10));
    //Display speed
    osd.writeString(1, 1, String("speed ") + String(osdData.speed, 4));
}

String padString(String text, int n){
    int i = n - text.length();
    for(i; i >= 0 ; i--){
        text += " ";
    }
    return text;
}