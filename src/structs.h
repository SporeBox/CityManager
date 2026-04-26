#ifndef STRUCTS_H
#define STRUCTS_H

typedef struct {
    int day;
    int month;
    int year;
    int hour;
    int minute;
} Timestamp;

typedef struct {
    float latitude;
    float longitude;
} Coord;

typedef struct {
    int    rapID;
    char   inspName[30];
    Coord  coordinates;
    char   issue[30];
    int    sevLvl;
    Timestamp time;
    char   description[200];
} Report;

#endif
