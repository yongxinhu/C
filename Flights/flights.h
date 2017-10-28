#ifndef FLIGHTS_H
#define FLIGHTS_H

#include <stdbool.h>
#include "timeHM.h"

typedef struct flightSys flightSys_t;
typedef struct airport airport_t;
typedef struct flight flight_t;

flightSys_t* createSystem();
flight_t* createFlight(airport_t* dest, timeHM_t dep, timeHM_t arr, int c);
void deleteSystem(flightSys_t* s);
void addAirport(flightSys_t* s, char* name);
airport_t* getAirport(flightSys_t* s, char* name);
void printAirports(flightSys_t* s);
void addFlight(airport_t* src, airport_t* dst, timeHM_t* departure, timeHM_t* arrival, int cost);
void printSchedule(airport_t* s);
bool getNextFlight(airport_t* src, airport_t* dst, timeHM_t* now, timeHM_t* departure, timeHM_t* arrival, int* cost);
int validateFlightPath(flight_t** flight_list, char** airport_name_list, int sz);

#endif
