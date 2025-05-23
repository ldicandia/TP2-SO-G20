#ifndef CIRCULAR_LIST_ADT_H
#define CIRCULAR_LIST_ADT_H

typedef struct circularListCDT *circularListADT;

typedef struct CNode {
	void *data;
	struct CNode *next;
} CNode;

// Inicializa una lista vacía
void initList_circular(circularListADT list);

// Agrega un elemento al final
CNode *append_circular(circularListADT list, void *data);

// Elimina el primer nodo con el valor dado
int removeNode_circular(circularListADT list, void *data);

// Libera toda la memoria de la lista
void destroyList_circular(circularListADT list);

int hasNext_circular(const circularListADT list);

void *next_circular(circularListADT list);

void resetIterator(circularListADT list);

#endif
