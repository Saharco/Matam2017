char *name; //Name of the system
int challenges_amount; //The fixed number of challenges in the system
int rooms_amount; //The fixed number of rooms in the system
Challenge *challenges; //Array of the challenges supported by the system
ChallengeRoom *rooms; //Array of the available rooms in the system
VisitorsList *visitors; //Linked list of the visitors of the system
int time_of_last_action; //The time mark of the last (legal) action in the system
