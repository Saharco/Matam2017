#include "EscapeTechnion.h"

/*
 * Implementation of the EscapeTechnion ADT declared in the .h file:
 * The struct's exposed name is of the pointer "EscapeTechnion".
 */
struct EscapeTechnion_t {
	int days_passed; //Amount of days passed since the creation of the system.
	Set escapers; //Set of escapers.
	Set companies; //Set of companies.
	FILE* output_channel; //The output channel for the system.
};

//In our format, hours are represented as two digits.
#define HR_DIGITS 2

//In our format, days and hours are separated with a dash sign.
#define SEPARATE_FIELDS '-'

//A valid email must contain exactly one of this sign
#define EMAIL_SIGN '@'

//The maximum possible skill / difficulty
#define MAX_LEVEL 10

//The amount of hours in a day (duh!)
#define HOURS_IN_A_DAY 24

//The amount of faculties being printed as the best ones
#define BEST_FACULTIES_AMOUNT 3

/*
 * A macro for checking the validity of a boolean condition.
 * 	$bool_condition - any boolean condition.
 * 	$res - the return value in case the boolean condition is false.
 * If the boolean condition is "false" - returns error.
 */
#define CHECK_VALID(bool_condition, res) \
	if(!(bool_condition)) { return res; }

/*
 * A macro for calculating the absolute distance between two numbers.
 */
#define ABSOLUTE_DISTANCE(x, y) ((x) < (y) ? (y-x) : (x-y))

//START OF STATIC FUNCTIONS

static SetElement setCompanyCopy(SetElement company);
static void setCompanyFree(SetElement company);
static int setCompanyCompare(SetElement company1, SetElement company2);
static SetElement setEscaperCopy(SetElement escaper);
static void setEscaperFree(SetElement escaper);
static int setEscaperCompare(SetElement escaper1, SetElement escaper2);
static bool isLegalEmail(char* email);
static bool checkRoomParameters(int id, int entry_fee, int recommended_people,
int difficulty, char* working_hours);
static int exponent10(int num, int p);
static bool checkHours(char *working_hours, int *time);
static bool checkReservationTime(char* date, int room_open, int room_close);
static bool validateDate(char *date, char delimeter);
static bool readDate(char *date, int *days, int* hour);
static Company getCompanyByEmail(EscapeTechnion sys, char* email);
static Company getCompanyByRoomId(EscapeTechnion sys, TechnionFaculty faculty,
int id);
static Escaper getEscaperByEmail(EscapeTechnion sys, char* email);
static Room getFacultyRoomById(EscapeTechnion sys, int id,
TechnionFaculty faculty);
static Room recommendedRoom(EscapeTechnion sys, int escaper_skill_level,
int num_ppl, TechnionFaculty escaper_faculty, TechnionFaculty* room_faculty);
static void updateRecommendedRoomValues(int* min, Room* best_room,
TechnionFaculty* best_faculty, int score, Room room, TechnionFaculty faculty);
static int numDigits(int num);
static void fillDate(char *date, int days, int hour);
static void dateNextRoomHour(Room room, char* date, int days, int hour);
static int systemTodaysReservations(EscapeTechnion sys);
static void printReservation(EscapeTechnion sys, Company curr_company,
Room curr_room, Reservation curr_reservation);
static void systemFillTopFaculties(EscapeTechnion sys, int faculty_earns[],
TechnionFaculty best_faculties[]);

//END OF STATIC FUNCTIONS

/*
 * Defining the copy function for a set of companies
 */
static SetElement setCompanyCopy(SetElement company) {
	return companyCopy((Company)company);
}

/*
 * Defining the free function for a set of companies
 */
static void setCompanyFree(SetElement company) {
	companyDestroy((Company)company);
}

/*
 * Defining the compare function for a set of companies
 */
static int setCompanyCompare(SetElement company1, SetElement company2) {
	char* email1 = NULL;
	char* email2 = NULL;
	companyGetEmail((Company)company1, &email1);
	companyGetEmail((Company)company2, &email2);
	return (strcmp(email1, email2));
}

/*
 * Defining the copy function for a set of escapers
 */
