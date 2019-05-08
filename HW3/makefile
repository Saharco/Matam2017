#The compiler being used for this program
CC = gcc
#The object files needed in order to create this program
OBJS = EscapeTechnion.o Company.o Room.o Escaper.o Reservation.o
#The object test files
TEST_OBJS = EscapeTechnion_test.o Company_test.o Room_test.o Escaper_test.o Reservation_test.o
#The name of the executable program being made
EXEC = mtm_escape
#The compile flags being used
COMP_FLAGS = -std=c99 -Wall -pedantic-errors -Werror -DNDEBUG
#The additional flags being used for linking the given static library
STATIC_LIB = -L. -lmtm
#The given .h file with definitions for this exercise
MTM3 = mtm_ex3.h
#The test utilities header file, being used in the test files. It's located in the "tests" directory
TESTS = tests/test_utilities.h

#The main program's rule:
$(EXEC) : mtm_escape.o $(OBJS)
	$(CC) mtm_escape.o $(OBJS) $(STATIC_LIB) -o $@

mtm_escape.o:		mtm_escape.c $(MTM3) EscapeTechnion.h set.h list.h \
					Company.h Room.h Escaper.h Reservation.h
	$(CC) -c $(COMP_FLAGS) $*.c
	
EscapeTechnion.o:	EscapeTechnion.c $(MTM3) EscapeTechnion.h set.h \
					Company.h Room.h Escaper.h Reservation.h
	$(CC) -c $(COMP_FLAGS)  $*.c
Company.o:			Company.c $(MTM3) Company.h set.h Room.h Escaper.h \
					Reservation.h
	$(CC) -c $(COMP_FLAGS)  $*.c
Escaper.o:			Escaper.c $(MTM3) Escaper.h set.h Room.h Reservation.h
	$(CC) -c $(COMP_FLAGS)  $*.c
Room.o:				Room.c $(MTM3) Room.h list.h Reservation.h
	$(CC) -c $(COMP_FLAGS)  $*.c
Reservation.o:		Reservation.c Reservation.h Room.h Escaper.h
	$(CC) -c $(COMP_FLAGS)  $*.c
	
#The tests programs:
	
EscapeTechnion_test: 	EscapeTechnion_test.o $(OBJS)
	$(CC) EscapeTechnion_test.o $(OBJS) $(STATIC_LIB) -o $@
Company_test: 		Company_test.o Company.o Room.o Escaper.o Reservation.o
	$(CC) Company_test.o Company.o Room.o Escaper.o Reservation.o $(STATIC_LIB) -o $@
Escaper_test: 		Escaper_test.o Room.o Reservation.o
	$(CC) Escaper_test.o Room.o Reservation.o $(STATIC_LIB) -o $@
Room_test: Room_test.o Room.o Reservation.o
	$(CC) Room_test.o Room.o Reservation.o $(STATIC_LIB) -o $@
Reservation_test: Reservation_test.o Reservation.o Room.o Escaper.o
	$(CC) Reservation_test.o Reservation.o Room.o Escaper.o $(STATIC_LIB) -o $@
EscapeTechnion_test.o:	tests/EscapeTechnion_test.c $(MTM3) $(TESTS) \
						set.h $(OBJS)
	$(CC) -c $(COMP_FLAGS) tests/$*.c
Company_test.o:		tests/Company_test.c $(MTM3) $(TESTS) set.h Company.h \
					Room.h Escaper.h Reservation.h
	$(CC) -c $(COMP_FLAGS) tests/$*.c
Escaper_test.o:		tests/Escaper_test.c $(MTM3) $(TESTS) Escaper.h set.h \
					Room.h Reservation.h
	$(CC) -c $(COMP_FLAGS) tests/$*.c
Room_test.o:		tests/Room_test.c $(MTM3) $(TESTS) Room.h list.h Reservation.h
	$(CC) -c $(COMP_FLAGS) tests/$*.c
Reservation_test.o:		tests/Reservation_test.c $(TESTS) Reservation.h Room.h Escaper.h
	$(CC) -c $(COMP_FLAGS) tests/$*.c
	
#And now, the generic list:

list_mtm1_test:		list_mtm1_test.o list_mtm1.o
	$(CC) $(COMP_FLAGS) $@.o list_mtm1.o -o $@
list_mtm1_test.o:	tests/list_mtm1_test.c $(TESTS) list_mtm/list_mtm1.h
	$(CC) -c $(COMP_FLAGS) $*.c
list_mtm1.o: 		list_mtm/list_mtm1.c list_mtm/list_mtm1.h
	$(CC) -c $(COMP_FLAGS) $*.c
	
#Finally, an option to clean object files (either with or without the executable)
	
clean:
	rm -f mtm_escape.o $(OBJS) $(TEST_OBJS) $(EXEC)
clean_objects:
	rm -f mtm_escape.o $(OBJS) $(TEST_OBJS)