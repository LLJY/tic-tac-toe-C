#include <stdio.h>
#include <include/tui.h>
#include <include/linked_list.h>
#include <include/gui.h>

int main(int argc, char **argv){
    // startGameUi();
    launch_gui(argc, argv);
    return 0;
}

void testLinkedList(){
    Node* head;

    // create a ridiculous amount of nodes
    head = createNode(1, 1, 1, NULL, NULL);
    for(int j = 0; j < 10000; j++){
        for(int i = 0; i<10; i++){
            Node* curMove = createNode(1, i, i, NULL, NULL);
            insertNodeHead(&head, curMove);
        }
    }
    printList(head);
    destroyList(head); // test the free() function
    return;
}