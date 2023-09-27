#include "linked_list.h"

int main(int argc, char *argv[]){
    char next = 'y', option;
    do{
        printf("---Sorted Linked List---\n");
        printf("Select option: \n");
        printf("1. Insert a value into Linked List \n");
        printf("2. Display Linked List \n");
        printf("3. Delete Linked list \n");
        printf("4. Exit\n");
        
        scanf(" %c", &option);
        switch(option){
            case '1':   
                int value;
                printf("Enter value: ");
                scanf("%d", &value);
                insertLL(value);
                break;
            
            case '2':   
                printf("Sorted Linked List\n");
                displayLL();
                break;
    
            case '3':   
                printf("Delete Linked List\n");
                freeLL();
                break;

            case '4':   
                next = 'n';
                break;
                
            default:
                printf("Invalid option selected!!!");
                next = 'y';
        }
        printf("\n");
    }while(next == 'y');

    freeLL();
    return 0;
}