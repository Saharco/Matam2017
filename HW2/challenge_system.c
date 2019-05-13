#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "challenge_system.h"

#define MAX_LENGTH 50

//----------STATIC FUNCTIONS DECLARATIONS----------
static Result system_read_data (ChallengeRoomSystem *sys, FILE* file);
static Challenge* get_challenge_by_id (ChallengeRoomSystem *sys, int id);
static ChallengeRoom* get_room_by_name (ChallengeRoomSystem *sys,
char *room_name);
static void system_abort_creation (ChallengeRoomSystem *sys, FILE* file);
static Result init_system (ChallengeRoomSystem *sys, char *sys_name);
static void delete_visitor_from_list (ChallengeRoomSystem *sys,int visitor_id,
int quit_time);
static int system_visitor_occupied (ChallengeRoomSystem *sys,
Visitor **visitor, int visitor_id);
static Result system_fastest_challenge (ChallengeRoomSystem *sys,
char **challenge_name);
//-------END OF STATIC FUNCTIONS DECLARATIONS-------


//---------STATIC FUNCTIONS IMPLEMENTATIONS---------


/* Gets a pointer to the system and a pointer to the configuration file. Reads all the data in the file
 * into the system's fields (assuming the file's format).
 * Will return OK if successful, otherwise will return an error: null parameter or
 * memory problem: can't allocate all the information for the system's fields.
 * NOTE: if OK isn't returned from this function - the create_system function will free
 * all the relevant fields that were previously allocated.
 * NOTE: the file is closed outside of this function, in create_system. */
static Result system_read_data (ChallengeRoomSystem *sys, FILE* file) {
	if(!sys || !file) return NULL_PARAMETER;
	char *name_data = malloc(MAX_LENGTH+1);
	if(!name_data) return MEMORY_PROBLEM;
	int id = 0, level = 0, room_challenges = 0;
	Result res = OK;
	fscanf(file, "%s\n", name_data);
	res = init_system(sys, name_data);
	if(res!=OK) return res;
	fscanf(file, "%d\n", &sys->challenges_amount);
	sys->challenges = malloc(sys->challenges_amount * sizeof(Challenge));
	if(!sys->challenges) return MEMORY_PROBLEM;
	for(int i=0; i<sys->challenges_amount; i++) {
		fscanf(file, "%s %d %d\n", name_data, &id, &level);
		res = init_challenge(&(sys->challenges[i]), id, name_data, level-1);
		if(res!=OK) return res;
	}
	fscanf(file, "%d\n", &sys->rooms_amount);
	sys->rooms = malloc(sys->rooms_amount * sizeof(ChallengeRoom));
	if(!sys->rooms) return MEMORY_PROBLEM;
	for(int i=0; i<sys->rooms_amount; i++) {
		fscanf(file, "%s %d", name_data, &room_challenges);
		res = init_room(&(sys->rooms[i]), name_data, room_challenges);
		if(res!=OK) return res;
		for(int j=0; j<room_challenges; j++) {
			if(j==room_challenges-1) fscanf(file, " %d\n", &id);
			else fscanf(file, " %d", &id);
			res = init_challenge_activity(&(sys->rooms[i].challenges[j]),
			get_challenge_by_id(sys, id));
			if(res!=OK) return res;
		}
	}
	free(name_data);
	return OK; //If we get here - the system has been initialized successfully!
}

/* Gets a pointer to the system and an id, and returns a pointer to a challenge in
 * the system that has the required ID. Will return null if no challenge was found. */
static Challenge* get_challenge_by_id (ChallengeRoomSystem *sys, int id) {
	if(!sys) return NULL;
	for(int i=0; i<sys->challenges_amount; i++)
		if(sys->challenges[i].id == id) return &sys->challenges[i];
	return NULL;
}

/* Gets a pointer to the system and a room's name. Return a pointer to a room in the system
 * with the required name. Will return null if no such room was found in the system. */
static ChallengeRoom* get_room_by_name (ChallengeRoomSystem *sys,
char *room_name) {
	if(!sys) return NULL;
	for(int i=0; i<sys->rooms_amount; i++)
		if(strcmp(sys->rooms[i].name, room_name) == 0) return &sys->rooms[i];
	return NULL;
}