static SetElement setEscaperCopy(SetElement escaper) {
	return escaperCopy((Escaper)escaper);
}

/*
 * Defining the free function for a set of escapers (it has the best name)
 */
static void setEscaperFree(SetElement escaper) {
	escaperDestroy((Escaper)escaper);
}

/*
 * Defining the compare function for a set of escapers
 */
static int setEscaperCompare(SetElement escaper1, SetElement escaper2) {
	char* email1 = NULL;
	char* email2 = NULL;
	escaperGetEmail((Escaper)escaper1, &email1);
	escaperGetEmail((Escaper)escaper2, &email2);
	return (strcmp(email1, email2));
}

/*
 * Checks if the email address contains exactly one '@'
 */
static bool isLegalEmail(char* email) {
	int i=0, count=0;
	while(email[i])
		if(email[i++] == EMAIL_SIGN)
			count++;
	return (count == 1);
}

/*
 * Validates the room parameters.
 * NOTE: The room already checks this, but we needed to check this first in the
 * system ADT because of the order of errors returned from the function...
 */
static bool checkRoomParameters(int id, int entry_fee, int recommended_people,
int difficulty, char* working_hours) {
	CHECK_VALID((working_hours), false)
	CHECK_VALID((id > 0), false)
	CHECK_VALID(((entry_fee>=0) && (entry_fee%MULTIPLE == 0)), false)
	CHECK_VALID((recommended_people > 0), false)
	CHECK_VALID(((difficulty>=1) && (difficulty<=10)), false)
	CHECK_VALID((strlen(working_hours) == (2*HR_DIGITS + 1)), false)
	CHECK_VALID((working_hours[HR_DIGITS] == SEPARATE_FIELDS), false)
	int open=0, close=0;
	CHECK_VALID((checkHours(working_hours, &open)), false)
	CHECK_VALID((checkHours((working_hours+HR_DIGITS+1), &close)), false)
	CHECK_VALID((open >= 0 && close > open && close <= HOURS_IN_A_DAY), false)
	CHECK_VALID(((close) > (open)), false)
	return true;
}

/*
 * Checks that the hours string is valid (one field at a time), and converts
 * result into a parameter address
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
 * Arithmetical exponent function: num*10^p
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
 * Checks if a reservation's scheduled time is within the open and close hours
 * of a room
 */
static bool checkReservationTime(char* date, int room_open, int room_close) {
	int days=0, hour=0;
	CHECK_VALID((readDate(date, &days, &hour)), false)
	CHECK_VALID((((hour)>=room_open) && (hour)<room_close), false)
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
			//the delimeter '-' was found
			date[i-1] = ' ';
			changed_delim = true;
			continue;
		}
		else if(date[i-1] < '0' || date[i-1] > '9')
			//non-delimeter, non-digit char
			return false;
		if (!changed_delim)
			//field1 (days): valid digit
			field1_flag = true;
		else
			//field2 (hour): valid digit
			field2_flag = true;
	}
	if(!changed_delim || !field1_flag || !field2_flag)
		return false;
	return true;
}

/*
 * A static function used to check the validity of the date.
 * 	$date: the date string, of the format "dd-hh".
 *	$days: the address of a value that will store either the day
 *	$hour: the address of a value that will store the hour
 * #return:
 * 	false - if the field that was checked (either day or hour) was not a valid
 * 	number, and true - otherwise.
 */
