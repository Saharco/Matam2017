#ifndef COMPANY_H_
#define COMPANY_H_

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "set.h"
#include "Room.h"
#include "Escaper.h"
#include "Reservation.h"
#include "mtm_ex3.h"

typedef struct Company_t *Company; //Name of the ADT.

typedef enum {
	//The company's errors;
	COMPANY_NULL_PARAMETER,
	COMPANY_ROOM_ID_EXISTS,
	COMPANY_INVALID_PARAMETER,
	COMPANY_OUT_OF_MEMORY,
	COMPANY_ROOM_HAS_RESERVATIONS,
	COMPANY_ROOM_NOT_FOUND,
	COMPANY_ILLEGAL_ID,
	COMPANY_SUCCESS,
} CompanyResult;

/*
 * Allocates a new company.
 * Creates a new company and initializes its fields according to the parameters
 * The required fields are:
 *  $result: Holds the result of the create function. Can be sent as NULL if
 *  the result of this function is not needed.
 * 	$email: The company's email. Must contain exactly one '@'.
 * 	$faculty: The faculty to which the company belongs.
 * #return:
 * 	NULL if one of the parameters is invalid or an allocation has failed,
 * 	and a new company otherwise. The new company has no rooms.
 */
Company companyCreate(CompanyResult* result, char* email,
TechnionFaculty faculty);
/*
 * Deallocates a company. Clears all of the company's rooms' reservations and
 * the rooms, according to their destroyers.
 * #return:
 * 	COMPANY_NULL_PARAMETER - If a NULL pointer was sent.
 * 	COMPANY_SUCCESS - Otherwise.
 */
CompanyResult companyDestroy(Company company);

/*
 * Creates a copy of the target company.
 * The new copy will belong to the same faculty and have the same email address
 * as the original company, as well as the exact same rooms belonging to it.
 * #return:
 * 	COMPANY_NULL_PARAMETER - If a NULL pointer was sent.
 * 	COMPANY_SUCCESS - Otherwise.
 */
Company companyCopy(Company company);

/*
 * Gets the company's email.
 * 	$email: Address where the email will be stored.
 * #return:
 * 	COMPANY_NULL_PARAMETER - If a NULL pointer was sent.
 * 	COMPANY_SUCCESS - Otherwise.
 */
CompanyResult companyGetEmail(Company company, char** email);

/*
 * Gets the faculty to which the company belongs.
 * 	$faculty: Address where the faculty will be stored.
 * #return:
 * 	COMPANY_NULL_PARAMETER - If a NULL pointer was sent.
 * 	COMPANY_SUCCESS - Otherwise.
 */
CompanyResult companyGetFaculty(Company company, TechnionFaculty* faculty);

/*
 * Gets the total amount of money earned by all of this company's rooms.
 * #return:
 *  0 - If a NULL parameter was sent, or if there was no money earned.
 *  All of the company's rooms' earns otherwise.
 */
int companyEarns(Company company);

/*
 * Adds a new room to a company's possession. The new room will have no
 * reservations yet. The required fields for initialization:
 * 	$id: The room's id. Must be a positive number and unique (no rooms in this
 * 	company share this id).
 * 	$entry_fee: The room's entry fee. Must be non-negative and divisible by 4.
 * 	$recommended_people: The amount of people recommended for the room. Must be
 * 	a positive number.
 * 	$difficulty: The room's difficulty. Must be between 1 and 10 (included).
 * 	$working_hours: The room's scheduled working hours. Received as a string of
 * 	the following format: "xx-yy". xx and yy must be between 0 and 23,
 * 	where yy>xx.
 * #return:
 * 	COMPANY_NULL_PARAMETER - If a NULL pointer was sent,
 * 	COMPANY_ILLEGAL_PARAMETER - If one of the parameters given does not meet the
 * 	requirements.
 * 	COMPANY_OUT_OF_MEMORY - If an allocation has failed,
 * 	COMPANY_ROOM_ID_EXISTS - If there is already a room in the company with the
 * 	same id as the id given.
 * 	COMPANY_SUCCESS - Otherwise.
 */
