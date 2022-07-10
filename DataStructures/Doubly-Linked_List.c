// 0.Documentation Section
// Doubly-Linked_List.c
// This program implements the data structure 'doubly-linked lists' and explores their operations 
// Authors: Khalil Khawli
// Date: July 9, 2022
// Concepts explored in this file are from Harvard University's 'Introduction to Computer Science' course
// Specifically this Youtube video: https://www.youtube.com/watch?v=FHMPswJDCvU

// 1. Pre-processor Directives Section
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// 2. Declarations Section
typedef struct dllist
{
    int number;
    struct dllist *prev; 
    struct dllist *next;
} dllnode;

// Global Variables:
dllnode* listHead;

// Number of operations that can be done to a linked list:
// 1. Create a linked list when it doesn't already exist
// 2. Search through a linked list to find an element
// 3. Insert a new node into the linked list
// 4. Delete a single element from a linked list
// 5. Delete an entire linked list

// Function Prototypes
dllnode* create(int value);
bool find(dllnode* head, int value);
dllnode* insert(dllnode* head, int value);
void delete_node(dllnode* target);
void destroy(dllnode* head);
void print_list(dllnode* head);

// 3. Subroutines Section
int main(void)
{
    // Tail of the list
    dllnode* list = create(0);
    listHead = list;
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

    print_list(list);
    // Delete value 7
    delete_node(list -> next -> next);
    print_list(list);

    destroy(list);
    return 0;
}

// Operation 1: Create a linked list
dllnode* create(int value) 
{
    dllnode *n = (dllnode*) malloc(sizeof(dllnode));
    if (n != NULL)
    {
        n -> number = value;
        n -> prev = NULL;
        n -> next = NULL;
    }
    return n;
}

// Operation 2: Search through a linked list to find an element
bool find(dllnode* head, int value)
{
    // Set pointer to the head of the linked list
    // Check that the pointer is not pointing to NULL
    // Assign the next pointer in the linked list chain to the traversal pointer every iteration of the 'for loop'
    for (dllnode *trav = head; trav != NULL; trav = trav -> next)
    {
        if (value == trav -> number)
        {
            return true;
        }
    }
    return false;
}

// Operation 3: Insert a new node into the linked list
// Steps involved:
//   a. Dynamically allocate space for a new dllnode
//   b. Check to make sure we didn't run out of memory 
//   c. Populate and insert the node at the beginning of the linked list
//   d. Fix the prev pointer of the old head of the linked list
//   e. Return a pointer to the new head of the linked list
dllnode* insert(dllnode* head, int value)
{
    dllnode* newNode = create(value);
    if (newNode != NULL)
    {
        newNode -> next = head; 
        head -> prev = newNode;
    }
    return newNode;
}

// Operation 4: Delete a single element from a linked list
// Steps involved:
//   a. Fix the pointers of the surrounding nodes to "skip over" target
//   b. Free Target
void delete_node(dllnode* target)
{
    // Beginning of list
    if ((target -> prev == NULL) && (target -> next != NULL) )
    {
        listHead = target -> next;
        target -> prev -> next = NULL;
        free(target);
    } 
    // Middle of the list
    else if ((target -> prev != NULL) && (target -> next != NULL) )
    {
        target -> prev -> next = target -> next;
        target -> next -> prev = target -> prev;
        free(target);
    }
    // End of the list
    else if ((target -> prev != NULL) && (target -> next != NULL) )
    {
        target -> prev -> next = NULL;
        free(target); 
    }
    return;
}

// Operation 5: Delete an entire linked list 
void destroy(dllnode* head)
{
    // deletes from head to tail
	while (head != NULL)
	{
		dllnode *tmp = head->next;
		free(head);
		head = tmp;
	}
    return;
}

void print_list(dllnode* head)
{
    for (dllnode *trav = head; trav != NULL; trav = trav -> next)
    {
        printf("List Number: %i\n", trav -> number);
    } 
    return;
}

