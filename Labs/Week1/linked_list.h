#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include <stdio.h>
#include <stdlib.h>

// Structure of a node in the linked list
struct node {
    int val;
    struct node *next;
};

// Alias for struct node
typedef struct node Node;

// Create a new linked list node and return the pointer
Node *createNode(int val);

// Insert value into the linked list at the correct location and return head
void insertLL(int value);

// free allocated memory for linked list
void freeLL();

// Display the linked list
void displayLL();

#endif