CompanyResult companyNewRoom(Company company, int id, int entry_fee,
int recommended_people, int difficulty, char* working_hours);

/*
 * Deallocates and removes a room from the company. The room must have no
 * reservations in order for it to be removed successfully.
 * 	$id: The id of the required room.
 * #return:
 * 	COMPANY_NULL_PARAMETER - If a NULL pointer was sent,
 * 	COMPANY_ROOM_HAS_RESERVATIONS - If the required room has unfulfilled
 * 	reservations
 * 	COMPANY_ILLEGAL_ID - If there's no room in the company with the required id,
 * 	COMPANY_SUCCESS - Otherwise.
 */
CompanyResult companyRemoveRoom(Company company, int id);

/*
 * Decreases the scheduled days of all the reservations of all the rooms in the
 * company by one, and fulfills all the reservations that were for today.
 * #return:
 * 	COMPANY_NULL_PARAMETER - If a NULL pointer was sent.
 * 	COMPANY_SUCCESS - Otherwise.
 */
CompanyResult companyNextDay(Company company);

/*
 * Returns the amount of reservations in all of the company's rooms that are
 * scheduled for today.
 * #return:
 *  0 - If a NULL parameter was sent or there were no reservations for today.
 *  Amount of today's reservations otherwise.
 */
int companyTodaysReservations(Company company);

/*
 * Gets the amount of rooms that belong to this company.
 * 	$size: Address where the amount of rooms will be stored.
 * #return:
 * 	COMPANY_NULL_PARAMETER - If a NULL pointer was sent.
 * 	COMPANY_SUCCESS - Otherwise.
 */
CompanyResult companyRoomsAmount(Company company, int* size);

/*
 * Gets a room in the company's possession (in a parameter address).
 * 	$room: Address where the room will be stored.
 * 	$id: The id of the room in this company.
 * #return:
 * 	COMPANY_NULL_PARAMETER - If a NULL pointer was sent,
 * 	COMPANY_ILLEGAL_ID - If there is no room in the company by that id,
 * 	COMPANY_SUCCESS - Otherwise.
 */
CompanyResult companyGetRoom(Company company, Room* room, int id);

/*
 * checks if there are reservations to the company.
 * #return:
 *  false - If a NULL parameter was sent or there are no reservations
 *  for the company.
 *  true - If a reservation was found for any of the company's rooms
 */
bool companyRoomsReserved(Company company);

/*
 * Returns the room in this company with the lowest id.
 * 	$desired_id: Address that will store the id of the room with the lowest id.
 * 	If no acceptable rooms were found - its assigned with a negative value
 * #return:
 * 	NULL - If a NULL pointer was sent, or no rooms exist in this company,
 * 	address of the desired room otherwise.
 */
Room companyLowestIdRoom(Company company, int* desired_id);

/*
 * Returns the room in this company with the lowest id that's bigger than a
 * parameter target id.
 * 	$id: Pointer that holds the value of the lower boundary of acceptable room
 * 	ids (no rooms with ids smaller than this (or equal) will be returned).
 * 	If no acceptable rooms were found - its assigned with a negative value
 * #return:
 * 	NULL - If a NULL pointer was sent, or no rooms exist in this company that
 * 	have a higher id than the given parameter id,
 * 	address of the desired room otherwise.
 */
Room companyNextRoom(Company company, int* id);

/*
 * Macro for iterating over all the rooms that belong to this company.
 * Assumption is that 'current_room' is of type 'Room' and 'company' is of type
 * 'Company'. The current_room parameter is the iterator for all the rooms.
 * The iterator Iterates over the list in the order of ids (from smallest to
 * biggest in this company).
 */
#define COMPANY_FOREACH(current_room, company, id) \
	for(Room current_room = companyLowestIdRoom(company, &id); \
		current_room != NULL; \
		current_room = companyNextRoom(company, &id))

#endif /* COMPANY_H_ */
