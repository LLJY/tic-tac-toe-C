#include <include/linked_list.h>

Node* createNode(PlayerType player, int row, int col, Node* next, Node* prev){
    Node* ret = malloc(sizeof(Node));
    
    // Check if malloc failed to allocate memory, sometimes it can happen if the OS is unable to alloc.
    if (unlikely(ret == NULL)) {
        fprintf(stderr, "Memory allocation failed in createNode! This might be an Operating System Issue! Terminating.\n"); // Print error to standard error
        exit(1); // Exit the program with an error code
    }

    ret->player = player;
    ret->row = row;
    ret->col = col;
    ret->Next = next;
    ret->Prev = prev;

    return ret;
}


void insertNodeHead(Node** head, Node* node){
    // assign the values to the new head, head has no Next, therefore NULL
    node->Prev = *head;
    node->Next = NULL;

    (*head)->Next = node;

    // update the pointer to the new head
    *head = node;
}

void insertNodeTail(Node** tail, Node* node){
    // assign the values to the new tail, tail has no Prev, therefore NULL
    node->Next = *tail;
    node->Prev = NULL;

    (*tail)->Prev = node;

    // update the pointer to the new tail
    *tail = node;
}

void printList(Node* head){
    int i = 0;
    while(head->Prev != NULL){
        println("node %d contains %d", i, head->row);
        head = head->Prev;
        i++;
    }
}

void destroyList(Node* head){
    while(head->Prev != NULL){
        Node* t = head;
        head = head->Prev;
        free(t);
    }
}