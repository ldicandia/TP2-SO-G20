// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <defs.h>
#include <linkedListADT.h>
#include <memoryManager.h>
#include <stdlib.h>
#include <videoDriver.h>
typedef struct LinkedListCDT {
	Node *first;
	Node *last;
	Node *current;
	int len;
} LinkedListCDT;

LinkedListADT createLinkedListADT() {
	LinkedListADT list = (LinkedListADT) allocMemory(sizeof(LinkedListCDT));
	if (list == NULL) {
		driver_printStr("\nList Allocation Failed", (Color) {0xFF, 0x00, 0x00});
		return NULL;
	}
	list->len	  = 0;
	list->first	  = NULL;
	list->last	  = NULL;
	list->current = NULL;
	return list;
}

Node *appendElement(LinkedListADT list, void *data) {
	if (list == NULL)
		return NULL;
	Node *newNode = (Node *) allocMemory(sizeof(Node));
	if (newNode == NULL) {
		driver_printStr("\nNode Allocation Failed", (Color) {0xFF, 0x00, 0x00});
		return NULL;
	}
	newNode->data = data;
	return appendNode(list, newNode);
}

Node *appendNode(LinkedListADT list, Node *node) {
	if (list == NULL)
		return NULL;
	node->next = NULL;
	if (list->len > 0)
		list->last->next = node;
	else
		list->first = node;
	node->prev = list->last;
	list->last = node;
	list->len++;
	return node;
}

Node *prependNode(LinkedListADT list, Node *node) {
	if (list == NULL)
		return NULL;
	node->prev = NULL;
	if (list->len > 0)
		list->first->prev = node;
	else
		list->last = node;
	node->next	= list->first;
	list->first = node;
	list->len++;
	return node;
}

Node *getFirst(LinkedListADT list) {
	if (list == NULL)
		return NULL;
	return list->first;
}

int isEmpty(LinkedListADT list) {
	if (list == NULL)
		return -1;
	return !list->len;
}

int getLength(LinkedListADT list) {
	if (list == NULL)
		return -1;
	return list->len;
}

void *removeNode(LinkedListADT list, Node *node) {
	if (list == NULL || node == NULL)
		return NULL;

	if (list->first == node)
		list->first = node->next;
	else
		node->prev->next = node->next;

	if (list->last == node)
		list->last = node->prev;
	else
		node->next->prev = node->prev;

	list->len--;
	void *data = node->data;
	node->next = NULL;
	node->prev = NULL;
	// freeMemory(node);
	return data;
}

// Atención: Usar funciones de agregado/borrado cuando se itera sobre la lista
// puede causar comportamiento indefinido.
void begin(LinkedListADT list) {
	if (list == NULL)
		return;
	list->current = list->first;
}

int hasNext(LinkedListADT list) {
	if (list == NULL)
		return -1;
	return list->current != NULL;
}

void *next(LinkedListADT list) {
	if (!hasNext(list))
		return NULL;
	void *data	  = list->current->data;
	list->current = list->current->next;
	return data;
}

void freeLinkedListADTDeep(LinkedListADT list) {
	Node *current = list->first;
	Node *next;
	while (current != NULL) {
		next = current->next;
		freeMemory(current);
		current = next;
	}
	freeLinkedListADT(list);
}

void freeLinkedListADT(LinkedListADT list) {
	freeMemory(list);
}