/* Gets a pointer to the system and a pointer to the configuration file.
 * Closes the file, and then calls upon the destroy_system function to free all
 * the allocated fields in the system.
 * NOTE: two strings are being sent into the destroy_system function, but their
 * values will be NULL after the end of this process because the system is guaranteed to be
 * empty of visitors (not created successfully), so they are NOT being freed! */
static void system_abort_creation (ChallengeRoomSystem *sys, FILE* file) {
	fclose(file);
	char *param1 = NULL;
	char *param2 = NULL;
	destroy_system(sys, sys->time_of_last_action, &param1, &param2);
}

/* Gets a pointer to the system and a string. Allocates a copy of the parameter string
 * and assigns it to be the system's name. The rest of the system's fields are initialized to either 0 or null.
 * Will return OK if successful, otherwise will return an error: null parameter or memory problem
 * (can't allocate the system's name). */
static Result init_system (ChallengeRoomSystem *sys, char *sys_name) {
	if(!sys || !sys_name) return NULL_PARAMETER;
	sys->visitors = NULL;
	sys->time_of_last_action = 0; //The newly created system's first action time mark: 0.
	sys->challenges_amount = 0;
	sys->rooms_amount = 0;
	sys->challenges = NULL;
	sys->rooms = NULL;
	sys->name = NULL; //To prevent uninitialized values in case of malloc failure.
	//maloc: store the system's name
	char *sys_name_copy = malloc(strlen(sys_name)+1);
	if(!sys_name_copy) return MEMORY_PROBLEM; //malloc failed
	strcpy(sys_name_copy, sys_name);
	sys->name = sys_name_copy;
	return OK;
}

/* Gets a pointer to the system, a visitor's id and the visitor's quit time.
 * Removes the visitor from the system's list, and in coordination with the other system's
 * mechanisms - updates all the relevant fields upon the visitor's removal */
static void delete_visitor_from_list (ChallengeRoomSystem *sys, int visitor_id,
int quit_time) {
	VisitorsList *to_delete = sys->visitors;
	VisitorsList *node = NULL;
	while(to_delete->visitor->visitor_id != visitor_id) {
		node = to_delete;
		to_delete = to_delete->next;
	}
	if(!node)
		sys->visitors = sys->visitors->next;
	else
		node->next = node->next->next;
	visitor_quit_room(to_delete->visitor, quit_time);
	free(to_delete->visitor);
	free(to_delete);
}

/* Gets a pointer to the system, the address of a pointer to a visitor and the visitor's ID.
 * Checks if a visitor with the given ID already exists in the system.
 * If such visitor exists - the address of the visitor pointer will be assigned to be that visitor,
 * and the function returns 1. Otherwise, returns 0 (visitor pointer unchanged). */
static int system_visitor_occupied (ChallengeRoomSystem *sys,
Visitor **visitor, int visitor_id) {
	if(!sys->visitors) return 0;
	VisitorsList *node = sys->visitors;
	while(node) {
		if(node->visitor->visitor_id == visitor_id) {
			*visitor = node->visitor;
			return 1;
		}
		node = node->next;
	}
	return 0;
}

/* Gets a pointer to the system and a pointer to a string.
 * Finds the fastest challenge (challenge with the smallest non-zero "best time")
 * and allocates a copy of that challenge's name, which is then assigned to be the string's value.
 * If no visitors have ever attended the system (all challenges have a clear time of 0),
 * the value of the string will be null.
 * Will return OK if successful, otherwise returns an error message: null parameter
 * or memory problem (can't allocate the challenge's name). */
static Result system_fastest_challenge (ChallengeRoomSystem *sys,
char **challenge_name) {
	if(!sys || !challenge_name) return NULL_PARAMETER;
	int min_time = sys->challenges[0].best_time;
	int time = min_time, challenge_index = 0;
	for(int i=1; i<sys->challenges_amount; i++) {
		best_time_of_system_challenge(sys, sys->challenges[i].name, &time);
		if((time) && ((!min_time) || (time < min_time))) {
			min_time = time;
			challenge_index = i;
		}
		else if((time == min_time) && (strcmp
		(sys->challenges[challenge_index].name, sys->challenges[i].name) > 0))
			sys->challenges[challenge_index].name = sys->challenges[i].name;
	}
	if(!min_time) {
		//No visitors in the system: the fastest challenge's name is "null"
		*challenge_name = NULL;
	}
	else {
		//malloc: hold the challenge's name
		char *required_challenge_copy =
		malloc(strlen(sys->challenges[challenge_index].name)+1);
		if(!required_challenge_copy) return MEMORY_PROBLEM; //malloc failed
		strcpy(required_challenge_copy, sys->challenges[challenge_index].name);
		*challenge_name = required_challenge_copy;
	}
	return OK;
}
//------END OF STATIC FUNCTIONS IMPLEMENTATIONS------



