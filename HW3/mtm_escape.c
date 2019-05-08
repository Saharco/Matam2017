#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "EscapeTechnion.h"

#define INPUT_FILE_FLAG "-i"
#define OUTPUT_FILE_FLAG "-o"
#define OPTION1_ARGS 0
#define OPTION2_ARGS 2
#define OPTION3_ARGS 4
#define MAX_COMMAND_WORDS 8
#define COMMANDS_AMOUNT 4
#define SUB_COMMANDS_AMOUNT 6
#define COMMENT_MARK '#'

/*
 * A macro for closing all the files before finishing the program.
 */
#define CLOSE_FILES(in_file, out_file, err_file) \
	fclose(in_file); \
	fclose(out_file); \
	fclose(err_file);

/*
 * A macro for comparing commands and sub-commands, and assigning the correct
 * code value to a parameter code (and returning true, because we found a match)
 * The comparison is between "commands & str1", and "sub & str2".
 */
#define TRY_CMD(str1, str2, command, sub, code, result) \
	if ((strcmp(str1, command) == 0) && (strcmp(str2, sub) == 0)) { \
		*code = result; \
		return true; }


typedef enum {
	/*
	 * List of all the possible command & sub-command combinations.
	 * For convince, we create a simple enum in order to save a code for the
	 * current command once we've deciphered it!
	 * Each combination is granted a code:
	 */
	COMMAND_COMPANY_ADD,
	COMMAND_COMPANY_REMOVE,
	COMMAND_ROOM_ADD,
	COMMAND_ROOM_REMOVE,
	COMMAND_ESCAPER_ADD,
	COMMAND_ESCAPER_REMOVE,
	COMMAND_ESCAPER_ORDER,
	COMMAND_ESCAPER_RECOMMEND,
	COMMAND_REPORT_DAY,
	COMMAND_REPORT_BEST,
	COMMAND_COMMENT,
	COMMAND_INVALID
} CommandCode;



static bool print_error(EscapeTechnionResult result, FILE* error_file);
static bool checkArgumentsNum(int arguments_num, FILE* err);
static bool validCommand(char* cmd, CommandCode* code);
static bool validSubCommand(char* cmd, char* sub, CommandCode* code);
static bool validParametersAmount(int parameters, CommandCode* code);
static bool handleCommandLine(char* buffer, char** words,
CommandCode* code, int* parameters, FILE* err_file);
static bool callCompanyAdd(EscapeTechnion system, char** words,
int parameters, FILE* err_file);
static bool callCompanyRemove(EscapeTechnion system, char** words,
int parameters, FILE* err_file);
static bool callRoomAdd(EscapeTechnion system, char** words,
int parameters, FILE* err_file);
static bool callRoomRemove(EscapeTechnion system, char** words,
int parameters, FILE* err_file);
static bool callEscaperAdd(EscapeTechnion system, char** words,
int parameters, FILE* err_file);
static bool callEscaperRemove(EscapeTechnion system, char** words,
int parameters, FILE* err_file);
static bool callEscaperOrder(EscapeTechnion system, char** words,
int parameters, FILE* err_file);
static bool callEscaperRecommend(EscapeTechnion system, char** words,
int parameters, FILE* err_file);
static bool callReportDay(EscapeTechnion system, FILE* err_file);
static bool callReportBest(EscapeTechnion system, FILE* err_file);
static bool translateCommand(EscapeTechnion system, char** words,
CommandCode code, int parameters, FILE* err_file);
static void getCommands(EscapeTechnion system, FILE* input_file,
FILE* output_file, FILE* err_file);

/*
 * Gets the EscapeTechnion system's error and converts it to MTM ERROR CODE,
 * and then prints it to the parameter error stream.
 * NOTE: We also return false iff the error was a memory problem.
 */
