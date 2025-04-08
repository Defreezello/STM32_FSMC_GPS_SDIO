// gps.h
#ifndef GPS_H
#define GPS_H

#include <stdint.h>

typedef struct {
    float latitude;
    float longitude;
    float speed;
    int satellites;
    char valid;
    int hour;
    int minutes;
    int seconds;
    int date;
    int month;
    int year;
} GPS_Data;


char GPS_ProcessChar(char *nmeaBuffer);
GPS_Data* GPS_GetData(void);

#endif
