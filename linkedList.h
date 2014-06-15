#ifndef LINKED_LIST_H
#define LINKED_LIST_H

struct LinkedListNode {
	struct LinkedListNode *next, *prev;
	void *data;
};

void linkedList_remove( struct LinkedListNode **head, struct LinkedListNode *node) ;
void linkedList_add( struct LinkedListNode **head, void *data) ;
struct LinkedListNode* linkedList_find( struct LinkedListNode *head, void *data) ;

#endif