/* Gets the configuration file's name and the address of a pointer to the system.
 * Creates the system with the given data in the configuration file (assuming the file's format is legitimate).
 * Updates all the relevant fields in the system: the challenges array's length and the challenges'
 * names and IDs, the rooms array's length, the names of the rooms and the supported challenges in each room,
 * and the rest of the fields are initialized to either 0 or null.
 * Will return OK if successful, otherwise will return an error: null parameter,
 * or memory problem(can't allocate all of the required fields), in which case the creation
 * process will be terminated and all previously allocated fields will be freed. */
Result create_system (char* init_file, ChallengeRoomSystem **sys) {
	if(!init_file || !sys) return NULL_PARAMETER;
	*sys = malloc(sizeof(ChallengeRoomSystem));
	if(!*sys) return MEMORY_PROBLEM;
	FILE* file = fopen(init_file, "r");
	if(!file) return NULL_PARAMETER;
	Result creation_result = system_read_data(*sys, file);
	if(creation_result!=OK) {
		//Insufficient memory to initialize the system: UNDO creation process
		system_abort_creation(*sys, file); //(closes the file, too!)
		return creation_result;
	}
	fclose(file);
	return OK; //System created successfully!
}

/* Gets a pointer to a system, the destroy time and the addresses of two strings.
 * All visitors inside the systems' rooms are being taken out,
 * and then the the input parameter strings receive statistics values about the system:
 * the most popular challenge and the challenge that was completed faster than all the others.
 * If the two strings can't be allocated: their values will not be changed. If there were never any
 * visitors in the system then their values are null.
 *  Then, the destruction process starts: all allocated fields are being freed one by one and in order,
 *  and finally, the system as a whole is freed and its pointer points to null.
 *  Returns OK if successful, otherwise returns an error: null parameter, or illegal
 *  time if the destroy time is not valid (smaller than the system's time), in which case the system
 *  will not be destroyed and the parameter string values will not be distributed. */
Result destroy_system(ChallengeRoomSystem *sys, int destroy_time,
char **most_popular_challenge_p, char **challenge_best_time) {
	if(!sys || !most_popular_challenge_p || !challenge_best_time)
		return NULL_PARAMETER;
	if(sys->time_of_last_action > destroy_time) return ILLEGAL_TIME; //Invalid time: opposes time continuity!
	all_visitors_quit(sys, destroy_time); //freed all the visitors' lists' nodes and all the visitors (including their names)!
	free(sys->visitors); //freed visitors' list!
	most_popular_challenge(sys, most_popular_challenge_p); //Get statistics: most popular challenge
	system_fastest_challenge(sys, challenge_best_time);//Get statistics: fastest challenge
	for(int i=0; i<sys->rooms_amount; i++)
	{
		//Free all the rooms in the system
		for(int j=0; j<sys->rooms[i].num_of_challenges; j++)
			reset_challenge_activity(&(sys->rooms[i].challenges[j]));
		free(sys->rooms[i].challenges); //freed challenge activities array!
		reset_room(&(sys->rooms[i])); //freed room's name!
	}
	free(sys->rooms); //freed the array of rooms!
	for(int i=0; i<sys->challenges_amount; i++)
		//Free all the challenges in the system
		reset_challenge(&(sys->challenges[i])); //freed the challenges' names!
	free(sys->challenges); //freed the challenges array!
	free(sys->name); //freed the system's name!
	free(sys); //freed the system!
	sys = NULL;
	return OK;
}

