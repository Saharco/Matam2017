#include "Reservation.h"

/*
 * Implementation of the Reservation ADT that's declared in the .h file:
 * The struct's exposed name is of the pointer "Reservation".
 */
struct Reservation_t {
	char* email; //Email of the customer
	int room_id; //ID of the room for which this reservation was made
	TechnionFaculty room_faculty; // The faculty where the desired room belongs
	int price; //The price of this reservation
	int days_left; //The amount of days left for this reservation
	int reserved_hour; //The hour of the day for which this reservation is for
	int num_of_people; //The amount of people for this reservation
	bool has_discount; //Whether or not the reservation got discounted
};


//In our format, days and hours are separated with a dash sign.
#define SEPARATE_FIELDS '-'

//Total amount of hours in one day.
#define HOURS_IN_A_DAY 24

//In our format, an email must contain this sign.
#define EMAIL_SIGN '@'

#define INVALID -1

/*
 * A macro for checking the validity of a boolean condition.
 * 	$bool_condition - any boolean condition.
 * 	$res - the return value in case the boolean condition is false.
 * If the boolean condition is "false" - returns error.
 */
#define CHECK_VALID(bool_condition, res) \
	if(!(bool_condition)) { return res; }

#define INIT_IF_EXISTS(ptr, value) \
	if(ptr) { (*ptr) = value; }


static bool isLegalEmail(char* email);
static bool checkParameters(char* email, int id, TechnionFaculty faculty,
int room_price, int room_open, int room_close, int num_of_people, char* date,
int* days, int* hour);
static bool validateDate(char *date, char delimeter);
static bool readDate(char *date, int *days, int* hour);
static void fillDate(char *date, int days, int hour);
static int numDigits(int num);
static void discountPrice(int *price);

/*
 * A static function for checking if a given email is legal.
 * A legal email is any email that contains exactly one '@'.
 * 	$email: The required email string for the procedure.
 * #return:
 * 	true - if the email has exactly one '@',
 * 	false - otherwise.
 */
static bool isLegalEmail(char* email) {
	assert(email);
	int i=0, count=0;
	while(email[i])
		if(email[i++] == EMAIL_SIGN) count++;
	return (count == 1);
}

/*
 * A static function used in order to check the validity of the parameters
 * that were used in order to define the to-be-allocated reservation.
 * If succeeds, stores the days and hour values of the reservation as integers
 * instead of a string in the given addresses.
 *  $email: Pointer to the email string of the customer who made this
 * 	reservation. Must contain '@'.
 * 	$id: Id of the desired room for this reservation. Must be greater than 0.
 * 	$room_price: The original entry fee of the room.
 * 	$room_open: The time when the room opens for business.
 * 	$room_close: The time when the room closes.
 * 	$num_of_people: The amount of people for this reservation. Must be greater
 * 	than 0.
 * 	$date: The time of reservation. The time is a string of the format: "dd-hh",
 * 	where the first two digits represent how many days from now the reservation
 * 	is for, and the the last two digits represent the hour of the day for the
 * 	reservation. Days must be non-negative, hour is between 0 and 23, but has
 * 	to be during the time when the escape room is open: more or equal to the
 * 	open time, and less than the close time.
 * 	$days: Pointer to the integer that will store the reservation's days.
 * 	$hour: Pointer to the integer that will store the reservation's hour.
 * #return:
 *	 false - If any of the parameters does not meet the listed requirements.
 *	 true - Otherwise.
 */
static bool checkParameters(char* email, int id, TechnionFaculty faculty,
int room_price, int room_open, int room_close, int num_of_people, char* date,
int* days, int* hour) {
	assert(email && date);
	CHECK_VALID((isLegalEmail(email)), false)
	CHECK_VALID((id>=0), false)
	CHECK_VALID((faculty>=0 && faculty<UNKNOWN), false)
	CHECK_VALID((room_price > 0 && room_price%MULTIPLE == 0), false)
	CHECK_VALID((num_of_people > 0), false)
	CHECK_VALID((room_open < room_close), false)
	CHECK_VALID(((room_open >= 0) && (room_close <= HOURS_IN_A_DAY)), false)
	CHECK_VALID((readDate(date, days, hour)), false)
	CHECK_VALID((((*hour)>=room_open) && (*hour)<room_close), false)
	return true;
}


