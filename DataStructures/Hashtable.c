// Resources:
// Hash functions: https://www.youtube.com/watch?v=KyUTuwz_b7Q
// Hash tables: https://www.youtube.com/watch?v=nvzVHwrrub0

// Collison Resolution

// Open addressing
// - Linear probing
// - Plus 3 rehash
// - Quadratic probing
// - Double hashing

// Closed addressing
// - Linked lists

// A hash table contains two things:
// - A hash function that generates a hash code
// - An array storing our data

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define HASH_MAX 10
#define LENGTH 20

typedef struct sllist
{
    // 'LENGTH+1' includes the '/0' character for strings
    char name[LENGTH+1]; 
    struct sllist *next; // Pointer to another node
} node;

node *table[HASH_MAX];

unsigned int hash(const char* name);
bool insert(const char *name);
bool check(const char *name);
void destroy(void);

int main(void)
{
    
    insert("Ross");
    insert("Rachel");
    insert("Joey");
    insert("Pheobe");

    printf("Found Ross: %i\n", check("Ross"));
    printf("Found Rachel: %i\n", check("Rachel"));
    printf("Found Joey: %i\n", check("Joey"));
    printf("Found Pheobe: %i\n", check("Pheobe"));
    printf("Found Bob: %i\n", check("Bob"));

    destroy();
    return 0;
}
// Converts name to a number
unsigned int hash(const char *name)
{
    int sum = 0;
    for (int i = 0; name[i] != '\0'; i++)
    {
        // Case insensitive
        sum += tolower(name[i]);
    }
    return sum % HASH_MAX;
}
// Inserts node into hash table, returning true if successful, else false
bool insert(const char *name)
{
    // Get hash value from hash function
    unsigned int hash_value = hash(name);

    // Create a new node for each name
    // Dynamically allocate space for a new node
    node *n = (node*) malloc(sizeof(node));

    // Check to make sure we didn't run out of memory
    if (n == NULL)
    {
        printf("Out of memory");
        return false;
    }

    // Initialize the node's name field value
    strcpy(n->name, name);
    // Initialize the node's next field pointer
    n->next = table[hash_value];
    // Set head of linked list to point to the new node
    table[hash_value] = n;
    return true;
}

// Returns true if name is in hash table, else false
bool check(const char *name)
{
    unsigned int hash_value = hash(name);
    node *head = table[hash_value];

    while (head != NULL)
    {
        if (strcmp(name, head->name) == 0)
        {
            return true;
        }
        head = head->next;
    }
    return false;
}

// Free entire hash table from memory
void destroy(void)
{
    for (int i = 0; i < HASH_MAX; i++)
    {
        node *head = table[i];
        while (head != NULL)
        {
            node *tmp = head->next;
            free(head);
            head = tmp;
        }
    }
    return;
}
