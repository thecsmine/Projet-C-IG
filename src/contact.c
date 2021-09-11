/*
  Module adapté du TP fil rouge
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "contact.h"


/*
  Insère un nouveau contact dans la liste de contact _cptr_ construit à partir des nom et
  numéro passés en paramètre.
  La liste de contact _cptr_ n'est pas vide, n'est pas le pointeur NULL.
  Si il existait déjà un contact du même nom, son numéro est remplacé et la fonction
  retourne une copie de l'ancien numéro. Sinon, la fonction retourne NULL.
*/
ei_widgetclass_t *contact_insert(struct cellule **cptr, const char *name, ei_widgetclass_t *wclass_ptr) {
	struct cellule sentinelle = {NULL, *cptr};
	struct cellule *sent = &sentinelle;
	while (sent->next != NULL) {
		sent = sent->next;
		if (strcmp(sent->contact->name, name) == 0) {
			ei_widgetclass_t *old = sent->contact->wclass_ptr;
			sent->contact->wclass_ptr = wclass_ptr;
			return old;
		}
	}
	struct contact *cont = malloc(sizeof(struct contact)); // création du contact suivant
	cont->name = name;
	cont->wclass_ptr = wclass_ptr;
	struct cellule *cel = malloc(sizeof(struct cellule));
	cel->contact = cont;
	cel->next = NULL;
	sent->next = cel;
	*cptr = sentinelle.next; // on remplace le pointeur vers la cellule
	return NULL;
}

/*
  Retourne le numéro associé au nom _name_ dans la liste de contact _cptr_
  Si aucun contact ne correspond, retourne NULL.
*/
ei_widgetclass_t *contact_search(struct cellule *cptr, const char *name) {
	struct cellule sentinelle = {NULL, cptr};
	struct cellule *sent = &sentinelle;
	while (sent->next != NULL) {
		sent = sent->next;
		if (strcmp(sent->contact->name, name) == 0) {
			return sent->contact->wclass_ptr;
		}
	}
	return NULL;
}

/*
  Supprime le contact de nom _name_ de la liste de contact _cptr_
  Si aucun contact ne correspond, ne fait rien.
*/
void contact_delete(struct cellule **cptr, const char *name) {
	struct cellule sentinelle = {NULL, *cptr};
	struct cellule *sent = &sentinelle;
	while (sent->next != NULL) {
		if (strcmp(sent->next->contact->name, name) == 0) {
			struct cellule *to_delete = sent->next;
			sent->next = to_delete->next;
			*cptr = sentinelle.next; // on remplace le pointeur vers la cellule
			free(to_delete->contact);
			free(to_delete);
			return;
		}
		sent = sent->next;
	}
}


/*
  Libère la mémoire associée à la liste de contact _cptr_.
*/
void contact_free(struct cellule *cptr) {
	if (cptr == NULL) {
		return;
	}

	while (cptr->next != NULL) {
		struct cellule *to_delete = cptr->next;
		cptr->next = to_delete->next;
		free(to_delete->contact);
		free(to_delete);
	}
	free(cptr->contact);
	free(cptr);
}

/*
  Affiche sur la sortie standard le contenu de la liste de contact _cptr_.
*/
void contact_print(struct cellule *cptr) {
	if (cptr == NULL) {
		return;
	}
	while (cptr->next != NULL) {
		printf("%s", cptr->contact->name);
		cptr = cptr->next;
	}
	printf("%s", cptr->contact->name);
}