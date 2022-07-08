// 0.Documentation Section
// Singly-Linked_List.c
// This program implements the data structure 'linked lists' and explores their operations 
// Authors: Khalil Khawli
// Date: July 8, 2022
// Concepts explored in this file are from Harvard University's 'Introduction to Computer Science' course
// Specifically this Youtube video: https://www.youtube.com/watch?v=zQI3FyWm144

// What is a linked list?
// A linked list is comprised of nodes
// A linked list node is an abstract special kind of structure
//
// It has two members:
//  - Data
//  - A pointer to another node of the same type

// 1. Pre-processor Directives Section
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 2. Declarations Section
typedef struct sllist
{
    int number;
    struct sllist *next; // Pointer to another node
} sllnode;

// Function Prototypes
sllnode* create(int value);
bool find(sllnode* head, int value);
sllnode* insert(sllnode* head, int value);
void destroy(sllnode* head);
void print_list(sllnode* head);

// Working with linked lists effectively
// Number of operations that can be done to a linked list:
// 1. Create a linked list when it doesn't already exist
// 2. Search through a linked list to find an element
// 3. Insert a new node into the linked list
// 4. Delete a single element from a linked list
// 5. Delete an entire linked list


// 3. Subroutines Section
int main(void)
{
    // Tail of the list
    sllnode* list = create(0);
    if (list == NULL)
    {
        printf("There has been an error");
        return 1;
    }
    printf("Number: %i \n", list -> number);
    
    // Populate list with numbers 0-9
    for (int i = 1; i < 10; i++)
    {
        list = insert(list, i);
        if (list == NULL)
        {
            printf("There has been an error");
            destroy(list);
            return 1;
        }
    }

    bool exists = find(list, 6);
    printf("Search for 6: %i \n", exists);
    exists = find(list, 13); 
    printf("Search for 13: %i \n", exists);

    print_list(list);
    destroy(list);
    return 0;
}

// Operation 1: Create a linked list
// Steps involved:
//   a. Dynamically allocate space for a new sllnode
//   b. Check to make sure we didn't run out of memory 
//   c. Initialize the node's value field
//   d. Initialize the nodes next field
//   e. Return a pointer to the newly created node
sllnode* create(int value) 
{
    sllnode *n = (sllnode*) malloc(sizeof(sllnode));
    if (n != NULL)
    {
        n -> number = value;
        n -> next = NULL;
    }
    return n;
}

// Operation 2: Search through a linked list to find an element
// bool find(sllnode* head, VALUE val);
// Steps involved:
//   a. Create a traversal pointer pointing to the lists head.
//   b. If the current node's val field is what we're looking for, report success.
//   c. If not, set traversal pointer to the next pointer in the list and go back to step b.
//   d. If you've reached the end of the list, report failure.
bool find(sllnode* head, int value)
{
    // Set pointer to the head of the linked list
    // Check that the pointer is not pointing to NULL
    // Assign the next pointer in the linked list chain to the traversal pointer every iteration of the 'for loop'
    for (sllnode *trav = head; trav != NULL; trav = trav -> next)
    {
        if (value == trav -> number)
        {
            return true;
        }
    }
    return false;
}

// Operation 3: Insert a new node into the linked list
// sllnode* insert(sllnode* head, VALUE val);
// Steps involved:
//   a. Dynamically allocate space for a new sllnode
//   b. Check to make sure we didn't run out of memory 
//   c. Populate and insert the node at the beginning of the linked list
//   d. Return a pointer to the new head of the linked list
sllnode* insert(sllnode* head, int value)
{
    sllnode* newNode = create(value);
    if (newNode != NULL)
    {
        newNode -> next = head; 
    }
    return newNode;
}

// Operation 4: Delete a single element from a linked list
// This operation has been deliberately left unimplemented
// It is easier to implement this in a Doubly-Linked List instead

// Operation 5: Delete an entire linked list 
// void destroy(sllnode* head);
// Steps involved:
//   a. If you've reached a null pointer, stop.
//   b. Delete the rest of the list
//   c. Free current node
void destroy(sllnode* head)
{
	while (head != NULL)
	{
		sllnode *tmp = head->next;
		free(head);
		head = tmp;
	}
    return;
}

void print_list(sllnode* head)
{
    for (sllnode *trav = head; trav != NULL; trav = trav -> next)
    {
        printf("List Number: %i\n", trav -> number);
    } 
    return;
}