/* Gets a pointer to the system, room's name, a visitor's name, the visitor's ID,
 * the visitor's required level and the visitor's start time. Enters the visitor at start time
 * into a non-occupied challenge in the required room that meets the level criteria of the visitor.
 * If several challenges exist - the smallest one in lexicographic order is chosen.
 * If a visitor with the same ID already exists in the system - the visitor will not be
 * assigned to any room and will not enter the system's circulation. All relevant
 * fields following the visitor's entry to the room are updated.
 * Will return OK if successful, otherwise will return an error: null parameter,
 * illegal time (starting time is smaller than the system's time), memory problem
 * (can't allocate the visitor / visitor node), already in room
 * (if the visitor is currently attending another challenge), or
 * no available challenges if no acceptable challenge was found. */
Result visitor_arrive(ChallengeRoomSystem *sys, char *room_name,
char *visitor_name, int visitor_id, Level level, int start_time) {
	if(!sys) return NULL_PARAMETER;
	if((sys->time_of_last_action) > start_time) return ILLEGAL_TIME;
	if(!room_name || !visitor_name) return ILLEGAL_PARAMETER;
	VisitorsList *node = malloc(sizeof(*node));
	if(!node) return MEMORY_PROBLEM;
	Visitor *visitor = NULL;
	if(!(system_visitor_occupied(sys, &visitor, visitor_id))) {
		visitor = malloc(sizeof(*visitor));
		if(!visitor) {
			free(node);
			return MEMORY_PROBLEM;
		}
		if(init_visitor(visitor, visitor_name, visitor_id)!=OK) {
			free(node);
			free(visitor);
			return MEMORY_PROBLEM;
		}
	}
	Result res = visitor_enter_room
	(get_room_by_name(sys, room_name), visitor, level, start_time);
	if(res!=OK) return res;
	node->visitor = visitor;
	node->next = sys->visitors;
	sys->visitors = node;
	sys->time_of_last_action = start_time;
	return OK;
}

/* Gets a pointer to the system, a visitor's ID and the visitor's quit time.
 * Removes the visitor from the room, clears the room's challenge activity
 * and removes the visitor from the system at quit time.
 * Will return OK if successful, otherwise will return an error: null parameter,
 * illegal time (visitor's quit time is smaller than the system's time) or
 * not in room (a visitor with the given ID does not exist in the system) */
Result visitor_quit(ChallengeRoomSystem *sys, int visitor_id, int quit_time) {
	if(!sys) return NULL_PARAMETER;
	if(sys->time_of_last_action > quit_time) return ILLEGAL_TIME;
	VisitorsList *node = sys->visitors;
	while((node) && node->visitor->visitor_id != visitor_id)
		node = node->next;
	if(!node)
		return NOT_IN_ROOM;
	delete_visitor_from_list(sys, visitor_id, quit_time);
	sys->time_of_last_action = quit_time;
	return OK;
}

/* Gets a pointer to the system and a quit time. Removes all the visitors that are currently
 * in the system from their respective rooms and challenges, updates all the relevant
 * fields resulting in this action. All visitors leave at the same time (quit time).
 * Will return OK if successful, otherwise will return an error: null parameter
 * or illegal time (quit time is smaller than the system's time). */
Result all_visitors_quit(ChallengeRoomSystem *sys, int quit_time) {
	if(!sys) return NULL_PARAMETER;
	if(sys->time_of_last_action > quit_time) return ILLEGAL_TIME;
	VisitorsList *curr_visitor = sys->visitors;
	while(curr_visitor) {
		visitor_quit_room(curr_visitor->visitor, quit_time);
		curr_visitor = curr_visitor->next;
	}
	while(sys->visitors) {
		VisitorsList *to_delete = sys->visitors;
		sys->visitors = sys->visitors->next;
		free(to_delete->visitor);
		free(to_delete);
	}
	return OK;
}

/* Gets a pointer to the system, a visitor's name and a pointer to a string.
 * Finds the room where the visitor with the given name is staying, allocates the room's name
 * into a copy and assigns parameter string's value to be that copy. If there is no
 * such visitor in the system - the value of the parameter string will not change.
 * Will return OK if successful, otherwise will return an error: null parameter,
 * illegal parameter, not in room (if the visitor is not in the system) or
 * memory problem (can't allocate the room's name copy). */
