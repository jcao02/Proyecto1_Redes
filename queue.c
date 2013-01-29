#include <stdio.h>
#include <stdlib.h>
#include "errores.h"
#include "queue.h"

struct box {
	distr elem;
	struct box *next, *prev;
};

struct queue {
	box *first, *last;
	int size;
};

/*
 *
 */
queue create_queue() {

	queue q;

	if ((q = (queue) malloc(sizeof(struct queue))) == NULL) {
		errorMem();
		return NULL;
	}

	/*Inicializa atributos*/
	q->first = NULL;
	q->size = 0;

	return q;
}

/*
 *
 */
void clear_queue(queue *q) {
	
	void *e;

	/*Si pasaron una direccion vacia*/
	if (q == NULL) {
		return;
	}

	/*Si la cola no esta inicializada*/
	if (*q == NULL) {
		return;
	}

	/*Libera la memoria de todos los elementos*/
	while (!is_empty(*q)) {
		e = get(*q);
		free (e);
	}

	free(*q);

	*q = NULL;
}

/*
 *
 */
box *create_box(distr e) {

	box *b;

	/*Pide memoria para la estructura box de la cola*/
	if ((b = (box *) malloc(sizeof(box))) == NULL) {
		errorMem();
		/*Si no hay memoria, retorna false*/
		return 0;
	}

	/*Asigna el elemento de la box*/
	b->elem = e;
	/*Inicializa apuntadores*/
	b->next = NULL;
	b->prev = NULL;

	return b;
}

/*
 *
 */
int is_empty(queue q) {

	if (q == NULL)
		return 1;

	return !q->size;
}

/*
 *
 */
int add(queue *q, distr e) {

	box *b, *aux;

	if (q == NULL) {
		return 0;
	}

	/*Si la cola no ha sido inicializada*/
	if (*q == NULL) {
		/*Creo la cola, si no la creo devuelvo false*/
		if (!(*q = create_queue())) {
			return 0;
		}
	}

	/*Creo la caja con el elemento, si no la creo devuelvo false*/
	if (!(b = create_box(e))) {
		return 0;
	}

	/*Si la cola esta vacia*/
	if (is_empty(*q)) {
		(*q)->first = b;
		(*q)->last = b;
	/*Si la cola tiene elementos*/
	} else {
		for (aux = (*q)->first; 
			(aux != NULL) && (e->pr > aux->elem->pr); 
				aux = aux->next);

		/*Si es el elemento de menor prioridad*/
		if (aux == NULL) {
			b->prev = (*q)->last;
			(*q)->last->next = b;
			(*q)->last = b;
		/*Si es el elemento de mayor prioridad*/
		} else if (aux == (*q)->first) {
			b->next = aux;
			aux->prev = b;
			(*q)->first = b;
		/*Si esta en el "medio"*/
		} else {
			b->prev = aux->prev;
			b->next = aux;
			aux->prev = b;
			b->prev->next = b;

		}
	}

	++((*q)->size);
}

/*
 *
 */
 distr get(queue q) {

	box *aux;
	distr e;

	/*Si la cola esta vacia*/
	if (is_empty(q)) {
		return NULL;
	}

	aux = q->first;

	q->first = q->first->next;

	if (q->size > 1) {
		q->first->prev = NULL;
	}

	e = aux->elem;
	free(aux);

	--(q->size);

	return e;
}

/*
 *
 */
iterator create_iterator(queue q) {

	iterator it;

	if (is_empty(q)) {
		return NULL;
	}

	if ((it = (iterator) malloc(sizeof(box*))) == NULL) {
		errorMem();
		return NULL;
	}

	*it = q->first;

	return it;
}

distr next_it(iterator it) {

	distr e;

	/*Si el iterador llego al final o no ha sido inicializado*/
	if ((it == NULL) || (*it == NULL)) {
		return NULL;
	}

	e = (*it)->elem;

	*it = (*it)->next;

	return e;
}

distr prev_it(iterator it) {

	/*si el iterador llego al principio o no ha sido inicializado*/
	if ((it == NULL) || (*it == NULL)) {
		return NULL;
	}

	*it = (*it)->prev;

	return (*it)->prev->elem;
}