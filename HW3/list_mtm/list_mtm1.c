#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include "list_mtm1.h"

/*
 * The node struct holds a value and the addresses of two other nodes in order
 * to establish a list unit.
 */
typedef struct node_t {
	ListElement data;
	struct node_t *next;
	struct node_t *previous;
} *Node;

struct list_t {
	Node head; //The first element in the list: its data is NOT part of the list
	int size; //The list's size
	Node current_element; //The list's iterator
	CopyListElement copy_function; //The list elements' copy function
	FreeListElement free_function; //The list elements' free function
};

#define NULL_ITERATOR -1 //Error code for when the list's iterator points to NULL

/*
 * A macro for checking the validity of pointers.
 * 	$ptr - a pointer of any type
 * 	$error - the return value in case the pointer points to NULL
 * If the pointer points to null - returns the error. Otherwise does nothing.
 */
#define CHECK_NULL(ptr, error) \
	if(!(ptr)) { return error; }


static Node nodeCreate(ListElement data, Node next, Node previous);
static void listSwap(ListElement *element_1, ListElement *element_2);
static bool bubble(List list, int n, CompareListElements compareElement);
static void set_current_as_last(List list);
static int getListIteratorIndex(List list);
static Node indexedInterator(List list, int index);

/*
 * Allocates a new node.
 * Creates a new node and initializes its data to be the parameter data,
 * and chains it to two other nodes: one before it, and one after it.
 * 	$data: The node's stored data.
 * 	$next: The node that comes after this node.
 * 	$previous: The node that comes before this node.
 * #return:
 * 	NULL - if one of the parameters is invalid or an allocation fails,
 * 	And a new node otherwise.
 */
static Node nodeCreate(ListElement data, Node next, Node previous) {
	CHECK_NULL(data, NULL)
	Node new_node = malloc(sizeof(*new_node));
	CHECK_NULL(new_node, NULL)
	new_node->data = data;
	new_node->next = next;
	new_node->previous = previous;
	return new_node;
}

/*
 * Basic swap function: gets the addresses of two elements and swaps the values.
 * 	$element_1: The address of the first element.
 * 	$element_2: The address of the second element.
 */
static void listSwap(ListElement *element_1, ListElement *element_2) {
	assert(element_1 && element_2);
	ListElement temp = *element_2;
	*element_2 = *element_1;
	*element_1 = temp;
}

/*
 * Basic "bubble" mechanism in bubble sort, for a list:
 * Iterates over two neighboring elements and swaps them if the first one is
 * bigger than the next.
 * 	$n - The amount of elements to iterate over.
 * 	$compareElement - The compare function for two elements in the list.
 * #return:
 * 	false - If not a single swap has occurred over the course of all the
 * 	elements, from the first one to the n-th one.
 * 	true - Otherwise.
 */
static bool bubble(List list, int n, CompareListElements compareElement) {
	assert(list && compareElement);
	bool swapped = false;
	listGetFirst(list);
	for(int i=1; i<n; i++) {
		if(compareElement(list->current_element->data,
		list->current_element->next->data) > 0) {
			listSwap(&(list->current_element->data),
			&(list->current_element->next->data));
			swapped = true;
		}
		list->current_element = list->current_element->next;
	}
	return swapped;
}

/*
 * Moves the list's iterator to the very last element in the list.
 */
static void set_current_as_last(List list) {
	assert(list && (list->size));
	list->current_element = list->head->next;
	while(list->current_element->next)
		list->current_element = list->current_element->next;
	//Now the iterator is pointing at the last node in the list
}

/*
 * Counts at which index in the list (if it were in fact indexed) the iterator
 * is currently pointing at.
 * #return:
 * 	NULL_ITERATOR(=-1) - If the iterator is invalid (NULL),
 * 	and the amount of elements in the list, according to their order, that come
 * 	before the iterator otherwise (its index).
 */
static int getListIteratorIndex(List list) {
	assert(list);
	int index = NULL_ITERATOR;
	CHECK_NULL(list->current_element, index);
	Node current = list->head;
	while((current->next) && (current != list->current_element)) {
		index++;
		current = current->next;
	}
	return index;
}

/*
 * Finds the node in the list that corresponds to a given index, and moves the
 * list's iterator to that node.
 * 	$index: The desired index, which is the amount of nodes that come before
 * 	the wanted node.
 * #return:
 * 	NULL - If the index is invalid,
 * 	and the node that the list's iterator points to otherwise.
 */
static Node indexedInterator(List list, int index) {
	assert(list);
	if(index == NULL_ITERATOR) return NULL;
	listGetFirst(list);
	for(int i = 0; i<index; i++)
		listGetNext(list);
	return list->current_element;
}