/*
 * Validates a reservation's scheduled date
*/
static bool validateDate(char *date, char delimeter) {
	assert(date);
	int i = 0;
	bool changed_delim = false, field1_flag = false, field2_flag = false;
	while(date[i++]) {
		if((!changed_delim) && (date[i-1] == delimeter)) {
			date[i-1] = ' ';
			changed_delim = true;
			continue;
		}
		else if(date[i-1] < '0' || date[i-1] > '9')
			return false;
		if(!changed_delim)
			field1_flag = true;
		else
			field2_flag = true;
	}
	if(!changed_delim || !field1_flag || !field2_flag)
		return false;
	return true;
}

/*
 * A static function used to check the validity of the date.
 * 	$date: the date string, of the format "dd-hh".
 *	$time: the address of a value that will store either the day or hour.
 * #return:
 * 	false - if the field that was checked (either day or hour) was not a valid
 * 	number, and true - otherwise.
 */
static bool readDate(char *date, int *days, int* hour) {
	if(!date)
		return false; //
	char* date_copy = malloc(sizeof(date)+1);
	if(!date_copy)
		return false;
	strcpy(date_copy, date);
	if(!validateDate(date_copy, SEPARATE_FIELDS)) {
		free(date_copy);
		return false;
	}
	sscanf(date_copy, "%d %d", days, hour);
	free(date_copy);
	return true;
}

/*
 * A static function used in order to initialize a string (of size 5) to be
 * of the desired format for reservationCreate: the string will be of the
 * format "days-hour", where "days" and "hour" are integers, and will contain a
 * leading zero in any of the two fields if needed (in case a number is smaller
 * than 10 and only has one digit).
 * 	$days - The days field in the string
 * 	$hour - The hour field in the string
 */
static void fillDate(char *date, int days, int hour) {
	sprintf(date, "%d%c%d" , days, SEPARATE_FIELDS, hour);
}

static int numDigits(int num) {
	int digits=0;
	while(num) {
		num/=10;
		digits++;
	}
	return ((digits == 0) ? 1 : digits);
}

/*
 * A static function used to grant a fixed percentage discount to a given price.
 * The discount's default is 25%, and is determined by the (visible)
 * DISCOUNT_PARAMETER macro, which is 75 by default. The discount is granted as
 * follows: Multiplying the price by the discount parameter, and then dividing
 * by 100. Assumption is that the price is divisible by 4 at all times, as
 * represented by the MULTIPLE(=4) macro.
 * 	$price: pointer to the price of the reservation.
 */
static void discountPrice(int *price) {
	(*price) = ((*price)*DISCOUNT_PARAMETER) / 100;
}

Reservation reservationCreate(ReservationResult *result, char* email, int id,
TechnionFaculty faculty, int room_price, int room_open, int room_close,
int num_of_people, char* date, bool has_discount) {
	if((!email) || (!date)) {
		INIT_IF_EXISTS(result, RESERVATION_NULL_PARAMETER);
		return NULL;
	}
	int days=0, hour=0, price=0;
	if(!checkParameters(email, id, faculty, room_price, room_open,
	room_close, num_of_people, date, &days, &hour)) {
		INIT_IF_EXISTS(result, RESERVATION_INVALID_PARAMETER);
		return NULL;
	}
	Reservation reservation = malloc(sizeof(*reservation));
	if(!reservation) {
		INIT_IF_EXISTS(result, RESERVATION_OUT_OF_MEMORY);
		return NULL;
	}
	price = room_price * num_of_people;
	if(has_discount)
		discountPrice(&price);
	reservation->email = email;
	reservation->room_id = id;
	reservation->price = price;
	reservation->num_of_people = num_of_people;
	reservation->days_left = days;
	reservation->reserved_hour = hour;
	reservation->has_discount = has_discount;
	reservation->room_faculty = faculty;
	INIT_IF_EXISTS(result, RESERVATION_SUCCESS);
	return reservation;
}

