#include "OsdHandler.h"
#include "spi.h"
#include <Arduino.h>

/*
The OSD screen consists of 30 columns and X rows of panels.
All the panels contain a character, and the font family is
uploaded beforehand.
*/

/*--- convenience functions for MAX7456 ---*/

void enableChangingOsdRegister () {
    digitalWrite(MAX7456_SELECT, LOW);
}

void disableChangingOsdRegister () {
    digitalWrite(MAX7456_SELECT, HIGH);
}

void changeOsdRegister (byte address, byte value) {
    Spi.transfer(address);
    Spi.transfer(value);
}

/*--- low level drawing functions ---*/

/**
 * Put cursor to (x;y) and start autoincrement,
 * enable OSD's SPI. 
 */
void osdOpenPanel (int x, int y) {
    unsigned int panelIndex = y * 30+x;
    byte panelIndexHigh = panelIndex >> 8;
    byte panelIndexLow = panelIndex;

    enableChangingOsdRegister();
    // enable auto increment
    changeOsdRegister(MAX7456_DMM_reg, 0b1);
    changeOsdRegister(MAX7456_DMAH_reg, panelIndexHigh);
    changeOsdRegister(MAX7456_DMAL_reg, panelIndexLow);
}

/**
 * Finish autoincrement & disable OSD's SPI.
 */
void osdClosePanel () {
    // This is needed "trick" to finish auto increment.
    changeOsdRegister(MAX7456_DMDI_reg, MAX7456_END_string);
    disableChangingOsdRegister();

}

void osdWriteChar (char c){
    changeOsdRegister(MAX7456_DMDI_reg, c);
}

/*--- high level drawing functions, define OsdHandler ---*/

OsdHandler::OsdHandler () {
    
    pinMode(MAX7456_SELECT, OUTPUT);
    pinMode(MAX7456_VSYNC, INPUT);
    digitalWrite(MAX7456_VSYNC, HIGH);

    enableChangingOsdRegister();
    changeOsdRegister(MAX7456_VM0_reg, 0b01001100);
    disableChangingOsdRegister();
}

void OsdHandler::waitForVsyncInactive () {
    enableChangingOsdRegister();
    while (Spi.transfer(MAX7456_STAT_reg_read) & MAX7456_STAT_reg_read_VSYNC_INACTIVE);
    disableChangingOsdRegister();
}

/**
 * Clear the whole screen.
 */
void OsdHandler::clear () {
    enableChangingOsdRegister();
    // this clears all the display memory
    changeOsdRegister(MAX7456_DMM_reg, 0b100);
    disableChangingOsdRegister();
}

void OsdHandler::write (int x, int y, char text[], unsigned int textLength){
    osdOpenPanel(x, y);
    for (int i = 0; i < textLength; i++) {
        osdWriteChar(text[i]);
    }
    osdClosePanel();
}

void OsdHandler::writeString (int x, int y, String text) {
    osdOpenPanel(x, y);
    for (char c : text) {
        if (0 <= c && c <= 16){
            osdWriteChar(STARTING_ARROW_CHARACTER+c-1);
        } else if (c == SATTELITE_CHARACTER_ASCII){
            osdWriteChar(SATTELITE_CHARACTER);
        } else if (c == DESTINATION_CHARACTER_ASCII){
            osdWriteChar(DESTINATION_CHARACTER);
        } else if (c == HOME_CHARACTER_ASCII){
            osdWriteChar(HOME_CHARACTER);
        } else if (c == KMH_CHARACTER_ASCII){
            osdWriteChar(KMH_CHARACTER);
        } else if (c == '-') {
            osdWriteChar(MINUS_CHARACTER);
        } else if (c == '.'){
            osdWriteChar(DOT_CHARACTER);
        } else if (c == ' ') {
            osdWriteChar(SPACE_CHARACTER);
        } else if ('0' <= c && c <= '9') {
            osdWriteChar(c - '0' + 1);
        } else if ('A' <= c && c <= 'Z') {
            osdWriteChar(c - 'A' + 11);
        } else if ('a' <= c && c <= 'z') {
            osdWriteChar(c - 'a' + START_OF_SMALL_CHARACTERS + 10);
        }
    }
    osdClosePanel();
}

char OsdHandler::mapArrow(double angle){
    int adjustedAngle = int(angle + 11.25) % 360;
    char mappedAngle = map(adjustedAngle, 0 , 360, 1, 17);
    return mappedAngle;
}