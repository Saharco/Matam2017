#include "Room.h"

/*
 * Implementation of the Room ADT that's declared in the .h file:
 * The struct's exposed name is of the pointer "Room".
 */
struct Room_t {
	int id; //The room's ID.
	int entry_fee; //The room's demanded fee.
	int recommended_people; //The amount of people recommended to participate.
	int schedule_open; //The hour of the day when the room opens for business.
	int schedule_close; //The hour of the day when the room closes.
	int difficulty; //The room's difficulty.
	int total_money_earned; //The room's total earns from customers.
	List room_reservations; //A list of reservations for this room.
};

//In our format, hours are represented as two digits.
#define HR_DIGITS 2

//In our format, days and hours are separated with a dash sign.
#define SEPARATE_FIELDS '-'

//Total amount of hours in one day.
#define HOURS_IN_A_DAY 24

/*
 * A macro for checking the validity of a boolean condition.
 * 	$bool_condition - any boolean condition.
 * 	$res - the return value in case the boolean condition is false.
 * If the boolean condition is "false" - returns error.
 */
#define CHECK_VALID(bool_condition, res) \
	if(!(bool_condition)) { return res; }

/*
 * A macro for assigning a value to a pointer iff it's not NULL.
 */
#define INIT_IF_EXISTS(ptr, value) \
	if(ptr) { (*ptr) = value; }

/*
 * A macro for calculating the power of two of a number: x^2
 */
#define POWER_OF_2(x) ((x)*(x))

static int exponent10(int num, int p);
static ListElement copyReservation(ListElement res);
static void freeReservation(ListElement res);
static bool checkParameters(int id, int entry_fee, int recommended_people,
int difficulty, char* working_hours, int *open, int *close);
static bool checkHours(char *working_hours, int *time);
static void roomExecuteReservations(Room room);
static void tryNextHour(Room room, int* hours_left, int* days, int* hour);


/*
 * A static function for raising a 10 to a power, and then multiplying that by
 * a given number.
 * 	$num: The coefficient number.
 * 	$p: The power to which 10 should be raised.
 * #return:
 * 	The number multiplied by 10 to the power of p: num*10^(p).
 */
static int exponent10(int num, int p) {
	if(num==0)
		return 0;
	int result = num;
	for(int i=0; i<p; i++)
		result*=10;
	return result;
}

/*
 * A static function that defines the room's list's copy function.
 * The copy function is required to get an element of type ListElement, and
 * return an element of the same type.
 * The copy function is defined as follows: the list's elements are of the type
 * "Reservation", so we cast the element to be its real type, and then we return
 * the value we get from Reservation's copy function
 */
static ListElement copyReservation(ListElement res) {
	return reservationCopy((Reservation)(res));
}

/*
 * A static function that defines the room's list's free function.
 * The free function is required to get an element of type ListElement, and
 * return void.
 * The free function is defined as follows: the list's elements are of the type
 * "Reservation", so we cast the element to be its real type, and then we use
 * its destroy function in order to deallocate it.
 */
static void freeReservation(ListElement res) {
	reservationDestroy((Reservation)(res));
}

/*
 * A static function used in order to check the validity of the parameters
 * that were used in order to define the to-be-allocated room.
 *  $The room's id: a positive number (greater than 0).
 * 	$The room's entry fee: a positive number. Must be a multiple of 4.
 * 	$The amount of people recommended for the room: a positive number.
 * 	$The room's difficulty: a number between 1 and 10 (included).
 * 	$The room's scheduled working hours: received as a string of the following
 * 	format: "xx-yy". xx and yy must be between 0 and 24, where yy>xx.
 * 	$open: the address of an integer that will store the room's open time.
 * 	$close: the address of an integer that will store the room's close time.
 *	#return:
 *	 false - if any of the parameters does not meet the listed requirements.
 *	 true - otherwise.
 */
static bool checkParameters(int id, int entry_fee, int recommended_people,
int difficulty, char* working_hours, int *open, int *close) {
	assert(open && close);
	CHECK_VALID((working_hours), false)
	CHECK_VALID((id > 0), false)
	CHECK_VALID(((entry_fee>=0) && (entry_fee%MULTIPLE == 0)), false)
	CHECK_VALID((recommended_people > 0), false)
	CHECK_VALID(((difficulty>=1) && (difficulty<=10)), false)
	CHECK_VALID((strlen(working_hours) == (2*HR_DIGITS + 1)), false)
	CHECK_VALID((working_hours[HR_DIGITS] == SEPARATE_FIELDS), false)
	CHECK_VALID((checkHours(working_hours, open)), false)
	CHECK_VALID((checkHours((working_hours+HR_DIGITS+1), close)), false)
	CHECK_VALID((*open >= 0 && *close > *open && *close <= HOURS_IN_A_DAY),
	false)
	CHECK_VALID(((*close) > (*open)), false)
	//All of the room's parameters are valid!
	return true;
}

