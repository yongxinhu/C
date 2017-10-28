/* 
 * CS61C Summer 2013
 * Name:
 * Login:
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "flights.h"
#include "timeHM.h"

#define class(name) \
typedef struct _##name name;\
struct _##name

struct _airportNode;
struct _flightNode;

struct flightSys {
    // Place the members you think are necessary for the flightSys struct here.
    struct _airportNode *airports;
};

struct airport {
    // Place the members you think are necessary for the airport struct here.
    char *name;
    struct _flightNode *flights;
};

struct flight {
    // Place the members you think are necessary for the flight struct here.
    airport_t *des;
    timeHM_t dep;
    timeHM_t arr;
    int cost;
};

/*
 This should be called if memory allocation failed.
 */
static void allocation_failed() {
    fprintf(stderr, "Out of memory.\n");
    exit(EXIT_FAILURE);
}

/*
	flightNode
*/

class(flightNode) {
	flight_t *f;
	flightNode *next;
};
flightNode *newFlightNode(flight_t *f) {
	flightNode *node = malloc(sizeof(flightNode));
    if (node == NULL){
        allocation_failed();
    }
	node->f = f;
	node->next = NULL;
	return node;
}
void deleteFlightNode(flightNode *self) {
	flightNode *next;
	while(self != NULL) {
		next = self->next;
		free(self->f);
		free(self);
		self = next;
	}
}
/*
	airportNode
*/
class(airportNode) {
	airport_t *ap;
	airportNode *next;
};
airportNode *newAirportNode(airport_t *airp) {
	airportNode *node = malloc(sizeof(airportNode));
    if (node == NULL){
        allocation_failed();
    }
	node->ap = airp;
	node->next = NULL;
	return node;
}
void deleteAirportNode(airportNode *self) {
	airportNode *next;
	while(self != NULL) {
		next = self->next;
		free(self->ap->name);
		deleteFlightNode(self->ap->flights);
		free(self->ap);
		free(self);
		self = next;
	}
}



/*
   Creates and initializes a flight system, which stores the flight schedules of several airports.
   Returns a pointer to the system created.
 */
flightSys_t* createSystem() {
    // Replace this line with your code
    flightSys_t *lpSys = malloc(sizeof(flightSys_t));
    if (lpSys == NULL){
        allocation_failed();
    }
    lpSys->airports = NULL;
    return lpSys;
}


/* Given a destination airport, a departure and arrival time, and a cost, return a pointer to new flight_t. Note that this pointer must be available to use even
   after this function returns. (What does this mean in terms of how this pointer should be instantiated)?
*/

flight_t* createFlight(airport_t* dest, timeHM_t dep, timeHM_t arr, int c) {
   // Replace this line with your code
    flight_t *f = (flight_t*)malloc(sizeof(flight_t));
    if (f == NULL) {
        allocation_failed();
    }
    f->des = dest;
    f->dep = dep;
    f->arr = arr;
    f->cost = c;
    return f;
}

/*
   Frees all memory associated with this system; that's all memory you dynamically allocated in your code.
 */
void deleteSystem(flightSys_t* s) {
    // Replace this line with your code
    if(s == NULL) return;
    deleteAirportNode(s->airports);
    free(s);
}


/*
   Adds a airport with the given name to the system. You must copy the string and store it.
   Do not store "name" (the pointer) as the contents it point to may change.
 */
void addAirport(flightSys_t* s, char* name) {
    if (s == NULL || name == NULL) return;
    airport_t* n = (airport_t*)malloc(sizeof(airport_t));
    if (n == NULL) {
        allocation_failed();
    }
    strcpy(n->name, name);
    n->flights = NULL;
    airportNode* apn = newAirportNode(n);
    if (s->airports == NULL){
        s->airports = apn;
    }else{
        airportNode* p= s->airports;
        while(p != NULL){
            p = p->next;
        }
        p->next = apn;
    }
    
}


/*
   Returns a pointer to the airport with the given name.
   If the airport doesn't exist, return NULL.
 */
airport_t* getAirport(flightSys_t* s, char* name) {
    // Replace this line with your code
    if(s==NULL || name == NULL) return NULL;
    airportNode *p = s->airports;
    while(p != NULL) {
    	if(strcmp(name,p->ap->name)) {
    		return p->ap;
		}
	}
    return NULL;
}


/*
   Print each airport name in the order they were added through addAirport, one on each line.
   Make sure to end with a new line. You should compare your output with the correct output
   in flights.out to make sure your formatting is correct.
 */
