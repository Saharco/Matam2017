#ifndef ESCAPETECHNION_H_
#define ESCAPETECHNION_H_

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>
#include "mtm_ex3.h"
#include "list.h"
#include "set.h"
#include "Reservation.h"
#include "Escaper.h"
#include "Room.h"
#include "Company.h"

typedef struct EscapeTechnion_t *EscapeTechnion; //Name of the ADT.

typedef enum {
	//EscapeTechnion's errors;
	ESCAPE_TECHNION_OUT_OF_MEMORY,
	ESCAPE_TECHNION_NULL_PARAMETER,
	ESCAPE_TECHNION_INVALID_PARAMETER,
	ESCAPE_TECHNION_EMAIL_ALREADY_EXISTS,
	ESCAPE_TECHNION_COMPANY_EMAIL_DOES_NOT_EXIST,
	ESCAPE_TECHNION_CLIENT_EMAIL_DOES_NOT_EXIST,
	ESCAPE_TECHNION_ID_ALREADY_EXIST,
	ESCAPE_TECHNION_ID_DOES_NOT_EXIST,
	ESCAPE_TECHNION_CLIENT_IN_ROOM,
	ESCAPE_TECHNION_ROOM_NOT_AVAILABLE,
	ESCAPE_TECHNION_RESERVATION_EXISTS,
	ESCAPE_TECHNION_NO_ROOMS_AVAILABLE,
	ESCAPE_TECHNION_SUCCESS,
} EscapeTechnionResult;

/*
 * Allocates a new EscapeTechnion system.
 * The new system has no companies nor escapers in it, and its current day
 * is set to be 0. Additionally, the default output channel is stdin.
 * #return:
 * 	NULL if an allocation has failed,
 * 	and a new EscapeTechnion system otherwise.
 */
EscapeTechnion escapeTechnionCreate();

/*
 * Creates a copy of the target system.
 * The new system will have the same companies as well as escapers in it,
 * and its current day and output channel are also being copied.
 * #return:
 * 	NULL if a memory allocation has failed or a NULL parameter was given,
 * 	and a new copy of the system otherwise.
 */
EscapeTechnion escapeTechnionCopy(EscapeTechnion sys);

/*
 * Modifies the system's output channel. The previous output channel is being
 * closed (unless it's the default stdin channel).
 * 	$output_file: File stream of the output channel.
 * #return:
 * 	ESCAPE_TECHNION_NULL_PARAMETER - If a NULL parameter was given,
 * 	ESCAPE_TECHNION_SUCCESS - Otherwise.
 */
EscapeTechnionResult escapeTechnionModifyOutput(EscapeTechnion sys,
FILE* output_file);

/*
 * Deallocates a system. Destroys all the companies, rooms, reservations and
 * escapers in the system using their destroyers.
 * #return:
 * 	ESCAPE_TECHNION_NULL_PARAMETER - If a NULL parameter was given,
 * 	ESCAPE_TECHNION_SUCCESS - Otherwise.
 */
EscapeTechnionResult escapeTechnionDestroy(EscapeTechnion sys);

/*
 * Adds a company to the EscapeTechnion system.
 * 	$email: The company's email.
 * 	$faculty: The company's faculty.
 * #return:
 * 	ESCAPE_TECHNION_NULL_PARAMETER - If a NULL parameter was given,
 * 	ESCAPE_TECHNION_INVALID_PARAMETER - If an invalid parameter was given,
 * 	ESCAPE_TECHNION_EMAIL_ALREADY_EXISTS - If that email already exists in the
 * 	system (either by an escaper or a company),
 * 	ESCAPE_TECHNION_SUCCESS - Otherwise.
 */
EscapeTechnionResult escapeTechnionAddCompany(EscapeTechnion sys, char* email,
TechnionFaculty faculty);

/*
 * Removes a company from the EscapeTechnion system.
 * 	$email: The email of the to-be-removed company
 * #return:
 * 	ESCAPE_TECHNION_NULL_PARAMETER - If a NULL parameter was given,
 * 	ESCAPE_TECHNION_INVALID_PARAMETER - If an invalid parameter was given,
 * 	ESCAPE_TECHNION_COMPANY_EMAIL_DOES_NOT_EXIST - If there is no company in the
 * 	system by that email,
 * 	ESCAPE_TECHNION_SUCCESS - Otherwise.
 */
EscapeTechnionResult escapeTechnionRemoveCompany(EscapeTechnion sys,
char* email);

/*
 * Adds a room to one of the EscapeTechnion system's companies.
 * 	$email: The company's email.
 * 	$id: The id of the room.
 * 	$price: The entry fee to this room.
 * 	$num_ppl: The recommended amount of people for this room.
 * 	$working_hours: The scheduled hours of operation for this room. Received
 * 	as a string of the format "xx-yy", where yy>xx and xx>=0, yy<=24.
 * 	$difficulty: The room's difficulty.
 * #return:
 * 	ESCAPE_TECHNION_NULL_PARAMETER - If a NULL parameter was given,
 * 	ESCAPE_TECHNION_INVALID_PARAMETER - If an invalid parameter was given,
 * 	ESCAPE_TECHNION_COMPANY_EMAIL_DOES_NOT_EXIST - If there is no company in the
 * 	system by that email,
 * 	ESCAPE_TECHNION_ID_ALREADY_EXIST - If the desired room id already exists
 * 	in one of the faculties' companies in the system.
 * 	ESCAPE_TECHNION_SUCCESS - Otherwise.
 */