List listCreate(CopyListElement copyElement, FreeListElement freeElement) {
	CHECK_NULL(copyElement, NULL) //Checks if the pointers are NULL
	CHECK_NULL(freeElement, NULL) //Checks if the pointers are NULL
	List list = malloc(sizeof(*list)); //Allocates a new list
	CHECK_NULL(list, NULL) //Checks if the allocation has failed
	list->head = malloc(sizeof(*(list->head))); //Allocates a new node
	if(!(list->head)) {
		//Allocation failed: free the list, and return NULL.
		free(list);
		return NULL;
	}
	//Successfully allocated the list and its head node. Initialize:
	list->head->data = NULL; //NOTE: This is not a valid element in the list
	list->head->next = NULL; //The last node in the list points to NULL
	list->current_element = NULL; //The list's iterator points to NULL
	list->size = 0; //An empty list is of size 0
	list->copy_function = copyElement;
	list->free_function = freeElement;
	return list;
}

List listCopy(List list) {
	CHECK_NULL(list, NULL)
	List list_copy = listCreate(list->copy_function, list->free_function);
	CHECK_NULL(list_copy, NULL)
	ListResult result = LIST_SUCCESS; //Store the result of the last function:
	LIST_FOREACH(ListElement, element , list_copy) {
		/*
		 * Iterating over the list and copies its elements to the list's copy.
		 * If an insertion has failed - we exit this loop.
		 */
		ListResult result = listInsertLast(list_copy, element);
		if(result!=LIST_SUCCESS) break;
		if(list->current_element->data == element)
			/*
			 * We should get here exactly once, because the data of each node
			 * is of type void*, which is an address. That is to say, since
			 * we only have separate copies in our list, there will only be one
			 * element in the list with the address "element".
			 * */
			set_current_as_last(list_copy);
			//list_copy's iterator now points to the correct element
	}
	if(result!=LIST_SUCCESS) {
		//Insertion failed due to memory problem: Destroy the list, return NULL
		listDestroy(list);
		return NULL;
	}
	return list_copy; //List successfully copied!
}

int listGetSize(List list) {
	CHECK_NULL(list, -1) //We return an invalid size if the list is NULL
	//How convenient! We already have a field that stores the list's size!
	return (list->size);
}

ListElement listGetFirst(List list) {
	CHECK_NULL(list, NULL)
	CHECK_NULL(list->size, NULL)
	/*
	 * The list exists and is not empty: remember, the head node in the list
	 * has an illegal value. We return the value of the node that comes after it
	 */
	list->current_element = list->head->next;
	return list->current_element->data;
}

ListElement listGetNext(List list) {
	CHECK_NULL(list, NULL)
	CHECK_NULL(list->current_element, NULL)
	/*
	 * The list exists and the iterator points to a valid member:
	 * We move the iterator to the next node, and return its value.
	 * If the iterator originally pointed at the last member - we return NULL.
	 */
	list->current_element = list->current_element->next;
	CHECK_NULL(list->current_element, NULL)
	return list->current_element->data;
}

ListElement listGetCurrent(List list) {
	CHECK_NULL(list, NULL)
	CHECK_NULL(list->current_element, NULL)
	//The list exists and the iterator points to a valid member
	return list->current_element->data;
}

ListResult listInsertFirst(List list, ListElement element) {
	CHECK_NULL(list, LIST_NULL_ARGUMENT)
	/*
	 * Allocating a copy of the given element. If the allocation fails - we will
	 * get an error in the nodeCreate function, and then we'll free it
	 * accordingly. Otherwise, we'll implement the copy to the list.
	 */
	ListElement element_copy = list->copy_function(element);
	Node new_node = nodeCreate(element_copy, list->head->next, list->head);
	if(!new_node) {
		/*
		 * We get here because of two reasons:
		 * 1) An allocation has failed in the element copy
		 * 2) An allocation has failed in the node
		 * In both cases, we free the element copy (it's okay to free NULL),
		 * and return a memory error.
		 */
		list->free_function(element_copy);
		return LIST_OUT_OF_MEMORY;
	}
	list->head->next = new_node; //This is now the first element of the list
	(list->size)++;
	return LIST_SUCCESS;
}

ListResult listInsertLast(List list, ListElement element) {
	CHECK_NULL(list, LIST_NULL_ARGUMENT)
	Node iterator_start = list->head->next;
	if(!iterator_start)
		//The list is empty. We'll insert this element as the first one.
		return listInsertFirst(list, element);
	/*
	 * Allocating a copy of the given element. If the allocation fails - we will
	 * get an error in the nodeCreate function, and then we'll free it
	 * accordingly. Otherwise, we'll implement the copy to the list.
	 */
	ListElement element_copy = list->copy_function(element);
	while(iterator_start->next)
		iterator_start = iterator_start->next;
	Node new_node = nodeCreate(element_copy, NULL, list->current_element);
	if(!new_node) {
		list->free_function(element_copy);
		return LIST_OUT_OF_MEMORY;
	}
	iterator_start->next = new_node; //This is now the last element in the list
	(list->size)++;
	return LIST_SUCCESS;
}

