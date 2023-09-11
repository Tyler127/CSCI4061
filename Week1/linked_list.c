#include "linked_list.h"

// Head pointer for the linked list
Node *head = NULL;

Node *createNode(int value){
    Node *node =  (Node *)malloc(sizeof(Node));
    node->val = value;
    node->next = NULL;
    return node;
}

void insertLL(int value){

    // Create a new node pointer
    Node *newNode = createNode(value);

    // Case 1: Empty linked list
    if(head->next == NULL){
        head = newNode;
        return;
    }

    // Case 2: value < head->val
    if(value < head->val){
        newNode->next = head;
        head = newNode;
        return;
    }

    // Create node for traversal
    Node *currNode = head;

    // Case 3: value > last node in linked list
    if(value > head->val){
        while(currNode->next != NULL && currNode->next->val < value){
            currNode = currNode->next;
        }
        newNode->next = currNode->next;
        currNode->next = newNode;
    }

    // Case 4: insert value within linked list
}

void freeLL(){
    Node *nextNode = NULL;
    while(head){
        nextNode = head -> next;
        free(head);
        head = nextNode;
    }
    head = NULL;
}

void displayLL(){
    Node *trav = head;
    while(trav){
        printf("%d ", trav->val);
        trav = trav->next;
    }
    printf("\n");
}