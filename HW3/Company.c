#include "Company.h"

struct Company_t {
	char* email; //The company's email.
	TechnionFaculty faculty; //The faculty to which this company belongs.
	Set rooms; //Set of rooms that belong to this company.
};

//A valid email must contain exactly one '@'
#define EMAIL_SIGN '@'

#define NOT_INITIALIZED -1

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

static bool isLegalEmail(char* email);
static SetElement copyRoom(SetElement room);
static void freeRoom(SetElement room);
static int roomsCompare(SetElement room1, SetElement room2);

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
 * A static function that defines the company's set's copy function.
 * The copy function is required to get an element of type SetElement, and
 * return an element of the same type.
 * The copy function is defined as follows: The set's elements are of the type
 * "Room", so we cast the element to be its real type, and then return the
 * result of the room's copy function.
 */
static SetElement copyRoom(SetElement room){
	return roomCopy((Room)(room));
}

/*
 * A static function that defines the company's set's free function.
 * The free function is required to get an element of type SetElement, and
 * return void. The free function is defined as follows: the set's elements are
 * of the type "Room", so we cast the element to be its real type, and then
 * use the room's destroy function.
 */
static void freeRoom(SetElement room) {
	roomDestroy((Room)(room));
}

/*
 * A static function that defines the company's set's compare function.
 * The compare function is required to get two elements of type SetElement, and
 * return an integer.
 * The compare function is defined as follows: the set's elements are of the
 * type "Room", so we cast the elements to be their real type. Then, we consider
 * the rooms' ids, and return the subtraction between the two rooms' ids.
 */
static int roomsCompare(SetElement room1, SetElement room2) {
    int id1=0, id2=0;
    roomGetId((Room)room1, &id1);
    roomGetId((Room)room2, &id2);
    return id1-id2;
}

Company companyCreate(CompanyResult* result, char* email,
TechnionFaculty faculty) {
	if(!email) {
		INIT_IF_EXISTS(result, COMPANY_NULL_PARAMETER);
		return NULL;
	}
	if(!isLegalEmail(email)) {
		INIT_IF_EXISTS(result, COMPANY_INVALID_PARAMETER);
		return NULL;
	}
	if(!(faculty>=0 && faculty<UNKNOWN)) {
		INIT_IF_EXISTS(result, COMPANY_INVALID_PARAMETER);
		return NULL;
	}
	//input is valid!
	Company company = malloc(sizeof(*company));
	if(!company) {
		INIT_IF_EXISTS(result, COMPANY_OUT_OF_MEMORY);
		return NULL;
	}
	company->email = malloc(strlen(email)+1);
	if(!(company->email)) {
		free(company);
		INIT_IF_EXISTS(result, COMPANY_OUT_OF_MEMORY);
		return NULL;
	}
	strcpy(company->email, email);
	company->rooms = setCreate(copyRoom, freeRoom, roomsCompare);
	if(!(company->rooms)) {
		free(company->email);
		free(company);
		INIT_IF_EXISTS(result, COMPANY_OUT_OF_MEMORY);
		return NULL;
	}
	company->faculty = faculty;
	INIT_IF_EXISTS(result, COMPANY_SUCCESS);
	return company;
}

CompanyResult companyDestroy(Company company){
	CHECK_VALID(company, COMPANY_NULL_PARAMETER)
	setDestroy(company->rooms);
	free(company->email);
	free(company);
	return COMPANY_SUCCESS;
}

Company companyCopy(Company company){
	CHECK_VALID(company, NULL)
	Company company_copy = companyCreate
	(NULL, company->email, company->faculty);
	CHECK_VALID(company_copy, NULL)
	setDestroy(company_copy->rooms);
	company_copy->rooms = setCopy(company->rooms);
	if(!company_copy->rooms) {
		free(company_copy);
		return NULL;
	}
	company_copy->faculty = company->faculty;
	return company_copy;
}

CompanyResult companyGetEmail(Company company, char** email){
	CHECK_VALID(company, COMPANY_NULL_PARAMETER)
	CHECK_VALID(email, COMPANY_NULL_PARAMETER)
	*email = company->email;
	return COMPANY_SUCCESS;
}


CompanyResult companyGetFaculty(Company company, TechnionFaculty* faculty){
	CHECK_VALID(company, COMPANY_NULL_PARAMETER)
	CHECK_VALID(faculty, COMPANY_NULL_PARAMETER)
	*faculty= company->faculty;
	return COMPANY_SUCCESS;
}

int companyEarns(Company company) {
	CHECK_VALID(company, 0)
	int company_earns = 0, room_earns = 0;
	SET_FOREACH(Room, room, company->rooms) {
		roomEarns(room, &room_earns);
		company_earns += room_earns;
	}
	return company_earns;
}

