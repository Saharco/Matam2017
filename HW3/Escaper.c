#include "Escaper.h"

/*
 * Implementation of the Escaper ADT declared in the .h file:
 * The struct's exposed name is of the pointer "Escaper".
 */
struct Escaper_t {
	char* email; //The escaper's email.
	TechnionFaculty faculty; //The escaper's Faculty.
	int skill_level; //The escaper's skill level.
	Set client_schedule; //A set of the escaper's scheduled hours of orders.
};

//In our format, an email must contain this sign.
#define EMAIL_SIGN '@'

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

static SetElement copyInteger(SetElement num);
static void freeInteger(SetElement num);
static int compareIntegers(SetElement num1, SetElement num2);
static bool isLegalEmail(char* email);
static bool checkParameters(char* email, TechnionFaculty faculty,
int skill_level);
void escaperExecuteReservations(Escaper escaper);

/*
 * A static function that defines the escaper's set's copy function.
 * The copy function is required to get an element of type SetElement, and
 * return an element of the same type.
 * The copy function is defined as follows: We allocate a new integer pointer,
 * and then - the set's elements are of the type "int*", so we cast the element
 * to be its real type and dereference the pointer to get the value, and store
 * that value as the value of the the dereferenced allocated integer. We then
 * return the new integer, or NULL if the allocation has failed.
 */
static SetElement copyInteger(SetElement num) {
	int* copy = malloc(sizeof(int));
	CHECK_VALID(copy, NULL)
	*copy = *((int*)(num));
	return copy;
}

/*
 * A static function that defines the escaper's set's free function.
 * The free function is required to get an element of type SetElement, and
 * return void. The free function is defined as follows: we simply free the
 * the non-complex int* type.
 */
static void freeInteger(SetElement num) {
	free(num);
}

/*
 * A static function that defines the escaper's set's compare function.
 * The compare function is required to get two elements of type SetElement, and
 * return an integer.
 * The compare function is defined as follows: the set's elements are of the
 * type "int*", so we cast the element to be its real type, and then
 * dereference to get the value that these pointers hold, and return an
 * arithmetical subtraction.
 */
static int compareIntegers(SetElement num1, SetElement num2) {
	return (*((int*)(num1)) - *((int*)(num2)));
}

/*
 * A static function used in order to check if the escaper's
 * email is valid by checking if it has the char '@' only one time in it
 */
static bool isLegalEmail(char* email){
	assert(email);
	int i=0, count=0;
	while(email[i])
		if(email[i++] == EMAIL_SIGN)
			count++;
	return (count == 1);
}

/*
 * A static function used in order to check the validity of the parameters
 * that were used in order to define the to-be-allocated room.
 * 	$The escaper's email: checks if the email is NULL and if
 * 	an email has the char '@' in it
 * 	$The escaper's skill level: a number between 1 and 10 (included)
 * 	#return:
 * 	 false - if any of the parameters does not meet the listed requirements.
 *	 true - otherwise.
 */
static bool checkParameters(char* email, TechnionFaculty faculty,
int skill_level) {
	CHECK_VALID(((faculty>=0) && (faculty<UNKNOWN)), false)
	CHECK_VALID(email, false)
	CHECK_VALID(isLegalEmail(email), false)
	CHECK_VALID(((skill_level>=1) && (skill_level<=10)), false)
	return true;
}

/*
 * Removes all of today's reservation from this escaper's scheduled reservations
 */
void escaperExecuteReservations(Escaper escaper) {
	assert(escaper);
	int* hours = ((int*)setGetFirst(escaper->client_schedule));
	while((hours) && (*hours < HOURS_IN_A_DAY)) {
		setRemove(escaper->client_schedule, hours);
		hours = ((int*)setGetFirst(escaper->client_schedule));
	}
}

Escaper escaperCreate(EscaperResult *result, char* email,
TechnionFaculty faculty, int skill_level) {
	if(!email) {
		INIT_IF_EXISTS(result, ESCAPER_NULL_PARAMETER);
		return NULL;
	}
	if(!checkParameters(email, faculty, skill_level)) {
		INIT_IF_EXISTS(result, ESCAPER_INVALID_PARAMETER);
		return NULL;
	}
	//input is valid!
	char* email_copy = malloc(strlen(email)+1);
	if(!email_copy) {
		INIT_IF_EXISTS(result, ESCAPER_OUT_OF_MEMORY);
		return NULL;
	}
	strcpy(email_copy, email);
	Escaper escaper = malloc(sizeof(*escaper));
	if(!escaper) {
		free(email_copy);
		INIT_IF_EXISTS(result, ESCAPER_OUT_OF_MEMORY);
		return NULL;
	}
	escaper->client_schedule = setCreate(copyInteger, freeInteger,
	compareIntegers);
	if(!(escaper->client_schedule)) {
		free(email_copy);
		free(escaper);
		INIT_IF_EXISTS(result, ESCAPER_OUT_OF_MEMORY);
		return NULL;;
	}
	//Initialize the struct's fields:
	escaper->email = email_copy;
	escaper->faculty = faculty;
	escaper->skill_level = skill_level;
	INIT_IF_EXISTS(result, ESCAPER_SUCCESS);
	return escaper;
}

