// gps.c
#include "gps.h"
#include <string.h>
#include <stdlib.h>

#define NMEA_BUFFER_SIZE 128

static GPS_Data gpsData = {0};
int UTC = 3;

// conversion of coordinates to degrees
float ParseCoordinate(char* coord, char direction) {	//static
    float degrees = (coord[0] - '0') * 10 + (coord[1] - '0');
    float minutes = atof(&coord[2]);
    float decimal = degrees + minutes / 100.0f; //	 60.0f; - to degrees
    
    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }
    
    return decimal;
}

char GPS_ProcessChar(char *nmeaBuffer) {

        //nmeaBuffer[bufferIndex] = '\0';
            char *token;
            uint8_t tokenIndex = 0;
            
            // search comma in packet
            token = strtok(nmeaBuffer, ",");
            while (token != NULL) {
                switch(tokenIndex) {
                    case 1: // Time
                        if (strlen(token) >= 6) {
                            gpsData.hour = ((token[0] - '0') * 10 + (token[1] - '0') + 3);
                            if(gpsData.hour >= 24)
                            	gpsData.hour = (gpsData.hour - 24);
                            gpsData.minutes = (token[2] - '0') * 10 + (token[3] - '0');
                            gpsData.seconds = (token[4] - '0') * 10 + (token[5] - '0');
                        }
                        break;
                    case 2: // State
                        gpsData.valid = (token[0]);
                        break;
                    case 3: // Latitude
                        if (strlen(token) > 0) {
                            char nextToken = *(token + strlen(token) + 1);
                            gpsData.latitude = ParseCoordinate(token, nextToken);
                        }
                        break;
                    case 5: // Longitude
                        if (strlen(token) > 0) {
                            char nextToken = *(token + strlen(token) + 1);
                            gpsData.longitude = ParseCoordinate(token + 1, nextToken);
                        }
                        break;
                    case 7: // speed
                        if (strlen(token) > 0) {
                            gpsData.speed = atof(token) * 1.852f; // converting to km/h
                        }
                        break;
                    case 8: // Date
                    	if (strchr(token, '.'))
                    	break;
                        if (strlen(token) >= 6) {
                        	gpsData.date = (token[0] - '0') * 10 + (token[1] - '0');
                        	gpsData.month = (token[2] - '0') * 10 + (token[3] - '0');
                        	gpsData.year = (token[4] - '0') * 10 + (token[5] - '0');
                        	tokenIndex++;
                         }
                        break;
                    case 9: // Date
                        if (strlen(token) >= 6) {
                            gpsData.date = (token[0] - '0') * 10 + (token[1] - '0');
                            gpsData.month = (token[2] - '0') * 10 + (token[3] - '0');
                            gpsData.year = (token[4] - '0') * 10 + (token[5] - '0');
                         }
                        break;
                }
                token = strtok(NULL, ",");
                tokenIndex++;
            }
return gpsData.valid;
}


GPS_Data* GPS_GetData(void) {
    return &gpsData;
}

