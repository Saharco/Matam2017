#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <assert.h>


#include "challenge.h"

/* Gets the ID, name and difficulty level of a challenge, and initializes
 * all the relevant fields to these values. Rest of the values are initialized to 0.
 * Will return OK if successful, otherwise will print an error message: null parameter,
 * or insufficient memory (can't allocate challenge's name). */
Result init_challenge(Challenge *challenge, int id, char *name, Level level) {
	if(!challenge || !name) return NULL_PARAMETER;
	//malloc: store the challenge's name
	char *name_copy = (char*)malloc(strlen(name)+1);
	if(!name_copy) return MEMORY_PROBLEM; //malloc failed
	strcpy(name_copy, name);
	challenge->id = id;
	challenge->level = level;
	challenge->name = name_copy;
	challenge->best_time = 0;
	challenge->num_visits = 0;
	return OK;
}
/* Initializes all of the challenge's fields to 0 (pointers are initialized to NULL).
 * Will return OK if successful, otherwise will print an error message: null parameter. */
Result reset_challenge(Challenge *challenge) {
	if(!challenge) return NULL_PARAMETER;
	if(challenge->name)	free(challenge->name); //Free the challenge's name
	challenge->name = NULL;
	challenge->id = 0;
	challenge->level = 0;
	challenge->best_time = 0;
	challenge->num_visits = 0;
	return OK;
}
/* Gets the name of a challenge and changes the challenge's name to the given parameter name.
 * Will return OK if successful, otherwise will return an error: null parameter,
 * or insufficient memory (can't allocate new name), in which case the name will not change. */
Result change_name(Challenge *challenge, char *name) {
	if(!challenge || !name) return NULL_PARAMETER;
	char *name_copy = (char*)malloc(strlen(name)+1); //malloc: hold the challenge's new name
	if(!name_copy) return MEMORY_PROBLEM; //malloc failed
	if(challenge->name) free(challenge->name); //Free the previous name
	strcpy(name_copy, name);
	challenge->name = name_copy;
	return OK;
}
/* Gets the new best time of a challenge, and changes the challenge's best time to the given parameter.
 * Returns OK if successful, otherwise will return an error: null parameter,
 * or illegal parameter (either the previous best time was better, or the new best time is a negative number) */
Result set_best_time_of_challenge(Challenge *challenge, int time) {
	if(!challenge) return NULL_PARAMETER;
	if(time<0 || (challenge->best_time > 0 && time > challenge->best_time))
		//Invalid time: opposes time continuity!
		return ILLEGAL_PARAMETER;
	challenge->best_time = time;
	return OK;
}
/* Gets a pointer to an integer that represents the challenge's best time.
 * References the pointer to the challenge's best time, stored in a separate copy.
 * Will return OK if successful, otherwise will return an error: null parameter. */
Result best_time_of_challenge(Challenge *challenge, int *time) {
	if(!challenge || !time) return NULL_PARAMETER;
	int best_time_copy = challenge->best_time;
	*time = best_time_copy;
	return OK;
}
/* Increases the challenge's number of visitors by 1.
 * Will return OK if successful, otherwise will return an error: null parameter. */
Result inc_num_visits(Challenge *challenge) {
	if(!challenge) return NULL_PARAMETER;
	challenge->num_visits++;
	return OK;
}
/* Gets a pointer to an integer that represents the challenge's number of visitors.
 * References the pointer to the challenge's number of visitors, stored in a separate copy.
 * Will return OK if successful, otherwise will return an error: null parameter. */
Result num_visits(Challenge *challenge, int *visits) {
	if(!challenge || !visits) return NULL_PARAMETER;
	int visits_copy = challenge->num_visits;
	*visits = visits_copy;
	return OK;
}