/*
 * A static function used in order to check the validity of an hour in the
 * time period string parameter.
 * A legal hour is composed of a letter that belongs to {0,1,2} followed up by
 * a letter that belongs to {0,1,2,3}.
 * 	$working_hours: the time period string.
 * 	$time: address of the value that will store the hour as a number
 * #return:
 * 	false - if the translated hour is illegal (wrong format / values),
 * 	true - otherwise
 */
static bool checkHours(char *working_hours, int *time) {
	int temp_digit = 0;
	(*time) = 0;
	for(int i = 0; i<HR_DIGITS; i++) {
		temp_digit = (int)((*(working_hours+i)) - '0');
		if(temp_digit<0 || temp_digit>9) return false;
		(*time)+= exponent10(temp_digit, (HR_DIGITS-1-i));
	}
	return true;
}

/*
 * Executes today's reservations: Adds the reservation's price to the room's
 * total money earned, and then deletes them from the room.
 */
void roomExecuteReservations(Room room) {
	assert(room);
	Reservation res = (Reservation)listGetFirst(room->room_reservations);
	while((res) && reservationIsToday(res)) {
		(room->total_money_earned)+=reservationGetPrice(res);
		listRemoveCurrent(room->room_reservations);
		res = (Reservation)listGetFirst(room->room_reservations);
	}
}

/*
 * Gets 3 pointers. hours_left contains the amount of hours left of a
 * reservation, and we assign the days and hour to point at the new best
 * days-hour combination when the room is open (of course, if we increase the
 * hour to beyond the room's close time - we have to go to the next day!)
 */
static void tryNextHour(Room room, int* hours_left, int* days, int* hour) {
	assert(hours_left && days && hour);
	int req_days = (*hours_left) / HOURS_IN_A_DAY,
	req_hour = ((*hours_left) % HOURS_IN_A_DAY) + 1;
	if(room->schedule_close <= req_hour) {
		req_days++;
		req_hour = room->schedule_open;
	}
	(*hours_left) = HOURS_IN_A_DAY * req_days + req_hour;
	(*days) = req_days;
	(*hour) = req_hour;
}

Room roomCreate(RoomResult* result, int id, int entry_fee,
int recommended_people, int difficulty, char* working_hours) {
	int open_time = 0, close_time = 0;
	if(!working_hours) {
		INIT_IF_EXISTS(result, ROOM_NULL_PARAMETER);
		return NULL;
	}
	if(!checkParameters(id, entry_fee, recommended_people, difficulty,
	working_hours, &open_time, &close_time)) {
		INIT_IF_EXISTS(result, ROOM_INVALID_PARAMETER);
		return NULL;
	}
	//input is valid!
	Room room = malloc(sizeof(*room));
	if(!room) {
		INIT_IF_EXISTS(result, ROOM_OUT_OF_MEMORY);
		return NULL;
	}
	room->room_reservations = listCreate(copyReservation, freeReservation);
	if(!(room->room_reservations)) {
		free(room);
		INIT_IF_EXISTS(result, ROOM_OUT_OF_MEMORY);
		return NULL;
	}
	//initialize the struct's fields:
	room->id = id;
	room->entry_fee = entry_fee;
	room->recommended_people = recommended_people;
	room->difficulty = difficulty;
	room->schedule_open = open_time;
	room->schedule_close = close_time;
	room->total_money_earned = 0;
	INIT_IF_EXISTS(result, ROOM_SUCCESS);
	return room;
}

RoomResult roomDestroy(Room room) {
	CHECK_VALID(room, ROOM_NULL_PARAMETER)
	listDestroy(room->room_reservations);
	free(room);
	return ROOM_SUCCESS;
}

Room roomCopy(Room room) {
	CHECK_VALID(room, NULL)
	char hours[HR_DIGITS*2 + 2] = {0};
	for(int i=0; i<HR_DIGITS*2+1; i++)
		hours[i] = '0';
	hours[HR_DIGITS] = '-';
	hours[HR_DIGITS*2] = '1';
	Room room_copy = roomCreate(NULL, room->id, room->entry_fee,
	room->recommended_people, room->difficulty, hours);
	CHECK_VALID(room_copy, NULL)
	listDestroy(room_copy->room_reservations);
	room_copy->room_reservations = listCopy(room->room_reservations);
	if(!(room_copy->room_reservations)) {
		free(room_copy);
		return NULL;
	}
	room_copy->schedule_open = room->schedule_open;
	room_copy->schedule_close = room->schedule_close;
	room_copy->total_money_earned = room->total_money_earned;
	return room_copy;
}
RoomResult roomGetId(Room room, int* id) {
	CHECK_VALID(room, ROOM_NULL_PARAMETER)
	CHECK_VALID(id, ROOM_NULL_PARAMETER)
	*id = room->id;
	return ROOM_SUCCESS;
}

RoomResult roomGetPrice(Room room, int* price) {
	CHECK_VALID(room, ROOM_NULL_PARAMETER)
	CHECK_VALID(price, ROOM_NULL_PARAMETER)
	*price = room->entry_fee;
	return ROOM_SUCCESS;
}

