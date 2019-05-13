//Linked list struct whose values are pointers to visitors
typedef struct SVisitorNode {
	Visitor *visitor; //Value of a node: pointer to a visitor
	struct SVisitorNode* next; //Pointer to the next node
} VisitorsList;