CompanyResult companyNewRoom(Company company, int id, int entry_fee,
int recommended_people, int difficulty, char* working_hours) {
	CHECK_VALID(company, COMPANY_NULL_PARAMETER)
	CHECK_VALID(working_hours, COMPANY_NULL_PARAMETER)
	RoomResult room_result;
	Room room = roomCreate(&room_result, id, entry_fee, recommended_people,
	difficulty, working_hours);
	if(room_result != ROOM_SUCCESS)
		//There are two possibilities for failure here. We return the correct
		//error code.
		return ((room_result == ROOM_OUT_OF_MEMORY) ?
		(COMPANY_OUT_OF_MEMORY) : (COMPANY_INVALID_PARAMETER));
	SetResult set_result = setAdd(company->rooms, room);
	roomDestroy(room);
	if(set_result != SET_SUCCESS)
		//There are two possibilities for failure here. We return the correct
		//error code.
		return(set_result == SET_OUT_OF_MEMORY) ?
		(COMPANY_OUT_OF_MEMORY) : (COMPANY_ROOM_ID_EXISTS);
	return COMPANY_SUCCESS;
}

CompanyResult companyRemoveRoom(Company company, int id){
	CHECK_VALID(company, COMPANY_NULL_PARAMETER)
	int target_id = 0;
	SET_FOREACH(Room, remove_room, company->rooms) {
		assert(remove_room);
		roomGetId(remove_room, &target_id);
		if(target_id == id) {
			if(!roomHasReservations(remove_room)) {
				setRemove(company->rooms, remove_room);
				return COMPANY_SUCCESS;
			}
			else
				return COMPANY_ROOM_HAS_RESERVATIONS;
		}
	}
	return COMPANY_ILLEGAL_ID;
}

CompanyResult companyNextDay(Company company) {
	CHECK_VALID(company, COMPANY_NULL_PARAMETER)
	SET_FOREACH(Room, room, company->rooms) {
		assert(room);
		roomNextDay(room);
	}
	return COMPANY_SUCCESS;
}

int companyTodaysReservations(Company company) {
	CHECK_VALID(company, 0)
	int reservations_amount = 0;
	SET_FOREACH(Room, room, company->rooms)
		reservations_amount+=roomTodaysReservations(room);
	return reservations_amount;
}

CompanyResult companyRoomsAmount(Company company, int* size) {
	CHECK_VALID(company, COMPANY_NULL_PARAMETER)
	CHECK_VALID(size, COMPANY_NULL_PARAMETER)
	*size = setGetSize(company->rooms);
	return COMPANY_SUCCESS;
}

CompanyResult companyGetRoom(Company company, Room* room, int id) {
	CHECK_VALID(company, COMPANY_NULL_PARAMETER)
	CHECK_VALID(room, COMPANY_NULL_PARAMETER)
	int target_id = 0;
	SET_FOREACH(Room, target_room, company->rooms) {
		assert(target_room);
		roomGetId(target_room, &target_id);
		if(target_id == id) {
			*room = target_room;
			return COMPANY_SUCCESS;
		}
	}
	return COMPANY_ILLEGAL_ID;
}

bool companyRoomsReserved(Company company) {
	CHECK_VALID(company, false)
	SET_FOREACH(Room, room, company->rooms) {
		if(roomHasReservations(room))
			return true;
	}
	return false;
}

Room companyLowestIdRoom(Company company, int* desired_id) {
	CHECK_VALID(company, NULL)
	CHECK_VALID(desired_id, NULL)
	int min = NOT_INITIALIZED, id = NOT_INITIALIZED;
	Room desired_room = setGetFirst(company->rooms);
	roomGetId(desired_room, &min);
	if(min == NOT_INITIALIZED) {
		//No rooms in the company!
		*desired_id = NOT_INITIALIZED;
		return NULL;
	}
	SET_FOREACH(Room, room, company->rooms) {
		//We find the room with the lowest id
		roomGetId(room, &id);
		if(id < min) {
			desired_room = room;
			min = id;
		}
	}
	return desired_room;
}

Room companyNextRoom(Company company, int* id) {
	CHECK_VALID(company, NULL)
	CHECK_VALID(id, NULL)
	int min = NOT_INITIALIZED, curr_id = NOT_INITIALIZED, input_id = (*id);
	Room desired_room = NULL;
	SET_FOREACH(Room, room, company->rooms) {
		roomGetId(room, &min);
		if(min > input_id) {
			//Found a room with an id that's bigger than the parameter id.
			desired_room = room;
			break;
		}
	}
	if(!desired_room) {
		//All the rooms in this company have an id that's smaller than the
		//parameter id!
		*id = NOT_INITIALIZED;
		return NULL;
	}
	*id = min;
	//There are acceptable rooms in this company
	SET_FOREACH(Room, room, company->rooms) {
		roomGetId(room, &curr_id);
		if((curr_id > input_id) && (curr_id < min)) {
			desired_room = room;
			min = curr_id;
		}
	}
	*id = min;
	return desired_room;
}
