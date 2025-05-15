
// listas ordenadas sin repetidos de cualquier tipo
#ifndef _orderedListADT_H
#define _orderedListADT_H
#include <memoryManager.h>
#include <PCB.h>

typedef struct list_cdt *orderedListADT;
typedef PCB *elem_type;

/*
 *  < 0 if e1 should be before e2 (i.e., e1 < e2 in ascending order)
 *  > 0 if e1 should be after e2 (i.e., e1 > e2)
 *  0 if they are "equal"
 */

typedef int (*t_compare)(elem_type e1, elem_type e2);

orderedListADT new_ordered_list(t_compare cmp);

void free_ordered_list(orderedListADT list);

int add_ordered_list(orderedListADT list,
					 elem_type elem); // retorna 1 si lo inserto, 0 si no.

int delete_ordered_list(orderedListADT list, elem_type elem);

size_t size_ordered_list(const orderedListADT list);

int is_empty_ordered_list(const orderedListADT list);

elem_type get(const orderedListADT list, size_t idx);

// le avisamos que vamos a recorrer la lista desde el primero
void ordered_list_to_begin(orderedListADT list);

// retorna true si hay pendientes que recorrer
int ordered_list_has_next(const orderedListADT list);

// me devuelve el siguiente. precondicion, has_next debe devolver true
elem_type ordered_list_next(orderedListADT list);
int ordered_list_delete_current(orderedListADT list);

#endif