void printAirports(flightSys_t* s) {
    if(s==NULL || s->airports == NULL) return;
    airportNode* n = s -> airports;
    do {
        printf("%s\n",n->ap->name);
        n = n-> next;
    } while (n!= NULL);
}


/*
   Adds a flight to src's schedule, stating a flight will leave to dst at departure time and arrive at arrival time.
 */
void addFlight(airport_t* src, airport_t* dst, timeHM_t* departure, timeHM_t* arrival, int cost) {
    // Replace this line with your code
    flight_t *fli = createFlight(dst,*departure,*arrival, cost);
    flightNode *node = newFlightNode(fli);
    node->next = src->flights;
    src->flights = node;
}


/*
   Prints the schedule of flights of the given airport.

   Prints the airport name on the first line, then prints a schedule entry on each 
   line that follows, with the format: "destination_name departure_time arrival_time $cost_of_flight".

   You should use printTime (look in timeHM.h) to print times, and the order should be the same as 
   the order they were added in through addFlight. Make sure to end with a new line.
   You should compare your output with the correct output in flights.out to make sure your formatting is correct.
 */
void printSchedule(airport_t* s) {
    if(s==NULL) return;
    printf("%s\n",s->name);
    flightNode* fn = s->flights;
    while (fn!= NULL) {
        printf("%s",fn->f->des->name);
        printf(" ");
        printTime(&(fn->f->dep));
        printf(" ");
        printTime(&(fn->f->arr));
        printf(" ");
        printf("%d\n",fn->f->cost);
        fn = fn->next;
    }
    // Replace this line with your code
}


/*
   Given a src and dst airport, and the time now, finds the next flight to take based on the following rules:
   1) Finds the cheapest flight from src to dst that departs after now.
   2) If there are multiple cheapest flights, take the one that arrives the earliest.

   If a flight is found, you should store the flight's departure time, arrival time, and cost in departure, arrival, 
   and cost params and return true. Otherwise, return false. 

   Please use the function isAfter() from time.h when comparing two timeHM_t objects.
 */
bool getNextFlight(airport_t* src, airport_t* dst, timeHM_t* now, timeHM_t* departure, timeHM_t* arrival, int* cost) {
    // Replace this line with your code
    if(src == NULL || dst == NULL) return false;
    flight_t *goodFlight = NULL;
    flightNode *p = src->flights;
    while(p != NULL) {
    	if(p->f->des == dst && isAfter(&(p->f->dep),now)) {
    		if(goodFlight == NULL) {
    			goodFlight = p->f;
			} else if(p->f->cost < goodFlight->cost) {
				goodFlight = p->f;
			} else if(p->f->cost == goodFlight->cost && isAfter(&(goodFlight->arr),&(p->f->arr))) {
				goodFlight = p->f;
			}
		}
    	p = p->next;
	}
    return false;
}

/* Given a list of flight_t pointers (flight_list) and a list of destination airport names (airport_name_list), first confirm that it is indeed possible to take these sequences of flights,
   (i.e. be sure that the i+1th flight departs after or at the same time as the ith flight arrives) (HINT: use the isAfter and isEqual functions).
   Then confirm that the list of destination airport names match the actual destination airport names of the provided flight_t struct's.
   sz tells you the number of flights and destination airport names to consider. Be sure to extensively test for errors (i.e. if you encounter NULL's for any values that you might expect to
   be non-NULL, return -1).

   Return from this function the total cost of taking these sequence of flights. If it is impossible to take these sequence of flights, if the list of destination airport names
   doesn't match the actual destination airport names provided in the flight_t struct's, or if you run into any errors mentioned previously or any other errors, return -1.
*/
int validateFlightPath(flight_t** flight_list, char** airport_name_list, int sz) {
    // Replace this line with your code
    if (flight_list == NULL || airport_name_list == NULL || sz == 0){
        return -1;
    }
    flight_t** temp = flight_list;
    char** atemp = airport_name_list;
    int tcost = 0;
    for (int i = 0; i < sz; i++) {
        flight_t* first = *temp;
        char* fairport = *atemp;
        if (first == NULL||fairport == NULL) {
            return -1;
        }
        if (strcmp(first->des->name, fairport)){
            return -1;
        }
        flight_t* second = *(temp+1);
        char* sairport = *(atemp+1);
        if (second == NULL || sairport == NULL){
            return -1;
        }
        if(isEqual(&(second->dep),&(first->arr))||isAfter(&(second->dep),&(first->arr))){
            return -1;
        }
        tcost = tcost + (first->cost);
        temp = temp + 1;
        atemp = atemp + 1;
    }
    return tcost;
    
}