RoomResult roomGetOpenTime(Room room, int *time) {
	CHECK_VALID(room, ROOM_NULL_PARAMETER)
	CHECK_VALID(time, ROOM_NULL_PARAMETER)
	*time = room->schedule_open;
	return ROOM_SUCCESS;
}

RoomResult roomGetCloseTime(Room room, int *time) {
	CHECK_VALID(room, ROOM_NULL_PARAMETER)
	CHECK_VALID(time, ROOM_NULL_PARAMETER)
	*time = room->schedule_close;
	return ROOM_SUCCESS;
}

RoomResult roomGetDifficulty(Room room, int *difficulty) {
	CHECK_VALID(room, ROOM_NULL_PARAMETER)
	CHECK_VALID(difficulty, ROOM_NULL_PARAMETER)
	*difficulty = room->difficulty;
	return ROOM_SUCCESS;
}

RoomResult roomEarns(Room room, int* money) {
	CHECK_VALID(room, ROOM_NULL_PARAMETER)
	CHECK_VALID(money, ROOM_NULL_PARAMETER)
	*money = room->total_money_earned;
	return ROOM_SUCCESS;
}

bool roomHasReservations(Room room) {
	CHECK_VALID(room, ROOM_NULL_PARAMETER)
	return (listGetSize(room->room_reservations) > 0);
}

int roomTodaysReservations(Room room) {
	CHECK_VALID(room, 0)
	int reservations_amount = 0;
	LIST_FOREACH(Reservation, reservation, room->room_reservations)
		if(reservationIsToday(reservation))
			reservations_amount++;
	return reservations_amount;
}

RoomResult roomNewReservation(Room room, Reservation reservation) {
	CHECK_VALID(room, ROOM_NULL_PARAMETER)
	CHECK_VALID(reservation, ROOM_NULL_PARAMETER)
	int res_hours = reservationHoursLeft(reservation), curr_hours = 0;
	Reservation current = (Reservation)listGetFirst(room->room_reservations);
	while(current) {
		//We find the correct segment in the list to insert the reservation.
		curr_hours = reservationHoursLeft(current);
		if(res_hours == curr_hours) return ROOM_ALREADY_RESERVED;
		else if(res_hours < curr_hours) break;
		current = (Reservation)listGetNext(room->room_reservations);
	}
	ListResult res;
	if(current)
		//The current reservation is for a later date than the given reservation
		//But the previous reservation, if exists, is not.
		res = listInsertBeforeCurrent(room->room_reservations, reservation);
	else
		//The given reservation is for the latest date of all the reservations.
		res = listInsertLast(room->room_reservations, reservation);
	if(res!=LIST_SUCCESS) return ROOM_OUT_OF_MEMORY;
	return ROOM_SUCCESS;
}

RoomResult roomRemoveEscaperReservations(Room room, char* email) {
	CHECK_VALID(room, ROOM_NULL_PARAMETER)
	CHECK_VALID(email, ROOM_NULL_PARAMETER)
	char* current_email = NULL;
	Reservation curr_reservation = listGetFirst(room->room_reservations);
	while(curr_reservation) {
		reservationGetEscaperEmail(curr_reservation, &email);
		if(strcmp(email, current_email) == 0) {
			listRemoveCurrent(room->room_reservations);
			curr_reservation = listGetFirst(room->room_reservations);
		}
		else
			curr_reservation = listGetNext(room->room_reservations);
	}
	return ROOM_SUCCESS;
}

Reservation roomGetReservationInTime(Room room, int hours_left) {
	CHECK_VALID(room, NULL)
	int curr_hours = 0;
	LIST_FOREACH(Reservation, curr_reservation, room->room_reservations) {
		curr_hours = reservationHoursLeft(curr_reservation);
		if(curr_hours > hours_left)
			break;
		else if (curr_hours == hours_left)
			return curr_reservation;
	}
	return NULL;
}

RoomResult roomGetEarliestTimeAvailable(Room room, int* days, int* hour) {
	CHECK_VALID(room, ROOM_NULL_PARAMETER)
	CHECK_VALID(days, ROOM_NULL_PARAMETER)
	CHECK_VALID(hour, ROOM_NULL_PARAMETER)
	int hours_left = HOURS_IN_A_DAY*(*days) + (*hour);
	while(((*hour) >= room->schedule_close) || ((*hour) < room->schedule_open)
	|| (roomGetReservationInTime(room, hours_left) != NULL))
		tryNextHour(room, &hours_left, days, hour);
	return ROOM_SUCCESS;
}

RoomResult roomNextDay(Room room) {
	CHECK_VALID(room, ROOM_NULL_PARAMETER)
	roomExecuteReservations(room);
	LIST_FOREACH(Reservation, res, room->room_reservations) {
		assert(res);
		reservationNextDay(res);
	}
	return ROOM_SUCCESS;
}

int roomCalculatedScore(Room room, int escaper_skill, int participants) {
	if(!room)
		return -1;
	return (POWER_OF_2(room->recommended_people - participants) +
	POWER_OF_2(room->difficulty - escaper_skill));
}