static bool print_error(EscapeTechnionResult result, FILE* error_file) {
	switch(result) {
		case ESCAPE_TECHNION_OUT_OF_MEMORY:
			mtmPrintErrorMessage(error_file, MTM_OUT_OF_MEMORY);
			return false;
		case ESCAPE_TECHNION_NULL_PARAMETER:
			mtmPrintErrorMessage(error_file, MTM_NULL_PARAMETER);
			break;
		case ESCAPE_TECHNION_INVALID_PARAMETER:
			mtmPrintErrorMessage(error_file, MTM_INVALID_PARAMETER);
			break;
		case ESCAPE_TECHNION_EMAIL_ALREADY_EXISTS:
			mtmPrintErrorMessage(error_file, MTM_EMAIL_ALREADY_EXISTS);
			break;
		case ESCAPE_TECHNION_COMPANY_EMAIL_DOES_NOT_EXIST:
			mtmPrintErrorMessage(error_file, MTM_COMPANY_EMAIL_DOES_NOT_EXIST);
			break;
		case ESCAPE_TECHNION_CLIENT_EMAIL_DOES_NOT_EXIST:
			mtmPrintErrorMessage(error_file, MTM_CLIENT_EMAIL_DOES_NOT_EXIST);
			break;
		case ESCAPE_TECHNION_ID_ALREADY_EXIST:
			mtmPrintErrorMessage(error_file, MTM_ID_ALREADY_EXIST);
			break;
		case ESCAPE_TECHNION_ID_DOES_NOT_EXIST:
			mtmPrintErrorMessage(error_file, MTM_ID_DOES_NOT_EXIST);
			break;
		case ESCAPE_TECHNION_CLIENT_IN_ROOM:
			mtmPrintErrorMessage(error_file, MTM_CLIENT_IN_ROOM);
			break;
		case ESCAPE_TECHNION_ROOM_NOT_AVAILABLE:
			mtmPrintErrorMessage(error_file, MTM_ROOM_NOT_AVAILABLE);
			break;
		case ESCAPE_TECHNION_RESERVATION_EXISTS:
			mtmPrintErrorMessage(error_file, MTM_RESERVATION_EXISTS);
			break;
		case ESCAPE_TECHNION_NO_ROOMS_AVAILABLE:
			mtmPrintErrorMessage(error_file, MTM_NO_ROOMS_AVAILABLE);
			break;
		default:
			break; // success!
	}
	return true;
}

/*
 * Checks that the number of arguments is 0, 2 or 4 (excluding the terminal's
 * executable name declaration).
 */
static bool checkArgumentsNum(int arguments_num, FILE* err_file) {
	if(arguments_num!=OPTION1_ARGS+1 && arguments_num!=OPTION2_ARGS+1
	&& arguments_num!=OPTION3_ARGS+1) {
		mtmPrintErrorMessage(err_file, MTM_INVALID_COMMAND_LINE_PARAMETERS);
		fclose(err_file);
		return false;
	}
	return true;
}

/*
 * Checks if a command is one of the valid commands, and returns true if it is.
 * Otherwise, returns false and updates the error code.
 */
static bool validCommand(char* cmd, CommandCode* code) {
	char* commands[COMMANDS_AMOUNT] = {"company", "room", "escaper", "report"};
	if(cmd[0] == COMMENT_MARK) {
		*code = COMMAND_COMMENT;
		return false;
	}
	for(int i=0; i<COMMANDS_AMOUNT; i++)
		if(strcmp(cmd, commands[i]) == 0)
			return true;
	*code = COMMAND_INVALID;
	return false;
}

/*
 * Checks if a sub-command is one of the valid sub-commands, and returns true
 * if it is. Otherwise, returns false and updates the error code.
 */
static bool validSubCommand(char* cmd, char* sub, CommandCode* code) {
	TRY_CMD("company", "add", cmd, sub, code, COMMAND_COMPANY_ADD)
	TRY_CMD("company", "remove", cmd, sub, code, COMMAND_COMPANY_REMOVE)
	TRY_CMD("room", "add", cmd, sub, code, COMMAND_ROOM_ADD)
	TRY_CMD("room", "remove", cmd, sub, code, COMMAND_ROOM_REMOVE)
	TRY_CMD("escaper", "add", cmd, sub, code, COMMAND_ESCAPER_ADD)
	TRY_CMD("escaper", "remove", cmd, sub, code, COMMAND_ESCAPER_REMOVE)
	TRY_CMD("escaper", "order", cmd, sub, code, COMMAND_ESCAPER_ORDER)
	TRY_CMD("escaper", "recommend", cmd, sub, code, COMMAND_ESCAPER_RECOMMEND)
	TRY_CMD("report", "day", cmd, sub, code, COMMAND_REPORT_DAY)
	TRY_CMD("report", "best", cmd, sub, code, COMMAND_REPORT_BEST)
	*code = COMMAND_INVALID;
	return false;
}

