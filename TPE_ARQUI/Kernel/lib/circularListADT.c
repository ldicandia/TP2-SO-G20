
#include <stdlib.h>
#include "circularListADT.h"
#include <memoryManager.h>
#include <videoDriver.h>

typedef struct circularListCDT {
	CNode *tail;
	int size;
	CNode *current;
} circularListCDT;

void initList_circular(circularListADT list) {
	list->tail = NULL;
	list->size = 0;
}

CNode *append_circular(circularListADT list, void *data) {
	CNode *newNode = (CNode *) allocMemory(sizeof(CNode));
	newNode->data  = data;

	if (list->tail == NULL) {
		newNode->next = newNode;
		list->tail	  = newNode;
	}
	else {
		newNode->next	 = list->tail->next;
		list->tail->next = newNode;
		list->tail		 = newNode;
	}
	list->size++;
	return newNode;
}

int removeNode_circular(circularListADT list, void *data) {
	if (list->tail == NULL)
		return 0;

	CNode *prev = list->tail, *curr = list->tail->next;

	do {
		if (curr->data == data) {
			if (curr == prev) {
				list->tail = NULL;
			}
			else {
				prev->next = curr->next;
				if (curr == list->tail)
					list->tail = prev;
			}
			freeMemory(curr);
			list->size--;
			return 1;
		}
		prev = curr;
		curr = curr->next;
	} while (curr != list->tail->next);

	return 0;
}

void destroyList_circular(circularListADT list) {
	if (list->tail == NULL)
		return;

	CNode *curr = list->tail->next;
	CNode *next;

	do {
		next = curr->next;
		freeMemory(curr);
		curr = next;
	} while (curr != list->tail->next);

	list->tail = NULL;
	list->size = 0;
}

int hasNext_circular(const circularListADT list) {
	return list->current != NULL;
}

void *next_circular(circularListADT list) {
	if (!hasNext(list)) {
		driver_printStr("No hay elementos en la lista\n",
						(Color) {0xFF, 0x00, 0x00});
		return NULL;
	}

	void *value = list->current->data;

	if (list->current == list->tail) {
		list->current = NULL;
	}
	else {
		list->current = list->current->next;
	}

	return value;
}

void resetIterator(circularListADT list) {
	list->current = list->tail ? list->tail->next : NULL;
}