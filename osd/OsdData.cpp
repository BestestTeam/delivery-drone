#include "OsdData.h"

/*
Data is transferred using the following protocol:
- OSD
    - Send 0x01
    - Wait for a packet, which looks like this: (string )+/
        All the strings correspond to a field in the OsdData
    - 
*/

/**
 * Removes the first segment of the packet and returns it.
 */
String removeFirstChunk(String& remainingPacket){
    int index = remainingPacket.indexOf(' ');
    String chunk = remainingPacket.substring(0,index);
    remainingPacket.remove(0,index+1);
    return chunk;
}

OsdData::OsdData(){}

void OsdData::getPacket(){
    String packet;
    do {
        Serial.write(0x01);
        packet = Serial.readStringUntil('/');
    } while (packet.length() == 0);
    destinationLatitude = removeFirstChunk(packet).toDouble();
    destinationLongitude = removeFirstChunk(packet).toDouble();
    currentLatitude = removeFirstChunk(packet).toDouble();
    currentLongitude = removeFirstChunk(packet).toDouble();
    currentAltitude = removeFirstChunk(packet).toDouble();
    speed = removeFirstChunk(packet).toDouble();
    directionToGo = removeFirstChunk(packet).toDouble();
    facingDirection = removeFirstChunk(packet).toDouble();
    travelledDistance = removeFirstChunk(packet).toInt(); 
    remainingDistance = removeFirstChunk(packet).toInt();
}

void OsdData::startTimer(){

}