static bool readDate(char *date, int *days, int* hour) {
	assert(date);
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
 * Gets the email of a company in the system and returns it (or NULL if it does
 * not exist)
 */
static Company getCompanyByEmail(EscapeTechnion sys, char* email) {
	assert(sys && email);
	char* target_email = NULL;
	SET_FOREACH(Company, target_company, sys->companies) {
		assert(target_company);
		companyGetEmail(target_company, &target_email);
		if(strcmp(email, target_email) == 0)
			return target_company;
	}
	return NULL;
}

/*
 * Gets the faculty and id of a company in the system and returns it
 * (or NULL if it does not exist)
 */
static Company getCompanyByRoomId(EscapeTechnion sys, TechnionFaculty faculty,
int id) {
	assert(sys);
	TechnionFaculty target_faculty = UNKNOWN;
	Room room = NULL;
	SET_FOREACH(Company, company, sys->companies) {
		assert(company);
		companyGetFaculty(company, &target_faculty);
		if(faculty == target_faculty) {
			companyGetRoom(company, &room, id);
			if(room != NULL)
				return company;
		}
	}
	return NULL;
}

/*
 * Gets the email of a user in the system and returns it (or NULL if it does
 * not exist)
 */
static Escaper getEscaperByEmail(EscapeTechnion sys, char* email) {
	assert(sys && email);
	char* target_email = NULL;
	SET_FOREACH(Escaper, target_escaper, sys->escapers) {
		assert(target_escaper);
		escaperGetEmail(target_escaper, &target_email);
		if((target_email) && strcmp(email, target_email) == 0)
			return target_escaper;
	}
	return NULL;
}

/*
 * Gets the faculty and id of a room in the system and returns it
 * (or NULL if it does not exist)
 */
static Room getFacultyRoomById(EscapeTechnion sys, int id,
TechnionFaculty faculty) {
	assert(sys);
	TechnionFaculty target_faculty;
	Room desired_room = NULL;
	SET_FOREACH(Company, company, sys->companies) {
		assert(company);
		companyGetFaculty(company, &target_faculty);
		if(faculty == target_faculty) {
			CompanyResult result = companyGetRoom(company, &desired_room, id);
			if(result == COMPANY_SUCCESS)
				return desired_room;
		}
	}
	return NULL;
}

/*
 *	Finds the recommended room in the system (calculation is performed according
 *	to the parameters given), and returns it (or NULL if there are no rooms
 *	in the system). If multiple rooms with the same score are found - the one
 *	that belongs to the faculty that's relatively closer to the escaper's
 *	faculty is returned.
 */
static Room recommendedRoom(EscapeTechnion sys, int escaper_skill_level,
int num_ppl, TechnionFaculty escaper_faculty, TechnionFaculty* room_faculty) {
	TechnionFaculty faculty = UNKNOWN, best_faculty = UNKNOWN;
	companyGetFaculty(setGetFirst(sys->companies), &best_faculty);
	int id=0, score=0;
	Room best_room = companyLowestIdRoom(setGetFirst(sys->companies), &id);
	if(!best_room)
		//There are no rooms in the system!
		return NULL;
	int min_score=roomCalculatedScore(best_room, escaper_skill_level, num_ppl);
	//There are rooms in the company.
	for(TechnionFaculty curr_faculty=0; curr_faculty<UNKNOWN; curr_faculty++){
		SET_FOREACH(Company, company, sys->companies) {
			companyGetFaculty(company, &faculty);
			if(faculty != curr_faculty)
				break;
			//The company is of the desired faculty
			COMPANY_FOREACH(room, company, id) {
				score = roomCalculatedScore(room, escaper_skill_level,
				num_ppl);
				if(score > min_score)
					continue;
				//Found a potential best room
				if(score < min_score) {
					updateRecommendedRoomValues(&min_score, &best_room,
					&best_faculty, score, room, curr_faculty);
				}
				else {
					//Found two rooms with the same score
					int diff1=ABSOLUTE_DISTANCE(escaper_faculty, curr_faculty);
					int diff2=ABSOLUTE_DISTANCE(escaper_faculty, best_faculty);
					if(diff1 < diff2) {
						//The newly found room is of a relatively closer
						//faculty to the escaper!
						updateRecommendedRoomValues(&min_score, &best_room,
						&best_faculty, score, room, curr_faculty);
					}
				}
			}
		}
	}
	*room_faculty = best_faculty;
	return best_room;
}

/*
 * A simple re-assignment function of 3 parameters, to prevent code duplication.
 */
static void updateRecommendedRoomValues(int* min, Room* best_room,
TechnionFaculty* best_faculty, int score, Room room, TechnionFaculty faculty) {
	assert(best_room && room);
	*min = score;
	*best_room = room;
	*best_faculty = faculty;
}

/*
 * Gets a number and returns the amount of digits in it (0 has one digit)
 */
static int numDigits(int num) {
	int digits=0;
	while(num) {
		num/=10;
		digits++;
	}
	return ((digits == 0) ? 1 : digits);
}

/*
 * Writes the days and hour fields into a string, with a '-' delimeter.
 */
static void fillDate(char *date, int days, int hour) {
	sprintf(date, "%d%c%d" , days, SEPARATE_FIELDS, hour);
}

/*
 * Updates the days-hour combination to be the very next hour that's valid for
 * the room (if we've reached the close time of a room - we have to move to
 * the next day!)
 */
static void dateNextRoomHour(Room room, char* date, int days, int hour) {
	assert(room && date);
	int open_time=0, close_time=0;
	roomGetOpenTime(room, &open_time);
	roomGetCloseTime(room, &close_time);
	if(hour == close_time-1) {
		hour = open_time;
		days++;
	}
	fillDate(date, days, hour);
}

/*
 * Returns the amount of reservations in the system that are scheduled for today
 */
static int systemTodaysReservations(EscapeTechnion sys) {
	int reservations_amount = 0;
	SET_FOREACH(Company, company, sys->companies)
		reservations_amount+=companyTodaysReservations(company);
	return reservations_amount;
}

/*
 * A reservation was sent to be printed, all of its data is here: the company
 * to which the room belongs, the room itself and the reservation.
 * We decipher all the relevant data for the print function thanks to the
 * ADT's getter functions, and send all of the data to be printed.
 */
static void printReservation(EscapeTechnion sys, Company company,
Room room, Reservation reservation) {
	assert(sys && company && room && reservation);
	char* escaper_email = NULL;
	char* company_email = NULL;
	TechnionFaculty escaper_faculty=UNKNOWN, room_faculty=UNKNOWN;
	int escaper_skill=0, room_id=0, reservation_hour=0,
	reservation_participants=0, reservation_price=0, room_difficulty=0;
	companyGetEmail(company, &company_email);
	reservationGetEscaperEmail(reservation, &escaper_email);
	Escaper escaper = getEscaperByEmail(sys, escaper_email);
	assert(escaper);
	escaperGetSkillLevel(escaper, &escaper_skill);
	escaperGetFaculty(escaper, &escaper_faculty);
	companyGetFaculty(company, &room_faculty);
	roomGetId(room, &room_id);
	roomGetDifficulty(room, &room_difficulty);
	reservation_hour = reservationGetHour(reservation);
	reservation_participants = reservationGetPeople(reservation);
	reservation_price = reservationGetPrice(reservation);
	mtmPrintOrder(sys->output_channel, escaper_email, escaper_skill,
	escaper_faculty, company_email, room_faculty, room_id, reservation_hour,
	room_difficulty, reservation_participants, reservation_price);
	reservationInvalidate(reservation);
}

/*
 * Gets two arrays: array of the earns of all faculties(faculties sorted by
 * indexes), and an array of the top (3) faculties. Updates the top faculties
 * array: each member holds the desired faculty, from best to worst according to
 * indexes order. The faculty earns array is left unchanged.
 */
static void systemFillTopFaculties(EscapeTechnion sys, int faculty_earns[],
TechnionFaculty best_faculties[]) {
	int max = 0;
	TechnionFaculty desired_faculty=UNKNOWN;
	int temporary_earns[BEST_FACULTIES_AMOUNT] = {0};
	for(int i=0; i<BEST_FACULTIES_AMOUNT; i++) {
		for(int curr_faculty = 0; curr_faculty<(int)UNKNOWN; curr_faculty++) {
			if(faculty_earns[curr_faculty] > max) {
				max = faculty_earns[curr_faculty];
				desired_faculty = (TechnionFaculty)curr_faculty;
			}
		}
		best_faculties[i] = desired_faculty;
		temporary_earns[i] = max;
		max = -1;
		faculty_earns[(int)desired_faculty] = -1;
	}
	for(int i=0; i<BEST_FACULTIES_AMOUNT; i++)
		faculty_earns[best_faculties[i]] = temporary_earns[i];
}

EscapeTechnion escapeTechnionCreate() {
	EscapeTechnion sys = malloc(sizeof(*sys));
	CHECK_VALID(sys, NULL)
	sys->companies = setCreate(setCompanyCopy, setCompanyFree,
	setCompanyCompare);
	if(!(sys->companies)) {
		free(sys);
		return NULL;
	}
	sys->escapers = setCreate(setEscaperCopy, setEscaperFree,
	setEscaperCompare);
	if(!(sys->escapers)) {
		setDestroy(sys->companies);
		free(sys);
		return NULL;
	}
	sys->days_passed = 0;
	sys->output_channel = stdout;
	return sys;
}

EscapeTechnion escapeTechnionCopy(EscapeTechnion sys) {
	CHECK_VALID(sys, NULL)
	EscapeTechnion sys_copy = escapeTechnionCreate();
	CHECK_VALID(sys_copy, NULL)
	sys_copy->companies = setCopy(sys->companies);
	if(!(sys_copy->companies)) {
		free(sys_copy);
		return NULL;
	}
	sys_copy->escapers = setCopy(sys->escapers);
	if(!(sys_copy->escapers)) {
		setDestroy(sys->companies);
		free(sys_copy);
		return NULL;
	}
	sys_copy->days_passed = sys->days_passed;
	sys_copy->output_channel = sys->output_channel;
	return sys_copy;
}

EscapeTechnionResult escapeTechnionModifyOutput(EscapeTechnion sys,
FILE* output_file) {
	CHECK_VALID(sys, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(output_file, ESCAPE_TECHNION_NULL_PARAMETER)
	if(sys->output_channel != stdout)
		fclose(sys->output_channel);
	sys->output_channel = output_file;
	return ESCAPE_TECHNION_SUCCESS;
}


EscapeTechnionResult escapeTechnionDestroy(EscapeTechnion sys) {
	CHECK_VALID(sys, ESCAPE_TECHNION_NULL_PARAMETER)
	setDestroy(sys->companies);
	setDestroy(sys->escapers);
	free(sys);
	return ESCAPE_TECHNION_SUCCESS;
}

EscapeTechnionResult escapeTechnionAddCompany(EscapeTechnion sys, char* email,
TechnionFaculty faculty) {
	CHECK_VALID(sys, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(email, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(isLegalEmail(email), ESCAPE_TECHNION_INVALID_PARAMETER)
	CHECK_VALID(((faculty>=0) && (faculty<UNKNOWN)),
	ESCAPE_TECHNION_INVALID_PARAMETER)
	char* target_email = NULL;
	SET_FOREACH(Escaper, escaper, sys->escapers) {
		assert(escaper);
		escaperGetEmail(escaper, &target_email);
		if(strcmp(email, target_email) == 0)
			return ESCAPE_TECHNION_EMAIL_ALREADY_EXISTS;
	}
	Company new_company = companyCreate(NULL, email, faculty);
	CHECK_VALID(new_company, ESCAPE_TECHNION_OUT_OF_MEMORY)
	SetResult result = setAdd(sys->companies, new_company);
	companyDestroy(new_company);
	if(result != SET_SUCCESS) {
		return(result == SET_OUT_OF_MEMORY) ? (ESCAPE_TECHNION_OUT_OF_MEMORY) :
		(ESCAPE_TECHNION_EMAIL_ALREADY_EXISTS);
	}
	return ESCAPE_TECHNION_SUCCESS;
}

EscapeTechnionResult escapeTechnionRemoveCompany(EscapeTechnion sys,
char* email) {
	CHECK_VALID(sys, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(email, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(isLegalEmail(email), ESCAPE_TECHNION_INVALID_PARAMETER)
	Company to_remove = getCompanyByEmail(sys, email);
	CHECK_VALID(to_remove, ESCAPE_TECHNION_COMPANY_EMAIL_DOES_NOT_EXIST)
	if(companyRoomsReserved(to_remove))
		return ESCAPE_TECHNION_RESERVATION_EXISTS;
	setRemove(sys->companies, to_remove);
	return ESCAPE_TECHNION_SUCCESS;
}

EscapeTechnionResult escapeTechnionAddRoom(EscapeTechnion sys, char* email,
int id, int price, int num_ppl, char* working_hours, int difficulty) {
	CHECK_VALID(sys, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(email, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(isLegalEmail(email), ESCAPE_TECHNION_INVALID_PARAMETER)
	CHECK_VALID(checkRoomParameters(id, price, num_ppl, difficulty,
	working_hours), ESCAPE_TECHNION_INVALID_PARAMETER)
	Company target_company = getCompanyByEmail(sys, email);
	CHECK_VALID(target_company, ESCAPE_TECHNION_COMPANY_EMAIL_DOES_NOT_EXIST);
	TechnionFaculty target_faculty = UNKNOWN, company_faculty = UNKNOWN;
	Room room = NULL;
	companyGetFaculty(target_company, &target_faculty);
	SET_FOREACH(Company, company, sys->companies) {
		assert(company);
		companyGetFaculty(company, &company_faculty);
		if(target_faculty == company_faculty) {
			companyGetRoom(company, &room, id);
			if(room)
				return ESCAPE_TECHNION_ID_ALREADY_EXIST;
		}
	}
	if(companyNewRoom(target_company, id, price, num_ppl, difficulty,
	working_hours) == COMPANY_OUT_OF_MEMORY)
		return ESCAPE_TECHNION_OUT_OF_MEMORY;
	return ESCAPE_TECHNION_SUCCESS;
}

EscapeTechnionResult escapeTechnionRemoveRoom(EscapeTechnion sys,
TechnionFaculty faculty, int id) {
	CHECK_VALID(sys, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID((faculty >= 0 && faculty < UNKNOWN),
	ESCAPE_TECHNION_INVALID_PARAMETER)
	CHECK_VALID((id>0), ESCAPE_TECHNION_INVALID_PARAMETER)
	Company desired_company = getCompanyByRoomId(sys, faculty, id);
	CHECK_VALID(desired_company, ESCAPE_TECHNION_ID_DOES_NOT_EXIST)
	if(companyRemoveRoom(desired_company, id) == COMPANY_ROOM_HAS_RESERVATIONS)
		return ESCAPE_TECHNION_RESERVATION_EXISTS;
	return ESCAPE_TECHNION_SUCCESS;
}

EscapeTechnionResult escapeTechnionAddEscaper(EscapeTechnion sys, char* email,
TechnionFaculty faculty, int skill_level) {
	CHECK_VALID(sys, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(email, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(isLegalEmail(email), ESCAPE_TECHNION_INVALID_PARAMETER)
	CHECK_VALID(((faculty>=0) && (faculty<UNKNOWN)),
	ESCAPE_TECHNION_INVALID_PARAMETER)
	CHECK_VALID(((skill_level > 0) && (skill_level<=MAX_LEVEL)),
	ESCAPE_TECHNION_INVALID_PARAMETER)
	char* target_email = NULL;
	SET_FOREACH(Company, company, sys->companies) {
		assert(company);
		companyGetEmail(company, &target_email);
		if(strcmp(email, target_email) == 0)
			return ESCAPE_TECHNION_EMAIL_ALREADY_EXISTS;
	}
	Escaper new_escaper = escaperCreate(NULL, email, faculty, skill_level);
	CHECK_VALID(new_escaper, ESCAPE_TECHNION_OUT_OF_MEMORY)
	SetResult result = setAdd(sys->escapers, new_escaper);
	escaperDestroy(new_escaper);
	if(result != SET_SUCCESS) {
		return(result == SET_OUT_OF_MEMORY) ? (ESCAPE_TECHNION_OUT_OF_MEMORY) :
		(ESCAPE_TECHNION_EMAIL_ALREADY_EXISTS);
	}
	return ESCAPE_TECHNION_SUCCESS;
}

EscapeTechnionResult escapeTechnionRemoveEscaper(EscapeTechnion sys,
char* email) {
	CHECK_VALID(sys, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(email, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(isLegalEmail(email), ESCAPE_TECHNION_INVALID_PARAMETER)
	Escaper to_remove = getEscaperByEmail(sys, email);
	CHECK_VALID(to_remove, ESCAPE_TECHNION_CLIENT_EMAIL_DOES_NOT_EXIST)
	int id = 0;
	SET_FOREACH(Company, company, sys->companies)
		COMPANY_FOREACH(room, company, id)
			roomRemoveEscaperReservations(room, email);
	setRemove(sys->companies, to_remove);
	return ESCAPE_TECHNION_SUCCESS;
}

EscapeTechnionResult escapeTechnionEscaperOrder(EscapeTechnion sys, char* email,
TechnionFaculty faculty, int id, char* time, int num_ppl) {
	CHECK_VALID(sys, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(email, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(time, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID((isLegalEmail(email)), ESCAPE_TECHNION_INVALID_PARAMETER)
	CHECK_VALID((id >= 0), ESCAPE_TECHNION_INVALID_PARAMETER)
	CHECK_VALID((num_ppl > 0), ESCAPE_TECHNION_INVALID_PARAMETER)
	CHECK_VALID(((faculty>=0)&&(faculty<UNKNOWN)),
	ESCAPE_TECHNION_INVALID_PARAMETER)
	Escaper desired_escaper = getEscaperByEmail(sys, email);
	CHECK_VALID(desired_escaper, ESCAPE_TECHNION_CLIENT_EMAIL_DOES_NOT_EXIST)
	Room desired_room = getFacultyRoomById(sys, id, faculty);
	CHECK_VALID(desired_room, ESCAPE_TECHNION_ID_DOES_NOT_EXIST)
	int room_open=0, room_close=0;
	roomGetOpenTime(desired_room, &room_close);
	roomGetCloseTime(desired_room, &room_close);
	CHECK_VALID((checkReservationTime(time, room_open, room_close)),
	ESCAPE_TECHNION_INVALID_PARAMETER)
	EscaperResult order_result;
	order_result = escaperMakeReservation(desired_escaper, desired_room,
	faculty, num_ppl, time);
	switch(order_result) {
	//There are multiple possibilities here. We return the correct error.
		case ESCAPER_OUT_OF_MEMORY:
			return ESCAPE_TECHNION_OUT_OF_MEMORY;
			break;
		case ESCAPER_IN_ROOM:
			return ESCAPE_TECHNION_CLIENT_IN_ROOM;
		case ESCAPER_RESERVED_ROOM:
			return ESCAPE_TECHNION_ROOM_NOT_AVAILABLE;
		default:
			return ESCAPE_TECHNION_SUCCESS;
	}
}

EscapeTechnionResult escapeTechnionEscaperRecommend(EscapeTechnion sys,
char* email, int num_ppl) {
	CHECK_VALID(sys, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(email, ESCAPE_TECHNION_NULL_PARAMETER)
	CHECK_VALID(isLegalEmail(email), ESCAPE_TECHNION_INVALID_PARAMETER)
	CHECK_VALID(((num_ppl)>0),ESCAPE_TECHNION_INVALID_PARAMETER)
	int skill_level = 0, room_id = 0, days = 0, hour = 0;
	TechnionFaculty escaper_faculty = UNKNOWN, room_faculty = UNKNOWN;
	Escaper escaper = getEscaperByEmail(sys, email);
	if(!escaper)
		return ESCAPE_TECHNION_CLIENT_EMAIL_DOES_NOT_EXIST;
	escaperGetSkillLevel(escaper, &skill_level);
	escaperGetFaculty(escaper, &escaper_faculty);
	Room room = recommendedRoom(sys, skill_level, num_ppl, escaper_faculty,
	&room_faculty);
	if(!room)
		return ESCAPE_TECHNION_NO_ROOMS_AVAILABLE;
	roomGetId(room, &room_id);
	//That's the earliest days-hour combination when the room isn't busy
	roomGetEarliestTimeAvailable(room, &days, &hour);
	int date_size = numDigits(days) + numDigits(hour) + 1;
	char* date = malloc(sizeof(date_size)+1);
	CHECK_VALID(date, ESCAPE_TECHNION_OUT_OF_MEMORY);
	date[date_size] = 0;
	fillDate(date, days, hour);
	while(escaperIsBusy(escaper, ((hour*HOURS_IN_A_DAY) + days))) {
		//Uh-oh! The escaper is busy at the best possible time for the room!
		//We try the next best option again and again until we succeed.
		dateNextRoomHour(room, date, days, hour);
		roomGetEarliestTimeAvailable(room, &days, &hour);
		fillDate(date, days, hour);
	}
	if(escaperMakeReservation(escaper, room,
	room_faculty, num_ppl, date) != ESCAPER_SUCCESS) {
		//A memory problem has occurred when we created the reservation.
		free(date);
		return ESCAPE_TECHNION_OUT_OF_MEMORY;
	}
	free(date);
	return ESCAPE_TECHNION_SUCCESS;
}

EscapeTechnionResult escapeTechnionReportDay(EscapeTechnion sys) {
	CHECK_VALID(sys, ESCAPE_TECHNION_NULL_PARAMETER)
	int id=0, total_res = systemTodaysReservations(sys);
	mtmPrintDayHeader(sys->output_channel, sys->days_passed, total_res);
	TechnionFaculty faculty;
	Reservation curr_reservation;
	for(int curr_time = 0; curr_time<HOURS_IN_A_DAY; curr_time++) {
	//We iterate over all the possible hours, from 0 to 23...
		for(TechnionFaculty curr_faculty = 0; curr_faculty<UNKNOWN;
		//We iterate over all the possible faculties, from lowest to highest...
		curr_faculty++) {
			SET_FOREACH(Company, curr_company, sys->companies) {
				//We iterate over all the companies in the system...
				companyGetFaculty(curr_company, &faculty);
				if(faculty == curr_faculty) {
					//We found a company of the desired faculty!
					COMPANY_FOREACH(curr_room, curr_company, id) {
						//We iterate over all the rooms in the company
						//(iteration is performed from lowest id to highest)
						curr_reservation = roomGetReservationInTime
						(curr_room, curr_time);
						if(reservationIsValid(curr_reservation)) {
							//We found a reservation that's for this hour and
							//belongs to this room. We send it to be printed.
							printReservation(sys, curr_company, curr_room,
							curr_reservation);
						}
					}
				}
			}
		}
	}
	mtmPrintDayFooter(sys->output_channel, sys->days_passed);
	SET_FOREACH(Company, company, sys->companies)
		companyNextDay(company);
	SET_FOREACH(Escaper, escaper, sys->escapers)
		escaperNextDay(escaper);
	(sys->days_passed)++;
	return ESCAPE_TECHNION_SUCCESS;
}

EscapeTechnionResult escapeTechnionReportBest(EscapeTechnion sys) {
	assert((int)UNKNOWN >= BEST_FACULTIES_AMOUNT);
	int faculty_earns[(int)UNKNOWN] = {0};
	int total_system_earns = 0;
	TechnionFaculty best_faculties[BEST_FACULTIES_AMOUNT];
	for(int i = 0; i<BEST_FACULTIES_AMOUNT; i++)
		best_faculties[i] = i;
	TechnionFaculty target_faculty = UNKNOWN;
	for(int curr_faculty = 0; curr_faculty<(int)UNKNOWN; curr_faculty++){
		SET_FOREACH(Company, company, sys->companies) {
			companyGetFaculty(company, &target_faculty);
			if((int)target_faculty == curr_faculty)
				faculty_earns[curr_faculty] += companyEarns(company);
		}
		total_system_earns += faculty_earns[curr_faculty];
	}
	mtmPrintFacultiesHeader(sys->output_channel, (int)UNKNOWN, sys->days_passed,
	total_system_earns);
	systemFillTopFaculties(sys, faculty_earns, best_faculties);
	for(int i=0; i<BEST_FACULTIES_AMOUNT; i++)
		mtmPrintFaculty(sys->output_channel, best_faculties[i],
		faculty_earns[((int)best_faculties[i])]);
	mtmPrintFacultiesFooter(sys->output_channel);
	return ESCAPE_TECHNION_SUCCESS;
}
