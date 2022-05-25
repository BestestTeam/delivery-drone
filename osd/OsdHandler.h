#ifndef OSD_HANDLER
#define OSD_HANDLER

#include <Arduino.h>

// MAX7456 pins
#define MAX7456_SELECT 6 //SS
#define MAX7456_VSYNC 2

// MAX7456 reg write addresses
#define MAX7456_VM0_reg   0x00
#define MAX7456_DMM_reg   0x04
#define MAX7456_DMAH_reg  0x05
#define MAX7456_DMAL_reg  0x06
#define MAX7456_DMDI_reg  0x07

// MAX7456 reg read addresses
#define MAX7456_STAT_reg_read 0xa0
#define MAX7456_STAT_reg_read_VSYNC_INACTIVE 0b10000

#define MAX7456_END_string 0xFF

#define STARTING_ARROW_CHARACTER 0x50
#define SPACE_CHARACTER 0x00
#define START_OF_SMALL_CHARACTERS 0x25

#define SATTELITE_CHARACTER 0x80
#define DESTINATION_CHARACTER 0x81
#define HOME_CHARACTER 0x87
#define MINUS_CHARACTER 0x91
#define KMH_CHARACTER 0xA5
#define DOT_CHARACTER 0x4A

#define SATTELITE_CHARACTER_ASCII (char) 17
#define DESTINATION_CHARACTER_ASCII (char) 18
#define HOME_CHARACTER_ASCII (char) 19
#define KMH_CHARACTER_ASCII (char) 20

class OsdHandler {
    public:
        OsdHandler ();

        void waitForVsyncInactive ();
        void write (int x, int y, char text[], unsigned int textLength);
        void writeString (int x, int y, String text);
        void clear ();
        char mapArrow(double angle);
};

#endif