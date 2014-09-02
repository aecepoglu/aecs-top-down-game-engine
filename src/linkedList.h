#ifndef LINKED_LIST_H
#define LINKED_LIST_H

struct LinkedListNode {
	struct LinkedListNode *next, *prev;
	void *data;
};

/* remove node from head */
void linkedList_remove( struct LinkedListNode **head, struct LinkedListNode *node) ;

/* pushes node before the head */
void linkedList_push( struct LinkedListNode **head, void *data) ;

/* finds node with given data in linked-list with given head */
struct LinkedListNode* linkedList_find( struct LinkedListNode *head, void *data) ;

void linkedList_free( struct LinkedListNode *head);

#endif
