#include "Gps.h"
#include "Arduino.h"
#include "Coordinate.h"

namespace Gps {
    String time;
    char positionStatus;
    float latitude;
    char latitudeHemisphere;
    float longitude;
    char longitudeHemisphere;
    float speedKN;
    float track;
    String date;
    float magneticVariation;
    char magneticVariationDirection;

    void setup () {}

    /// <summary>
	/// Checks if string contains $GPRMC
	/// </summary>
	/// <param name="str0"></param>
	/// <param name="str1"></param>
	/// <returns>return true or false</returns>

    bool checkGPRMC(String data)
    {
        return str0.indexOf("$GPRMC") >= 0;
    }

    /// <summary>
	/// Read data from GPS as NMEA string
	/// </summary>
	/// <param name="data"></param>
	/// <param name="gpsData"></param>
	/// <returns>NMEA string</returns>

    String readGPRMC()
    {
        String data;
        while (checkGPRMC(data))
        {
            if (Serial1.available())
            {
                data = Serial1.readStringUntil('\n');
            }
        }
        return data;
    }


    /// <summary>
	/// Parse serial data from GPS to GPS values
	/// </summary>
	/// <param name="data"></param>
	/// <returns>Returns GPSData</returns>

    void parse(String data)
    {
        String dataChunks[11];

        data.remove(0, 7); //remove NMEA sentence header

        Serial.println("Data: " + data);

        //divide string into data chunks
        for (int i = 0; i < 11; i++)
        {
            String chunk = data.substring(0, data.indexOf(','));
            dataChunks[i] = chunk;
            data.remove(0, data.indexOf(',') + 1);
        }

        time = !(dataChunks[0] == "") ? dataChunks[0] : EMPTY_STRING;
        positionStatus = !(dataChunks[1] == "") ? dataChunks[1][0] : EMPTY_CHAR;
        latitude = !(dataChunks[2] == "") ? dataChunks[2] : EMPTY_STRING;
        latitudeHemisphere = !(dataChunks[3] == "") ? dataChunks[3][0] : EMPTY_CHAR;
        longitude = !(dataChunks[4] == "") ? dataChunks[4] : EMPTY_STRING;
        longitudeHemisphere = !(dataChunks[5] == "") ? dataChunks[5][0] : EMPTY_CHAR;
        speedKN = !(dataChunks[6] == "") ? dataChunks[6].toFloat() : EMPTY_FLOAT;
        track = !(dataChunks[7] == "") ? dataChunks[7].toFloat() : EMPTY_FLOAT;
        date = !(dataChunks[8] == "") ? dataChunks[8] : EMPTY_STRING;
        magneticVariation = !(dataChunks[9] == "") ? dataChunks[9].toFloat() : EMPTY_FLOAT;
        magneticVariationDirection = !(dataChunks[10] == "") ? dataChunks[10][0] : EMPTY_CHAR;

    }

    Coordinate parseNMEA(NMEACoordinate nmeaCoordinate)
    {
        String nmeaLatitude = nmeaCoordinate.latitude;
        float latitudeDegrees = nmeaLatitude.substring(0, nmeaLatitude.indexOf('.')).toFloat();
        String nmeaLongitude = nmeaCoordinate.longitude;
        float longitudeDegrees = nmeaLongitude.substring(0, nmeaLongitude.indexOf('.')).toFloat();

        nmeaLatitude.remove(0, nmeaLatitude.indexOf('.') + 1);
        nmeaLongitude.remove(0, nmeaLongitude.indexOf('.') + 1);

        float latitudeMinutes = nmeaLatitude.toFloat() / 60;
        float longitudeMinutes = nmeaLongitude.toFloat() / 60;

        float latitude = latitudeDegrees + latitudeMinutes;
        float longitude = longitudeDegrees + longitudeMinutes;

        return
        {
            Gps::latitudeHemisphere == 'N'
                ? latitude
                : -latitude,
            Gps::longitudeHemisphere == 'W'
                ? longitude
                : -longitude
        }
    }

    void poll()
    {
        parse(readGPRMC());
    }
}