/*
 * Checks if the amount of parameters given with the command is valid. Returns
 * true if it is, and false if it isn't, and updates the error code.
 */
static bool validParametersAmount(int parameters, CommandCode* code) {
	switch(*code) {
		case COMMAND_COMPANY_ADD:
			return (parameters == 2);
		case COMMAND_COMPANY_REMOVE:
			return (parameters == 1);
		case COMMAND_ROOM_ADD:
			return (parameters == 6);
		case COMMAND_ROOM_REMOVE:
			return (parameters == 2);
		case COMMAND_ESCAPER_ADD:
			return (parameters == 3);
		case COMMAND_ESCAPER_REMOVE:
			return (parameters == 1);
		case COMMAND_ESCAPER_ORDER:
			return (parameters == 5);
		case COMMAND_ESCAPER_RECOMMEND:
			return (parameters == 2);
		case COMMAND_REPORT_DAY:
			return (parameters == 0);
		case COMMAND_REPORT_BEST:
			return (parameters == 0);
		default:
			*code = COMMAND_INVALID;
			return false;
	}
}

/*
 * A static function for completely deciphering a command line. Updates the
 * command code and the parameters for this command (in the words array).
 * Returns false if the command is not one of the valid commands,
 * and true otherwise.
 */
static bool handleCommandLine(char* buffer, char** words, CommandCode* code,
int* parameters, FILE* err_file) {
	assert(buffer && words && code && err_file);
	int index = 0;
	char* token = strtok(buffer, " \t\n");
	if(!token) {
		*code = COMMAND_COMMENT;
		return false;
	}
	while(words[index] && token) {
		strcpy(words[index++], token);
		token = strtok(NULL, " \t\n");
	}
	if((words[0]) && (!validCommand(words[0], code))) {
		if(*code == COMMAND_INVALID)
			mtmPrintErrorMessage
			(err_file, MTM_INVALID_COMMAND_LINE_PARAMETERS);
		return false;
	}
	if((words[1]) && (!validSubCommand(words[0], words[1], code))) {
		if(*code == COMMAND_INVALID)
			mtmPrintErrorMessage
			(err_file, MTM_INVALID_COMMAND_LINE_PARAMETERS);
		return false;
	}
	if(!validParametersAmount(index-2, code)) {
		mtmPrintErrorMessage(err_file, MTM_INVALID_COMMAND_LINE_PARAMETERS);
		return false;
	}
	*parameters = index-2;
	return true;
}

/*
 * Calls the system's company add function, prints the error (if there is one)
 * to the error stream, and returns false iff a memory problem has occurred
 * (true otherwise)
 */
static bool callCompanyAdd(EscapeTechnion system, char** words,
int parameters, FILE* err_file) {
	int params_index = 2;
	EscapeTechnionResult result = escapeTechnionAddCompany(system,
	words[params_index], (TechnionFaculty)atoi(words[params_index+1]));
	return print_error(result, err_file);
}

/*
 * Calls the system's company remove function, prints the error
 * (if there is one) to the error stream, and returns false iff a memory
 * problem has occurred (true otherwise)
 */
static bool callCompanyRemove(EscapeTechnion system, char** words,
int parameters, FILE* err_file) {
	int params_index = 2;
	EscapeTechnionResult result = escapeTechnionRemoveCompany(system,
	words[params_index]);
	return print_error(result, err_file);
}

/*
 * Calls the system's room add function, prints the error
 * (if there is one) to the error stream, and returns false iff a memory
 * problem has occurred (true otherwise)
 */
static bool callRoomAdd(EscapeTechnion system, char** words,
int parameters, FILE* err_file) {
	int params_index = 2;
	EscapeTechnionResult result = escapeTechnionAddRoom(system,
	words[params_index], atoi(words[params_index+1]),
	atoi(words[params_index+2]), atoi(words[params_index+3]),
	words[params_index+4], atoi(words[params_index+5]));
	return print_error(result, err_file);
}

