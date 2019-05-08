#ifndef ROOM_H_
#define ROOM_H_

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "list.h"
#include "Reservation.h"

typedef struct Room_t *Room; //Name of the ADT.

typedef enum {
	//The room's errors:
	ROOM_SUCCESS,
    ROOM_NULL_PARAMETER,
	ROOM_OUT_OF_MEMORY,
	ROOM_INVALID_PARAMETER,
	ROOM_ALREADY_RESERVED,
} RoomResult;

//We demand that the entry fee of a room id divisible by MULTIPLE
#define MULTIPLE 4

/*
 * Allocates a new room.
 * Creates a new room and initializes its fields according to the parameters.
 * The required fields are:
 *  $result: Holds the result of the create function. Can be sent as NULL if
 *  the result of this function is not needed.
 *  $id: The room's id. Must be a positive number and unique (no rooms in this
 * 	company share this id).
 * 	$entry_fee: The room's entry fee. Must be non-negative and divisible by 4.
 * 	$recommended_people: The amount of people recommended for the room. Must be
 * 	a positive number.
 * 	$difficulty: The room's difficulty. Must be between 1 and 10 (included).
 * 	$working_hours: The room's scheduled working hours. Received as a string of
 * 	the following format: "xx-yy". xx and yy must be between 0 and 23,
 * 	where yy>xx.
 * #return:
 * 	NULL if one of the parameters is invalid or an allocation fails,
 * 	and a new room otherwise. The new room has no reservations nor earns.
 */
Room roomCreate(RoomResult* result, int id, int entry_fee,
int recommended_people, int difficulty, char* working_hours);

/*
 * Deallocates a room. Clears all of the reservations of the room by using the
 * reservations' destroyer.
 * #return:
 * 	ROOM_NULL_PARAMETER - if a NULL pointer was sent.
 * 	ROOM_SUCCESS - otherwise.
 */
RoomResult roomDestroy(Room room);

/*
 * Creates a copy of the target room.
 * The new copy will contain all of the values of the source room's fields.
 * #return:
 * 	NULL if a NULL pointer was sent or a memory allocation failed,
 * 	and a new copy of the room otherwise.
 */
Room roomCopy(Room room);

/*
 * Gets the room's ID.
 * 	$id: address of a value that will store the id.
 * #return:
 * 	ROOM_NULL_PARAMETER - if a NULL pointer was sent.
 * 	ROOM_SUCCESS - otherwise.
 */
RoomResult roomGetId(Room room, int* id);

/*
 * Gets the entry fee of room.
 * 	$price: address of a value that will store the price.
 * #return:
 * 	ROOM_NULL_PARAMETER - if a NULL pointer was sent.
 * 	ROOM_SUCCESS - otherwise.
 */
RoomResult roomGetPrice(Room room, int* price);

/*
 * Gets the time in the day when the room is scheduled to open.
 * 	$time: address of a value that will store the time.
 * #return:
 * 	ROOM_NULL_PARAMETER - if a NULL pointer was sent.
 * 	ROOM_SUCCESS - otherwise.
 */
RoomResult roomGetOpenTime(Room room, int *time);

/*
 * Gets the time in the day when the room is scheduled to close.
 * 	$time: address of a value that will store the time.
 * #return:
 * 	ROOM_NULL_PARAMETER - if a NULL pointer was sent.
 * 	ROOM_SUCCESS - otherwise.
 */
RoomResult roomGetCloseTime(Room room, int *time);

/*
 * Gets the room's difficulty.
 * 	$difficulty: address of a value that will store the difficulty.
 * #return:
 * 	ROOM_NULL_PARAMETER - if a NULL pointer was sent.
 * 	ROOM_SUCCESS - otherwise.
 */
RoomResult roomGetDifficulty(Room room, int *difficulty);

/*
 * Gets the total amount of money earned in the room.
 * 	$money: address of a value that will store the amount of money.
 * #return:
 * 	ROOM_NULL_PARAMETER - if a NULL pointer was sent.
 * 	ROOM_SUCCESS - otherwise.
 */
RoomResult roomEarns(Room room, int* money);

/*
 * Checks if a room has any scheduled reservations at the time.
 * #return:
 * 	false - if a NULL pointer was sent, or the room has no reservations at all.
 * 	true - otherwise.
 */
bool roomHasReservations(Room room);

/*
 * Returns how many reservations are scheduled for today for this room.
 * #return:
 *  0: if a NULL parameter was sent or no reservations were found for today.
 *  and the amount of reservations for today otherwise.
 */
int roomTodaysReservations(Room room);

/*
 * Creates a new reservation for the room.
 * The reservation is taken into account by priority in the following way:
 * The room potentially has more urgent reservations (reservations that should
 * be implemented first = less days left / smaller hour value), so the new
 * reservation for the room will be added before / after certain reservations.
 * 	$reservation: the parameter for this function has to be of type
 * 	"reservation", as defined in Reservation.h.
 * #return:
 * 	ROOM_NULL_PARAMETER - if a NULL pointer was sent.
 * 	ROOM_ALREADY_RESERVED - if the room is reserved in the exact hour and day
 * 	desired in the new reservation.
 * 	ROOM_OUT_OF_MEMORY - if the reservation is legal, but an allocation failed.
 * 	ROOM_SUCCESS - otherwise.
 */
RoomResult roomNewReservation(Room room, Reservation reservation);

/*
 * Removes all reservations made by an escaper with the parameter email from
 * this room.
 * 	$email: Email of the escaper.
 * #return:
 * 	ROOM_NULL_PARAMETER - if a NULL pointer was sent.
 * 	ROOM_SUCCESS - otherwise.
 */
RoomResult roomRemoveEscaperReservations(Room room, char* email);

/*
 * Finds a reservation that's scheduled for a certain hours away from now.
 *  $hours_left: The amount of hours left for the reservation.
 * #return:
 *  NULL - If a NULL parameter was sent or a reservation wasn't found
 *  and the desired reservation otherwise.
 */
Reservation roomGetReservationInTime(Room room, int hours_left);

/*
 * Gets the address of two parameters: days and hour. The values that these
 * pointers point to are the integer values of a MINIMUM days-hour combination
 * of a reservation to this room. Finds the nearest available day-hour
 * combination that's bigger than the parameter day-hour combination when the
 * escape room is open and NOT reserved, and UPDATES the values that the days
 * and hour pointer point to, to be the days and hour found.
 * 	$days - The days pointer,
 * 	$hour - The hour pointer.
 * #return:
 * 	ROOM_NULL_PARAMETER - if a NULL pointer was sent.
 * 	ROOM_SUCCESS - otherwise.
 */
RoomResult roomGetEarliestTimeAvailable(Room room, int* days, int* hour);

/*
 * Decreases the room's reservations' scheduled days by one, and fulfills all
 * the reservations that were for today.
 * #return:
 * 	RESERVATION_NULL_PARAMETER - if a NULL pointer was sent.
 * 	RESERVATION_SUCCESS - otherwise.
 */
RoomResult roomNextDay(Room room);

/* Calculates a room's recommended score according to reservation parameters.
 * It compares the escaper's skill with the room's
 * difficulty and the number of people in the reservation with the recommended
 * number of people recommended for the room in order to assign a score.
 *  $escaper_skill: the escaper's skill level.
 *  $participants: the number of the people in the order;
 * #return:
 *  -1 - If a NULL parameter was sent,
 *  and the recommendation score of this room otherwise.
 */
int roomCalculatedScore(Room room, int escaper_skill, int participants);

#endif /* ROOM_H_ */