EscapeTechnionResult escapeTechnionAddRoom(EscapeTechnion sys, char* email,
int id, int price, int num_ppl, char* working_hours, int difficulty);

/*
 * Removes a room from the EscapeTechnion system.
 * 	$faculty: The faculty of the company to which this room belogns.
 * 	$id: The room's id.
 * #return:
 * 	ESCAPE_TECHNION_NULL_PARAMETER - If a NULL parameter was given,
 * 	ESCAPE_TECHNION_INVALID_PARAMETER - If an invalid parameter was given,
 * 	ESCAPE_TECHNION_ID_DOES_NOT_EXIST - If there is no room by that id for any
 * 	of the faculty's companies,
 * 	ESCAPE_TECHNION_RESERVATION_EXISTS - If there are unfulfilled reservations
 * 	to this room,
 * 	ESCAPE_TECHNION_SUCCESS - Otherwise.
 */
EscapeTechnionResult escapeTechnionRemoveRoom(EscapeTechnion sys,
TechnionFaculty faculty, int id);

/*
 * Adds an escaper the EscapeTechnion system.
 * 	$email: The escaper's email.
 * 	$faculty: The faculty to which this escaper belongs.
 * 	$skill_level: The skill level of this escaper.
 * #return:
 * 	ESCAPE_TECHNION_NULL_PARAMETER - If a NULL parameter was given,
 * 	ESCAPE_TECHNION_EMAIL_ALREADY_EXISTS - If that email already exists in the
 * 	system (either by an escaper or a company),
 * 	ESCAPE_TECHNION_SUCCESS - Otherwise.
 */
EscapeTechnionResult escapeTechnionAddEscaper(EscapeTechnion sys, char* email,
TechnionFaculty faculty, int skill_level);

/*
 * Removes an escaper from the EscapeTechnion system.
 * 	$email: The email of the to-be-removed escaper.
 * #return:
 * 	ESCAPE_TECHNION_NULL_PARAMETER - If a NULL parameter was given,
 * 	ESCAPE_TECHNION_INVALID_PARAMETER - If an invalid parameter was given,
 * 	ESCAPE_TECHNION_CLIENT_EMAIL_DOES_NOT_EXIST - If there is no escaper in the
 *  system by that email,
 * 	ESCAPE_TECHNION_SUCCESS - Otherwise.
 */
EscapeTechnionResult escapeTechnionRemoveEscaper(EscapeTechnion sys,
char* email);

/*
 * Creates a new reservation to a room by an escaper in the system.
 * 	$email: The email of the escaper.
 * 	$faculty: The faculty to which the desired room belongs.
 * 	$id: The id of the desired room.
 * 	$time: The time of the reservation. Received as a string of the format
 * 	"dd-hh", where day>=0 and hour is between 0 and 23. The time has to be
 * 	within the open and close time of the desired room.
 * 	$num_ppl: The amount of people for which this reservation is made.
 * #return:
 * 	ESCAPE_TECHNION_NULL_PARAMETER - If a NULL parameter was given,
 * 	ESCAPE_TECHNION_INVALID_PARAMETER - If an invalid parameter was given,
 * 	ESCAPE_TECHNION_CLIENT_EMAIL_DOES_NOT_EXIST - If there is no escaper in
 * 	the system by that email,
 * 	ESCAPE_TECHNION_ID_DOES_NOT_EXIST - If there is no room in any of the
 * 	faculty's companies by that id.
 * 	ESCAPE_TECHNION_CLIENT_IN_ROOM - If the escaper already has a reservation
 * 	for a room at that time,
 * 	ESCAPE_TECHNION_SUCCESS - Otherwise.
 */
EscapeTechnionResult escapeTechnionEscaperOrder(EscapeTechnion sys, char* email,
TechnionFaculty faculty, int id, char* time, int num_ppl);

/*
 * Creates a new reservation to a room by an escaper in the system.
 * The room is being automatically decided by the system, according to the best
 * match available, and at the earliest time available (for both the room and
 * the escaper).
 * 	$email: The email of the escaper.
 * 	$num_ppl: The amount of people for which this reservation is made.
 * #return:
 * 	ESCAPE_TECHNION_NULL_PARAMETER - If a NULL parameter was given,
 * 	ESCAPE_TECHNION_INVALID_PARAMETER - If an invalid parameter was given,
 * 	ESCAPE_TECHNION_CLIENT_EMAIL_DOES_NOT_EXIST - If there is no escaper in
 * 	the system by that email,
 * 	ESCAPE_TECHNION_NO_ROOMS_AVAILABLE - If there are no rooms in the system,
 * 	ESCAPE_TECHNION_SUCCESS - Otherwise.
 */
EscapeTechnionResult escapeTechnionEscaperRecommend(EscapeTechnion sys,
char* email, int num_ppl);

/*
 * Fulfills all of today's reservations, and prints them to the system's
 * output channel. The order of printing: By hour->By faculty->By id
 * (from smallest to biggest). The system's day is then moved by one.
 */
EscapeTechnionResult escapeTechnionReportDay(EscapeTechnion sys);

/*
 * Prints the top 3 faculties in the system (the faculties that earned the most
 * money) to the system's output channel. The order of printing:
 * By money(from biggest to smallest) -> By faculty(From smallest to biggest).
 */
EscapeTechnionResult escapeTechnionReportBest(EscapeTechnion sys);

#endif /* ESCAPETECHNION_H_ */
