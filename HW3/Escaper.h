#ifndef ESCAPER_H_
#define ESCAPER_H_

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include "set.h"
#include "Reservation.h"
#include "Room.h"
#include "mtm_ex3.h"

typedef struct Escaper_t *Escaper; //Name of the ADT.

typedef enum {
	//The escaper's errors;
	ESCAPER_SUCCESS,
	ESCAPER_OUT_OF_MEMORY,
	ESCAPER_NULL_PARAMETER,
	ESCAPER_INVALID_PARAMETER,
	ESCAPER_RESERVED_ROOM,
	ESCAPER_IN_ROOM,
} EscaperResult;


/*
 * Allocates a new escaper.
 * Creates a new escaper and initializes their fields according to the
 * parameters given.
 * The required fields are:
 * 	$result: Holds the result of the create function. Can be sent as NULL if
 *  the result of this function is not needed.
 * 	$email: The customer's email string. Must contain '@' exactly once.
 * 	$faculty: The faculty that the escaper belongs to.
 * 	$skill_level: The escaper's skill level. Must be between 1 and 10.
 * #return:
 * 	NULL - If one of the parameters is invalid or an allocation fails,
 * 	and a new escaper otherwise. The new escaper has no reservations.
 */
Escaper escaperCreate(EscaperResult *result, char* email,
TechnionFaculty faculty, int skill_level);

/*
 * Deallocates an escaper. Clears all the reservations of the escaper by using
 * the reservations' destroyer.
 * #return:
 * 	ESCAPER_NULL_PARAMETER - If a NULL pointer was sent.
 * 	ESCAPER_SUCCESS - Otherwise.
 */
EscaperResult escaperDestroy(Escaper escaper);

/*
 * Creates a copy of the target escaper.
 * The new copy will contain all the values of the source escaper's fields.
 * #return:
 * 	NULL if a NULL pointer was sent or a memory allocation failed,
 * 	and a new copy of the escaper otherwise.
 */
Escaper escaperCopy(Escaper escaper);

/*
 * Returns the escaper's email (in a parameter address).
 * 	$email: Address of a string that will store the email.
 * #return:
 * 	ESCAPER_NULL_PARAMETER - if a NULL pointer was sent,
 * 	ESCAPER_SUCCESS - otherwise.
 */
EscaperResult escaperGetEmail(Escaper escaper, char** email);

/*
 * Returns the escaper's faculty (in a parameter address).
 * 	$faculty: Address of the faculty enumerated integer that will store the
 * 	faculty
 * #return:
 *  ESCAPER_NULL_PARAMETER - If a NULL pointer was sent,
 * 	ESCAPER_SUCCESS - Otherwise.
 */
EscaperResult escaperGetFaculty(Escaper escaper, TechnionFaculty* faculty);

/*
 * Returns the escaper's skill level (in a parameter address).
 * 	$skill_level: Address of a value that will store the skill level.
 * #return:
 * 	ESCAPER_NULL_PARAMETER - If a NULL pointer was sent.
 * 	ESCAPER_SUCCESS - Otherwise.
 */
EscaperResult escaperGetSkillLevel(Escaper escaper, int* skill_level);

/*
 * Returns the escaper's amount of reservations (in a parameter address).
 * 	$amount: Address of a value that will store the amount of reservations.
 * #return:
 * 	ESCAPER_NULL_PARAMETER - If a NULL pointer was sent.
 * 	ESCAPER_SUCCESS - Otherwise.
 */
EscaperResult escaperReservationsAmount(Escaper escaper, int* amount);

/*
 * The escaper schedules a new reservation to a room.
 * The reservation is taken into account by priority in the following way:
 * The escaper potentially has more urgent reservations
 * (reservations that should be implemented first = less days left / smaller
 * hour value), so the new reservation for the escaper will be added before or
 * after certain reservations, according to whichever comes first.
 * 	$room: The room to which the escaper wants to order a reservation for.
 * 	$faculty: The faculty of the company that owns the desired room.
 * 	$num_of_people: The amount of people for this reservation.
 * 	$date: The scheduled time for the reservation. The time is a string of the
 * 	format: "dd-hh", where the first two digits represent how many days from now
 * 	the reservation is for, and the the last two digits represent the hour of
 * 	the day for the reservation. Days must be non-negative, hour is between 0
 * 	and 23, but has to be during the time when the escape room is open: more or
 * 	equal to the room's open time, and less than the room's close time.
 * #return:
 * 	ESCAPER_NULL_PARAMETER - If a NULL pointer was sent,
 * 	ESCAPER_IN_ROOM - If the escaper has already ordered a reservation to the
 * 	same time.
 * 	ESCAPER_OUT_OF_MEMORY - If an allocation has failed.
 * 	ESCAPER_RESERVED_ROOM - If the desired room is already reserved at that time
 * 	ESCAPER_SUCCESS - Otherwise.
 */
EscaperResult escaperMakeReservation(Escaper escaper, Room room,
TechnionFaculty faculty, int num_of_people, char* date);

/*
 * Decreases a full day's worth of hours (24) from the scheduled time of all the
 * customer's reservations. Reservations for today will be fulfilled.
 * #return:
 *  ESCAPER_NULL_PARAMETER - If a NULL pointer was sent.
 * 	ESCAPER_SUCCESS - Otherwise.
 */
EscaperResult escaperNextDay(Escaper escaper);

//DOCUMENT THIS!!!
bool escaperIsBusy(Escaper escaper, int hours);

/*
 * Removes a scheduled reservation for this escaper.
 *	$hours: the scheduled hour for the reservation we are trying to remove.
 * #return:
 *	ESCAPER_NULL_PARAMETER - If a NULL pointer was sent.
 *	ESCAPER_INVALID_PARAMETER - If there is no reservation for the desired hour,
 *	ESCAPER_SUCCESS - Otherwise.
 */
EscaperResult escaperRemoveReservation(Escaper escaper, int hours);

#endif /* ESCAPER_H_ */