Result system_room_of_visitor(ChallengeRoomSystem *sys,
char *visitor_name, char **room_name) {
	if(!sys) return NULL_PARAMETER;
	if(!room_name || !visitor_name) return ILLEGAL_PARAMETER;
	VisitorsList *node = sys->visitors;
	while((node) && strcmp(node->visitor->visitor_name, visitor_name) != 0)
		node = node->next;
	if(!node) return NOT_IN_ROOM;
	return room_of_visitor(node->visitor, room_name);
}

/* Gets a pointer to the system, a challenge's ID and a name (string).
 * Allocates the name into a separate copy, and then finds a challenge in the system
 * with the given name and changes its current name to the newly allocated name.
 * Will return OK if successful, otherwise will return an error: null parameter,
 * memory problem(can't allocate the new name), or illegal parameter (no such challenge exists in the system) */
Result change_challenge_name(ChallengeRoomSystem *sys, int challenge_id,
char *new_name) {
	if(!sys || !new_name) return NULL_PARAMETER;
	for(int i=0; i<sys->challenges_amount; i++)
		if(sys->challenges[i].id == challenge_id)
			return change_name(&sys->challenges[i], new_name);
	return ILLEGAL_PARAMETER;
}

/* Gets a pointer to the system, a room's name and a new name.
 * Changes the room in the system with the same name to be the new name given (if it exists).
 * Will return OK if successful, otherwise will return an error: null parameter,
 * memory problem (can't allocate new name) or illegal parameter (no such room exists in the system) */
Result change_system_room_name(ChallengeRoomSystem *sys, char *current_name,
char *new_name) {
	if(!sys || !current_name || !new_name) return NULL_PARAMETER;
	for(int i=0; i<sys->rooms_amount; i++) {
		if(!strcmp(sys->rooms[i].name, current_name))
			return change_room_name(&sys->rooms[i], new_name);
	}
	return ILLEGAL_PARAMETER;
}

/* Gets a pointer to the system, a challenge's name and the address of an integer.
 * Finds the challenge with the shortest global clear time, and assigns the best
 * time to be the value of that integer parameter.
 * Will return OK if successful, otherwise will return an error: null parameter,
 * or illegal parameter if no such challenge exists. */
Result best_time_of_system_challenge(ChallengeRoomSystem *sys,
char *challenge_name, int *time) {
	if(!sys || !challenge_name || !time) return NULL_PARAMETER;
	if(!(sys->time_of_last_action)) {
		*time = 0;
		return OK;
	}
	for(int i=0; i<sys->challenges_amount; i++)
		if(strcmp(sys->challenges[i].name, challenge_name) == 0)
			return best_time_of_challenge(&(sys->challenges[i]), time);
	return ILLEGAL_PARAMETER;
}

/* Gets a pointer to the system and the address of a string. Finds the most
 * popular challenge in the system (the challenge with the highest amount of visits),
 * and assigns a copy of its name to be the parameter string's value. If no challenges are found
 * or no visitors have ever been to the system - the value of the parameter string will be null.
 * Will return OK if successful, otherwise will return an error: null parameter,
 * memory problem (can't allocate name copy).
 */
Result most_popular_challenge(ChallengeRoomSystem *sys, char **challenge_name) {
	if(!sys || !challenge_name) return NULL_PARAMETER;
	int visits = 0, max_visits = 0, popular_challenge_index = 0;
	for(int i=0; i<sys->challenges_amount; i++) {
		num_visits(&(sys->challenges[i]), &visits);
		if(max_visits < visits) {
			max_visits = visits;
			popular_challenge_index = i;
		}
		else if((visits!=0) && (max_visits == visits) && (strcmp(sys->
		challenges[popular_challenge_index].name, sys->challenges[i].name) > 0))
			popular_challenge_index = i;
	}
	if(!max_visits) {
		*challenge_name = NULL;
		return OK;
	}
	char *popular_challenge_copy =
	malloc(strlen(sys->challenges[popular_challenge_index].name)+1);
	if(!popular_challenge_copy) return MEMORY_PROBLEM;
	strcpy
	(popular_challenge_copy, sys->challenges[popular_challenge_index].name);
	*challenge_name = popular_challenge_copy;
	return OK;
}