/*
 * Calls the system's room remove function, prints the error
 * (if there is one) to the error stream, and returns false iff a memory
 * problem has occurred (true otherwise)
 */
static bool callRoomRemove(EscapeTechnion system, char** words,
int parameters, FILE* err_file) {
	int params_index = 2;
	EscapeTechnionResult result = escapeTechnionRemoveRoom(system,
	(TechnionFaculty)atoi(words[params_index]), atoi(words[params_index+1]));
	return print_error(result, err_file);
}

/*
 * Calls the system's escaper add function, prints the error
 * (if there is one) to the error stream, and returns false iff a memory
 * problem has occurred (true otherwise)
 */
static bool callEscaperAdd(EscapeTechnion system, char** words,
int parameters, FILE* err_file) {
	int params_index = 2;
	EscapeTechnionResult result = escapeTechnionAddEscaper(system,
	words[params_index], (TechnionFaculty)atoi(words[params_index+1]),
	atoi(words[params_index+2]));
	return print_error(result, err_file);
}

/*
 * Calls the system's escaper remove function, prints the error
 * (if there is one) to the error stream, and returns false iff a memory
 * problem has occurred (true otherwise)
 */
static bool callEscaperRemove(EscapeTechnion system, char** words,
int parameters, FILE* err_file) {
	int params_index = 2;
	EscapeTechnionResult result = escapeTechnionRemoveEscaper(system,
	words[params_index]);
	return print_error(result, err_file);
}

/*
 * Calls the system's escaper order function, prints the error
 * (if there is one) to the error stream, and returns false iff a memory
 * problem has occurred (true otherwise)
 */
static bool callEscaperOrder(EscapeTechnion system, char** words,
int parameters, FILE* err_file) {
	int params_index = 2;
	EscapeTechnionResult result = escapeTechnionEscaperOrder(system,
	words[params_index], (TechnionFaculty)atoi(words[params_index+1]),
	atoi(words[params_index+2]), words[params_index+3],
	atoi(words[params_index+4]));
	return print_error(result, err_file);
}

/*
 * Calls the system's escaper recommend function, prints the error
 * (if there is one) to the error stream, and returns false iff a memory
 * problem has occurred (true otherwise)
 */
static bool callEscaperRecommend(EscapeTechnion system, char** words,
int parameters, FILE* err_file) {
	int params_index = 2;
	EscapeTechnionResult result = escapeTechnionEscaperRecommend(system,
	words[params_index], atoi(words[params_index+1]));
	return print_error(result, err_file);
}

/*
 * Calls the system's report day function, prints the error
 * (if there is one) to the error stream, and returns false iff a memory
 * problem has occurred (true otherwise)
 */
static bool callReportDay(EscapeTechnion system, FILE* err_file) {
	EscapeTechnionResult result = escapeTechnionReportDay(system);
	return print_error(result, err_file);
}

/*
 * Calls the system's report best function, prints the error
 * (if there is one) to the error stream, and returns false iff a memory
 * problem has occurred (true otherwise)
 */
static bool callReportBest(EscapeTechnion system, FILE* err_file) {
	EscapeTechnionResult result = escapeTechnionReportBest(system);
	return print_error(result, err_file);
}

/*
 * Calls one of the system's functions, according to the command given.
 * The correct function is chosen according to the code assigned at the
 * deciphering stage that came before it. All that's left is to select
 * the function that matches our code! This function returns false
 * iff a memory problem has occured.
 */
static bool translateCommand(EscapeTechnion system, char** words,
CommandCode code, int parameters, FILE* err_file) {
	switch((int)code) {
		case COMMAND_COMPANY_ADD:
			return callCompanyAdd(system, words, parameters, err_file);
		case COMMAND_COMPANY_REMOVE:
			return callCompanyRemove(system, words, parameters, err_file);
		case COMMAND_ROOM_ADD:
			return callRoomAdd(system, words, parameters, err_file);
		case COMMAND_ROOM_REMOVE:
			return callRoomRemove(system, words, parameters, err_file);
		case COMMAND_ESCAPER_ADD:
			return callEscaperAdd(system, words, parameters, err_file);
		case COMMAND_ESCAPER_REMOVE:
			return callEscaperRemove(system, words, parameters, err_file);
		case COMMAND_ESCAPER_ORDER:
			return callEscaperOrder(system, words, parameters, err_file);
		case COMMAND_ESCAPER_RECOMMEND:
			return callEscaperRecommend(system, words, parameters, err_file);
		case COMMAND_REPORT_DAY:
			return callReportDay(system, err_file);
		case COMMAND_REPORT_BEST:
			return callReportBest(system, err_file);
		default:
			return false;
	}
	return false;
}

