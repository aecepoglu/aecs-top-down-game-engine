#include <stdlib.h>

#include "linkedList.h"
#include "log.h"

void linkedList_remove( struct LinkedListNode **head, struct LinkedListNode *node) {
	if( node->next != NULL)
		node->next->prev = node->prev;
	if( node->prev != NULL)
		node->prev->next = node->next;
	else
		*head = node->next;
}

void linkedList_add( struct LinkedListNode **head, void *data) {
	struct LinkedListNode *newNode = (struct LinkedListNode*)malloc( sizeof( struct LinkedListNode));
	newNode->data = data;
	newNode->prev = NULL;

	if( *head == NULL) {
		*head = newNode;
		newNode->next = NULL;
	}
	else {
		(*head)->prev = newNode;
		newNode->next = *head;
		*head = newNode;
	}
}


struct LinkedListNode* linkedList_find( struct LinkedListNode *head, void *data) {
	struct LinkedListNode *node = head;

	while(node) {
		if(node->data == data)
			break;
		node = node->next;
	}
	return NULL;
}
