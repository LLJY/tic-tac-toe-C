#include <util.h>

typedef struct Node{
    PlayerType player; // 0 for player 1, 1 for player 2 -1 for AI
    int row;
    int col;
    struct Node* Next;
    struct Node* Prev;

}Node;

/// @brief Creates a new node for linkedlist
/// @param player player value, 1 for player, -1 for AI, 0 for AI
/// @param row row index of the placed nought or cross
/// @param col col index of the placed nought or cross
/// @param next pointer to the next element
/// @param prev pointer to the previous element
/// @return 
extern Node* createNode(PlayerType player, int row, int col, Node* next, Node* prev);

/// @brief Inserts a new head of the linkedlist and updates the current linked list HEAD pointer
/// @param head pointer to pointer of HEAD
/// @param node value of new node, can be created with createNode()
extern void insertNodeHead(Node** head, Node* node);

/// @brief Inserts a new head of the linkedlist and updates the current linked list TAIL pointer
/// @param tail pointer to pointer of TAIL
/// @param node value of new node, can be created with createNode()
extern void insertNodeTail(Node** tail, Node* node);

/// @brief Frees the memory from the linked list.
/// @param head top of the list.
extern void printList(Node* head);

extern void destroyList(Node* head);