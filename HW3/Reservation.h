#ifndef RESERVATION_H_
#define RESERVATION_H_

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "mtm_ex3.h"

typedef struct Reservation_t *Reservation; //Name of the ADT.

typedef enum {
	//The reservation's errors:
	RESERVATION_NULL_PARAMETER,
	RESERVATION_INVALID_PARAMETER,
	RESERVATION_OUT_OF_MEMORY,
	RESERVATION_SUCCESS,
} ReservationResult;

//The return value of the integer getter functions
#define RESERVATION_ILLEGAL -1

//We demand that the entry fee of a room id divisible by MULTIPLE
#define MULTIPLE 4

//Used in order to determine the 25% possible discount to a reservation's price.
//Assumption is that a room's entry fee is divisible by 4
#define DISCOUNT_PARAMETER 75

/*
 * Allocates a new reservation.
 * Creates a new reservation and initializes its fields according to the
 * parameters given. The required fields are:
 *  $result: Holds the result of the create function. Can be sent as NULL if
 *  the result of this function is not needed.
 * 	$email: Pointer to the email string of the customer who made this
 * 	reservation. Must contain '@'.
 * 	$id: Id of the desired room for this reservation. Must be greater than 0.
 * 	$faculty: The faculty to which the customer belongs.
 * 	$room_price: The original entry fee of the room.
 * 	$room_open: The time when the room opens for business.
 * 	$room_close: The time when the room closes.
 * 	$num_of_people: The amount of people for this reservation. Must be greater
 * 	than 0.
 * 	$date: The scheduled time for the reservation. The time is a string of the
 * 	format: "dd-hh", where the first two digits represent how many days from now
 * 	the reservation is for, and the the last two digits represent the hour of
 * 	the day for the reservation. Days must be non-negative, hour is between 0
 * 	and 23, but has to be during the time when the escape room is open: more or
 * 	equal to the open time, and less than the close time.
 * 	$has_discount: a flag that represents whether or not the escaper deserves
 * 	a discount for this reservation. The escaper gets a discount iff he's a
 * 	member of the same faculty to which this room belongs.
 * #return:
 * 	NULL if one of the parameters is invalid or an allocation fails,
 * 	and a new reservation otherwise.
 */
Reservation reservationCreate(ReservationResult *result, char* email, int id,
TechnionFaculty faculty, int room_price, int room_open, int room_close,
int num_of_people, char* date, bool has_discount);

/*
 * Deallocates a reservation.
 * #return:
 * 	RESERVATION_NULL_PARAMETER - if a NULL pointer was sent.
 * 	RESERVATION_SUCCESS - otherwise.
 */
ReservationResult reservationDestroy(Reservation reservation);

/*
 * Creates a copy of the target reservation.
 * The new copy will contain all of the values of the source reservation.
 * #return:
 * 	NULL if a NULL pointer was sent or a memory allocation failed,
 * 	and a new copy of the reservation otherwise.
 */
Reservation reservationCopy(Reservation reservation);

/*
 * Decreases the reservation's scheduled day by one.
 * #Return:
 * 	RESERVATION_NULL_PARAMETER - if a NULL pointer was sent.
 * 	RESERVATION_SUCCESS - otherwise.
 */
ReservationResult reservationNextDay(Reservation reservation);

/*
 * Checks if a reservation is scheduled for today, that is: its got 0 days left.
 * #return:
 * 	false - if a NULL pointer was sent, or if the reservation is not for today.
 * 	true - otherwise.
 */
bool reservationIsToday(Reservation reservation);

/*
 * Returns the total amount of hours left for the reservation.
 * Note: a day is composed of 24 hours.
 * #return:
 * 	RESERVATION_NULL_PARAMETER - if a NULL pointer was sent,
 * 	and the total amount of hours left for the reservation otherwise.
 */
int reservationHoursLeft(Reservation reservation);

/*
 * Returns the reservation's scheduled hour.
 * #return:
 * 	RESERVATION_NULL_PARAMETER - if a NULL pointer was sent,
 * 	and the scheduled hour of the reservation otherwise.
 */
int reservationGetHour(Reservation reservation);

/*
 * Returns the reservation's scheduled day.
 * #return:
 * 	RESERVASION_NULL_PARAMETER - if a NULL pointer was sent,
 * 	and the scheduled day of the reservation otherwise.
 */
int reservationGetDay(Reservation reservation);

/*
 * Returns the reservation's price.
 * #return:
 * 	RESERVATION_NULL_PARAMETER - if a NULL pointer was sent,
 * 	and the price of the reservation otherwise.
 */
int reservationGetPrice(Reservation reservation);

/*
 * Returns the amount of people in this reservation.
 * #return:
 * 	RESERVATION_NULL_PARAMETER - if a NULL pointer was sent,
 * 	and the amount of people for the reservation otherwise.
 */
int reservationGetPeople(Reservation reservation);

/*
 * Returns the id of the room for which this reservation was made
 * (in a parameter address!).
 * #return:
 * 	RESERVATION_NULL_PARAMETER - if a NULL pointer was sent.
 * 	RESERVATION_SUCCESS - otherwise.
 */
ReservationResult reservationGetRoomId(Reservation reservation, int* id);

/*
 * Returns the email of the escaper to whom this reservation belongs to
 * (in a parameter address!).
 * #return:
 * 	RESERVATION_NULL_PARAMETER - if a NULL pointer was sent.
 * 	RESERVATION_SUCCESS - otherwise.
 */
ReservationResult reservationGetEscaperEmail(Reservation reservation,
char** email);

/*
 * Returns whether or not the reservation is discounted off its' original price
 * due to the customer being a member of the faculty to which the room belongs.
 * #return:
 * 	false - if a NULL pointer was sent or if the reservation isn't discounted,
 * 	true - the reservation is discounted.
 */
bool reservationIsDiscounted(Reservation reservation);

/*
 * Checks if the reservation is still valid (was not manually invalidated).
 * #return:
 * 	true - If the reservation is valid,
 * 	false - Otherwise.
 */
bool reservationIsValid(Reservation reservation);

/*
 * Invalidates a reservation (assigns invalid data to it).
 * #return:
 * 	RESERVATION_NULL_PARAMETER - if a NULL pointer was sent.
 * 	RESERVATION_SUCCESS - otherwise.
 */
ReservationResult reservationInvalidate(Reservation reservation);

#endif /* RESERVATION_H_ */