ReservationResult reservationDestroy(Reservation reservation) {
	CHECK_VALID(reservation, RESERVATION_NULL_PARAMETER)
	free(reservation);
	return RESERVATION_SUCCESS;
}

Reservation reservationCopy(Reservation reservation) {
	CHECK_VALID(reservation, NULL)
	int date_size = numDigits(reservation->days_left) +
	numDigits(reservation->reserved_hour) + 1;
	char* date = malloc(date_size+1);
	CHECK_VALID(date, NULL)
	date[date_size] = 0;
	fillDate(date, reservation->days_left, reservation->reserved_hour);
	Reservation reservation_copy = reservationCreate(NULL, reservation->email,
	reservation->room_id, reservation->room_faculty, MULTIPLE, 0,
	HOURS_IN_A_DAY-1, reservation->num_of_people, date,
	reservation->has_discount);
	free(date);
	CHECK_VALID(reservation_copy, NULL)
	reservation_copy->reserved_hour = reservation->reserved_hour;
	reservation_copy->price = reservation->price;
	return reservation_copy;
}

ReservationResult reservationNextDay(Reservation reservation) {
	CHECK_VALID(reservation, RESERVATION_NULL_PARAMETER)
	if(reservation->days_left > 0) (reservation->days_left)--;
	return RESERVATION_SUCCESS;
}

bool reservationIsToday(Reservation reservation) {
	CHECK_VALID(reservation, false)
	return ((reservation->days_left)==0);
}

int reservationHoursLeft(Reservation reservation) {
	CHECK_VALID(reservation, RESERVATION_ILLEGAL)
	int hours_total = HOURS_IN_A_DAY*(reservation->days_left)
	+ reservation->reserved_hour;
	return hours_total;
}

int reservationGetDay(Reservation reservation) {
	CHECK_VALID(reservation, RESERVATION_ILLEGAL)
	return reservation->days_left;
}

int reservationGetHour(Reservation reservation) {
	CHECK_VALID(reservation, RESERVATION_ILLEGAL)
	return reservation->reserved_hour;
}

int reservationGetPrice(Reservation reservation) {
	CHECK_VALID(reservation, RESERVATION_ILLEGAL)
	return reservation->price;
}

int reservationGetPeople(Reservation reservation) {
	CHECK_VALID(reservation, RESERVATION_ILLEGAL)
	return reservation->num_of_people;
}

ReservationResult reservationGetRoomId(Reservation reservation, int* room_id) {
	CHECK_VALID(reservation, RESERVATION_NULL_PARAMETER)
	CHECK_VALID(room_id, RESERVATION_NULL_PARAMETER)
	*room_id = reservation->room_id;
	return RESERVATION_SUCCESS;
}

ReservationResult reservationGetEscaperEmail(Reservation reservation,
char** email) {
	CHECK_VALID(reservation, RESERVATION_NULL_PARAMETER)
	CHECK_VALID(email, RESERVATION_NULL_PARAMETER)
	*email = reservation->email;
	return RESERVATION_SUCCESS;
}

bool reservationIsDiscounted(Reservation reservation) {
	CHECK_VALID(reservation, false)
	return reservation->has_discount;
}

bool reservationIsValid(Reservation reservation) {
	CHECK_VALID(reservation, false)
	return (reservation->num_of_people > 0);
}

ReservationResult reservationInvalidate(Reservation reservation) {
	CHECK_VALID(reservation, false)
	reservation->num_of_people = INVALID;
	//This reservation now stores invalid data, and will be recognized as such.
	return RESERVATION_SUCCESS;
}