/*
 * This function reads all of the input from either stdin or the selected
 * input stream, until it reaches EOF. For each line it deciphers the correct
 * command (if it was correct at all), executes the system's correct function.
 * In case of errors - it will print the correct error (either from the system
 * or the command line). If a memory / command line problem is returned, the
 * function will stop reading its input.
 */
static void getCommands(EscapeTechnion system, FILE* input_file,
FILE* output_file, FILE* err_file) {
	escapeTechnionModifyOutput(system, output_file);
	char buffer[MAX_LEN+1] = {0}; //Terminate the string
	char* words[MAX_COMMAND_WORDS];
	for(int i=0; i<MAX_COMMAND_WORDS; i++) {
		//We will store the function parameters inside this array.
		words[i] = malloc((MAX_LEN+1));
		if(!(words[i])) {
			escapeTechnionDestroy(system);
			mtmPrintErrorMessage(err_file, MTM_NULL_PARAMETER);
			return;
		}
		words[i][MAX_LEN] = 0; //Terminate the string
	}
	int parameters = 0;
	while(fgets(buffer, MAX_LEN+1, input_file) != NULL) {
		CommandCode code = COMMAND_COMMENT;
		if(!handleCommandLine(buffer, words, &code, &parameters,
		err_file)) {
			if(code == COMMAND_INVALID)
				break; //command is invalid
			continue; //command is a comment
		}
		if(!translateCommand(system, words, code, parameters, err_file))
			break;
		for(int i = 0; i<MAX_COMMAND_WORDS; i++)
			words[i][MAX_LEN] = 0;
	}
	//End of file / fatal error occurred. Finish process:
	for(int i=0; i<MAX_COMMAND_WORDS; i++)
		free(words[i]);
	escapeTechnionDestroy(system); //destroy the system
	CLOSE_FILES(input_file, output_file, err_file); //close all files
}

int main(int argc, char** argv) {
	FILE* input_file = stdin;
	FILE* output_file = stdout;
	FILE* err_file = stderr;
	if(!checkArgumentsNum(argc, err_file))
		return 0;
	for(int i=1; i<argc-1; i+=2) {
		if(strcmp(argv[i], INPUT_FILE_FLAG) == 0) {
			if(input_file != stdin) {
				fclose(input_file);
				mtmPrintErrorMessage(err_file,
				MTM_INVALID_COMMAND_LINE_PARAMETERS);
				return 0;
			}
			input_file = fopen(argv[i+1], "r");
			if(!input_file) {
				mtmPrintErrorMessage(err_file, MTM_CANNOT_OPEN_FILE);
				return 0;
			}
		}
		else if(strcmp(argv[i], OUTPUT_FILE_FLAG) == 0) {
			if(output_file != stdout) {
				fclose(output_file);
				mtmPrintErrorMessage(err_file,
				MTM_INVALID_COMMAND_LINE_PARAMETERS);
				return 0;
			}
			output_file = fopen(argv[i+1], "w");
			if(!output_file) {
				mtmPrintErrorMessage(err_file, MTM_CANNOT_OPEN_FILE);
				return false;
			}
		}
		else {
			fclose(input_file);
			fclose(output_file);
			mtmPrintErrorMessage(err_file,
			MTM_INVALID_COMMAND_LINE_PARAMETERS);
			return 0;
		}
	}
	EscapeTechnion system = escapeTechnionCreate();
	if(!system) {
		CLOSE_FILES(input_file, output_file, err_file);
		return 0;
	}
	//All went well with the initialization. Start reading input:
	getCommands(system, input_file, output_file, err_file);
	return 0;
}
