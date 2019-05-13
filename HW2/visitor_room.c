#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "visitor_room.h"
#define NO_VALID_CHALLENGE -1

/* Gets a pointer to a challenge activity in a room, and a pointer to a challenge.
 * Initializes all the fields in the challenge activity to either 0 or null,
 * and the challenge field with the challenge parameter.
 * Returns OK if successful, otherwise will return an error: null parameter */
Result init_challenge_activity(ChallengeActivity *activity,
Challenge *challenge){
	if(!activity || !challenge) return NULL_PARAMETER;
	activity->challenge = challenge;
	activity->visitor = NULL;
	activity->start_time = 0;
	return OK;
}

/* Gets a pointer to a challenge activity in a room.
 * Initializes all the fields in the challenge activity to either 0 or null.
 * Returns OK if successful, otherwise will return an error: null parameter */
Result reset_challenge_activity(ChallengeActivity *activity){
	if(!activity) return NULL_PARAMETER;
	activity->challenge = NULL;
	activity->visitor = NULL;
	activity->start_time = 0;
	return OK;
}

/* Gets a pointer to a visitor, the visitor's name and the visitor's ID.
 * Initializes all of the visitor's fields to null,  except for the visitor's name
 * and his ID which are assigned with the parameter input.
 * Returns OK if successful, otherwise will return an error: null parameter,
 * or insufficient memory (can't allocate visitor's name) */
Result init_visitor(Visitor *visitor, char *name, int id){
	if(!visitor || !name) return NULL_PARAMETER;
	//malloc: store the challenge's name
	char* name_copy = (char*)malloc(strlen(name)+1);
	if(!name_copy) return MEMORY_PROBLEM; //malloc failed
	strcpy(name_copy,name);
	visitor->visitor_name = name_copy;
	visitor->visitor_id = id;
	visitor->room_name = NULL;
	visitor->current_challenge = NULL;
	return OK;
}

/* Gets a pointer to a visitor, and resets all of the visitor's fields to either 0 or null.
 * As for the visitor's name - it is freed first.
 * Returns OK if successful, otherwise will return an error: null parameter. */
Result reset_visitor(Visitor *visitor){
	if(!visitor) return NULL_PARAMETER;
	if(!visitor->visitor_name) free(visitor->visitor_name); //Free the visitor's name
	visitor->visitor_name = NULL;
	visitor->visitor_id = 0;
	visitor->room_name = NULL;
	visitor->current_challenge = NULL;
	return OK;
}

/* Gets a pointer to a room, its name and the amount of challenges in the room.
 * Initializes all of the room's fields: the room's name is the parameter name,
 * the amount of challenges is the size of the challenge activities array.
 * Returns OK if successful, otherwise will return an error: null parameter,
 * illegal parameter (amount of challenges in the room is smaller than 1), or memory problem
 * (can't allocate either the room's name, or the array of challenge activities). */
Result init_room(ChallengeRoom *room, char *name, int num_challenges){
	if(!room || !name) return NULL_PARAMETER;
	if(num_challenges<1) return ILLEGAL_PARAMETER; //Room can't have less than one challenge in it
	//malloc: store the room's name
	char* name_copy = malloc(strlen(name)+1);
	if(!name_copy) return MEMORY_PROBLEM; //malloc failed
	//malloc: store the room's challenges activities array
	room->challenges = malloc(num_challenges * sizeof(ChallengeActivity));
	if(!(room->challenges)) {
		//malloc failed: free the previously allocated name before returning error
		free(name_copy);
		return MEMORY_PROBLEM;
	}
	strcpy(name_copy,name);
	room->name = name_copy;
	room->num_of_challenges = num_challenges;
	for(int i=0; i<room->num_of_challenges; i++)
		reset_challenge_activity(&(room->challenges[i])); //Reset all of the challenge activities in the array
	return OK;
}

/* Gets a pointer to a room and resets all of its fields to either 0 or null.
 * Allocated memory is freed beforehand.
 * Returns OK if successful, otherwise will return an error: null parameter */
Result reset_room(ChallengeRoom *room){
	if(!room) return NULL_PARAMETER;
	if(!room->name) free(room->name);
	room->name = NULL;
	room->num_of_challenges = 0;
	room->challenges = NULL;
	return OK;
}

/* Gets a pointer to a room, a level (easy, medium, hard or all_levels => 1,2,3 or 4) and the address of an integer.
 * Counts down all of the challenges in the room that aren't currently occupied and meet the level requirement.
 * If the level requirement is all_levels: all non-occupied challenges are acceptable.
 * Changes the integer whose address was received to be the amount of challenges that meet the requirements.
 * Returns OK if successful, otherwise will return an error: null parameter. */
Result num_of_free_places_for_level(ChallengeRoom *room, Level level,
int *places){
	if(!room || !places) return NULL_PARAMETER;
	int count = 0;
	int all_levels_flag = (level == All_Levels) ? (1) : (0);
	for(int i=0; i<(room->num_of_challenges); i++)
		if(((all_levels_flag)||(room->challenges[i].challenge->level == level))
		&& (!(room->challenges[i].visitor)))
			count++;
	*places = count;
	return OK;
}