ListResult listInsertBeforeCurrent(List list, ListElement element) {
	CHECK_NULL(list, LIST_NULL_ARGUMENT)
	CHECK_NULL(list->current_element, LIST_INVALID_CURRENT)
	/*
	 * Allocating a copy of the given element. If the allocation fails - we will
	 * get an error in the nodeCreate function, and then we'll free it
	 * accordingly. Otherwise, we'll implement the copy to the list.
	 */
	ListElement element_copy = list->copy_function(element);
	Node new_node = nodeCreate(element_copy, list->current_element,
	list->current_element->previous);
	if(!new_node) {
		list->free_function(element_copy);
		return LIST_OUT_OF_MEMORY;
	}
	list->current_element->previous = new_node; //Inserted before the iterator!
	return LIST_SUCCESS;
}

ListResult listInsertAfterCurrent(List list, ListElement element) {
	CHECK_NULL(list, LIST_NULL_ARGUMENT)
	CHECK_NULL(list->current_element, LIST_INVALID_CURRENT)
	/*
	 * Allocating a copy of the given element. If the allocation fails - we will
	 * get an error in the nodeCreate function, and then we'll free it
	 * accordingly. Otherwise, we'll implement the copy to the list.
	 */
	ListElement element_copy = list->copy_function(element);
	Node new_node = nodeCreate(element_copy, list->current_element->next,
	list->current_element);
	if(!new_node) {
		list->free_function(element_copy);
		return LIST_OUT_OF_MEMORY;
	}
	list->current_element->next = new_node; //Inserted after the iterator!
	(list->size)++;
	return LIST_SUCCESS;
}

ListResult listRemoveCurrent(List list) {
	CHECK_NULL(list, LIST_NULL_ARGUMENT)
	CHECK_NULL(list->current_element, LIST_INVALID_CURRENT)
	//The list is not empty and the iterator points to a valid member
	//We change the node that comes before the iterator to have the node that
	//after the iterator as its next node
	list->current_element->previous->next = list->current_element->next;
	list->free_function(list->current_element->data);
	free(list->current_element);
	list->current_element = NULL; //Iterator is now NULL
	(list->size)--;
	return LIST_SUCCESS;

}

ListResult listSort(List list, CompareListElements compareElement) {
	CHECK_NULL(list, LIST_NULL_ARGUMENT)
	CHECK_NULL(compareElement, LIST_NULL_ARGUMENT)
	//The list exists and the compare function is valid. We can proceed to sort
	int size = list->size;
	if(size<2) return LIST_SUCCESS; //A list of one (or 0) is already sorted.
	//The "index" of the iterator is saved so we can assign it to the iterator
	int iterator_index = getListIteratorIndex(list);
	while (bubble(list, (size--), compareElement));
	/*
	 * The list is now sorted, thanks to the glory of bubble sort.
	 * All that's left is to assign the iterator with the new element that took
	 * the place of the previous one in that index:
	 */
	list->current_element = indexedInterator(list, iterator_index);
	return LIST_SUCCESS;
}

List listFilter(List list, FilterListElement filterElement, ListFilterKey key){
	CHECK_NULL(list, NULL);
	CHECK_NULL(filterElement, NULL);
	//The list and the filter function exist
	List filtered_list = listCreate(list->copy_function, list->free_function);
	CHECK_NULL(filtered_list, NULL);
	listGetFirst(list);
	//Allocation was a success. We begin iterating over the original list:
	while(list->current_element) {
		if(filterElement(list->current_element->data, key)) {
			if(listInsertLast(filtered_list, list->current_element->data)!=
			LIST_SUCCESS) break; //Allocation failure
		}
		list->current_element = list->current_element->next;
	}
	if(list->current_element) {
		/*
		 * We didn't get to finish iterating over the entire list as a result
		 * of a memory allocation failure. Destroy and return NULL
		 */
		listDestroy(filtered_list);
		return NULL;
	}
	if(filtered_list->head->next)
		filtered_list->current_element = filtered_list->head->next;
	return filtered_list;
}

ListResult listClear(List list) {
	CHECK_NULL(list, LIST_NULL_ARGUMENT)
	listGetFirst(list);
	Node temp = NULL;
	while(list->current_element) {
		temp = list->current_element;
		list->free_function(temp->data);
		list->current_element = list->current_element->next;
		free(temp);
	}
	list->current_element = NULL;
	list->size=0;
	return LIST_SUCCESS;
}

void listDestroy(List list) {
	listClear(list);
	free(list->head);
	free(list);
}