EscaperResult escaperDestroy(Escaper escaper){
	CHECK_VALID(escaper,ESCAPER_NULL_PARAMETER)
	setDestroy(escaper->client_schedule);
	free(escaper->email);
	free(escaper);
	return ESCAPER_SUCCESS;
}

Escaper escaperCopy(Escaper escaper){
	CHECK_VALID(escaper, NULL)
	Escaper escaper_copy = escaperCreate(NULL, escaper->email, escaper->faculty,
	escaper->skill_level);
	CHECK_VALID(escaper_copy, NULL)
	setDestroy(escaper_copy->client_schedule);
	escaper_copy->client_schedule = setCopy(escaper->client_schedule);
	return escaper_copy;
}
EscaperResult escaperGetEmail(Escaper escaper, char** email){
	CHECK_VALID(escaper, ESCAPER_NULL_PARAMETER)
	CHECK_VALID(email, ESCAPER_NULL_PARAMETER)
	*email = escaper->email;
	return ESCAPER_SUCCESS;
}

EscaperResult escaperGetFaculty(Escaper escaper, TechnionFaculty* faculty){
	CHECK_VALID(escaper, ESCAPER_NULL_PARAMETER)
	CHECK_VALID(faculty, ESCAPER_NULL_PARAMETER)
	*faculty = escaper->faculty;
	return ESCAPER_SUCCESS;
}

EscaperResult escaperGetSkillLevel(Escaper escaper, int* skill_level){
	CHECK_VALID(escaper, ESCAPER_NULL_PARAMETER)
	CHECK_VALID(skill_level, ESCAPER_NULL_PARAMETER)
	*skill_level = escaper->skill_level;
	return ESCAPER_SUCCESS;
}

EscaperResult escaperReservationsAmount(Escaper escaper, int* amount) {
	CHECK_VALID(escaper, ESCAPER_NULL_PARAMETER)
	CHECK_VALID(amount, ESCAPER_NULL_PARAMETER)
	*amount = setGetSize(escaper->client_schedule);
	return ESCAPER_SUCCESS;
}

EscaperResult escaperMakeReservation(Escaper escaper, Room room,
TechnionFaculty faculty, int num_of_people, char* date) {
	CHECK_VALID(escaper, ESCAPER_NULL_PARAMETER)
	CHECK_VALID(room, ESCAPER_NULL_PARAMETER)
	CHECK_VALID(date, ESCAPER_NULL_PARAMETER)
	int id = 0, price = 0, open = 0, close = 0;
	roomGetId(room, &id);
	roomGetPrice(room, &price);
	roomGetOpenTime(room, &open);
	roomGetCloseTime(room, &close);
	Reservation res = reservationCreate(NULL, escaper->email, id, faculty,
	price, open, close, num_of_people, date, ((escaper->faculty) == faculty));
	CHECK_VALID(res, ESCAPER_OUT_OF_MEMORY)
	int hour = reservationHoursLeft(res);
	SetResult result1 = setAdd(escaper->client_schedule, &hour);
	if(result1 != SET_SUCCESS) {
		//There are only two options for failure here. We return the correct one
		reservationDestroy(res);
		return (result1 == SET_OUT_OF_MEMORY) ?
		(ESCAPER_OUT_OF_MEMORY) : (ESCAPER_IN_ROOM);
	}
	RoomResult result2 = roomNewReservation(room, res);
	if(result2 != ROOM_SUCCESS) {
		//There are only two options for failure here. We return the correct one
		reservationDestroy(res);
		setRemove(escaper->client_schedule, &hour);
		return (result2 == ROOM_OUT_OF_MEMORY) ?
		(ESCAPER_OUT_OF_MEMORY) : (ESCAPER_RESERVED_ROOM);
	}
	reservationDestroy(res);
	return ESCAPER_SUCCESS;
}

EscaperResult escaperNextDay(Escaper escaper) {
	CHECK_VALID(escaper, ESCAPER_NULL_PARAMETER)
	escaperExecuteReservations(escaper);
	SET_FOREACH(int*, time, escaper->client_schedule) {
		assert(time);
		(*time) -= HOURS_IN_A_DAY; //Time of the reservation decreased by 24.
	}
	return ROOM_SUCCESS;
}

bool escaperIsBusy(Escaper escaper, int hours) {
	CHECK_VALID(escaper, ESCAPER_NULL_PARAMETER)
	return setIsIn(escaper->client_schedule, &hours);
}

//We ended up not using this function. Oh well, at least it looks good.
EscaperResult escaperRemoveReservation(Escaper escaper, int hours) {
	CHECK_VALID(escaper, ESCAPER_NULL_PARAMETER)
	if(setRemove(escaper->client_schedule, &hours) != SET_SUCCESS)
		return ESCAPER_INVALID_PARAMETER;
	return ESCAPER_SUCCESS;
}