/* Gets a pointer to a room and the name of the room and changes the room's name to the given parameter name.
 * Will return OK if successful, otherwise will return an error: null parameter,
 * or insufficient memory (can't allocate new name), in which case the name will not change. */
Result change_room_name(ChallengeRoom *room, char *new_name){
	if(!room || !new_name) return NULL_PARAMETER;
	//malloc: store the room's new name
	char *new_name_copy = malloc(strlen(new_name)+1);
	if(!new_name_copy) return MEMORY_PROBLEM; //malloc failed
	strcpy(new_name_copy, new_name);
	free(room->name);
	room->name = new_name_copy;
	return OK;
}

/* Gets a pointer to a visitor and the address of a string. Finds the name of the room
 * where the required visitor is at, and stores its name (allocated as a new string) to be
 * the received parameter string.
 * Will return OK if successful, otherwise will return an error:
 * null parameter, not in room (the visitor is not staying in any room) or memory problem
 * (can't allocate the room's name) */
Result room_of_visitor(Visitor *visitor, char **room_name){
	if(!visitor || !room_name) return NULL_PARAMETER;
	if(!visitor->room_name) return NOT_IN_ROOM;
	char *room_name_copy = malloc(strlen(*(visitor->room_name))+1);
	if(!room_name_copy) return MEMORY_PROBLEM;
	strcpy(room_name_copy, *(visitor->room_name));
	*room_name = room_name_copy;
	return OK;
}

/* Gets a pointer to a room, a pointer to a visitor, a level (easy, medium, hard or all_levels => 1,2,3 or 4) and the starting time.
 * Moves the visitor into a room that meets the level requirements (not empty, and of the desirable level).
 * If the desirable level is all_levels: all non-occupied challenges are acceptable.
 * In the situation where there are multiple challenges meeting the visitor's requirements:
 * the chosen challenge will be the smallest one in lexicographic order.
 * At the end of this process, the following fields are changed: the visitor's room is now the parameter room,
 * the visitor's challenge activity is now the challenge activity in the room with the desirable challenge,
 * the global amount of visits in the challenge is increased by one, the challenge activity is now
 * occupied with the visitor in it, and the starting time of the visitor's
 * challenge trial is updated in the challenge activity.
 * Will return OK if successful, otherwise will return an error: null parameter,
 * already in room (if the visitor is currently attending a challenge), or
 * no available challenges if no acceptable challenge was found. */
Result visitor_enter_room(ChallengeRoom *room, Visitor *visitor, Level level,
int start_time){
	if(!room || !visitor) return NULL_PARAMETER;
	if(visitor->room_name) return ALREADY_IN_ROOM;
	int valid_challenge_index = NO_VALID_CHALLENGE;
	int all_levels_flag = (level == All_Levels) ? 1 : 0;
	for(int i=0; i<(room->num_of_challenges); i++)
	{
		if((!(room->challenges[i].visitor)) && ((all_levels_flag) ||
		(room->challenges[i].challenge->level == level))) {
			if((valid_challenge_index == NO_VALID_CHALLENGE) ||
			(strcmp(room->challenges[valid_challenge_index].challenge->name,
			room->challenges[i].challenge->name) > 0))
				valid_challenge_index = i;
		}
	}
	if(valid_challenge_index == NO_VALID_CHALLENGE)
		return NO_AVAILABLE_CHALLENGES;
	visitor->room_name = &room->name;
	inc_num_visits(room->challenges[valid_challenge_index].challenge);
	room->challenges[valid_challenge_index].visitor = visitor;
	visitor->current_challenge = &room->challenges[valid_challenge_index];
	visitor->current_challenge->start_time = start_time;
	return OK;
}

/* Gets a pointer to a visitor and his finish time, and removes the visitor from the record.
 * The following fields are updated: if the visitor has set a new record in the challenge,
 * then the global best time of the challenge is updated. The challenge activity's visitor that was occupied
 * with the visitor is now null and its start time is 0. Finally, the visitor is reset.
 * Returns OK if successful, otherwise will return an error: null parameter, or not in room
 * if the visitor is not staying in any room */
Result visitor_quit_room(Visitor *visitor, int quit_time){
	if(!visitor)return NULL_PARAMETER;
	if(!visitor->room_name) return NOT_IN_ROOM;
	int visitor_time = quit_time - visitor->current_challenge->start_time;
	if((visitor->current_challenge->challenge->best_time == 0) ||
	(visitor_time < visitor->current_challenge->challenge->best_time))
		//Update that best global time of the challenge
		set_best_time_of_challenge
		(visitor->current_challenge->challenge, visitor_time);
	visitor->current_challenge->visitor = NULL;
	visitor->current_challenge->start_time = 0;
	reset_visitor(visitor);
	return OK;
}
