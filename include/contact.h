/**
 *  @file	contact.h
 *  @brief	Manages the cells containing pointers to \ref ei_widgetclass_t type.
 *
 */

#ifndef _CONTACT_H_
#define _CONTACT_H_

#include "ei_widget.h"
#include "ei_widgetclass.h"

/*
  Un contact représente une association {nom, numéro}.
*/
struct contact {
	char *name;
	ei_widgetclass_t *wclass_ptr;
};

/* 
  Structure de données décrivant une liste chainée 
*/
struct cellule {
	struct contact *contact;
	struct cellule *next;
};

/* TOUT DOUX: à compléter */
/* Profitez de cette période sombre pour braver les interdits et rétablir le contact. */

/*
  Insère un nouveau contact dans la liste de contact _cptr_ construit à partir des nom et
  numéro passés en paramètre.
  La liste de contact _cptr_ n'est pas vide, n'est pas le pointeur NULL.
  Si il existait déjà un contact du même nom, son numéro est remplacé et la fonction
  retourne une copie de l'ancien numéro. Sinon, la fonction retourne NULL.
*/
extern ei_widgetclass_t *contact_insert(struct cellule **cptr, const char *name, ei_widgetclass_t *wclass_ptr);

/*
  Retourne le numéro associé au nom _name_ dans la liste de contact _cptr_
  Si aucun contact ne correspond, retourne NULL.
*/
extern ei_widgetclass_t *contact_search(struct cellule *cptr, const char *name);

/*
  Supprime le contact de nom _name_ de la liste de contact _cptr_
  Si aucun contact ne correspond, ne fait rien.
*/
extern void contact_delete(struct cellule **cptr, const char *name);

/*
  Libère la mémoire associée à la liste de contact _cptr_.
*/
extern void contact_free(struct cellule *cptr);

/*
  Affiche sur la sortie standard le contenu de la liste de contact _cptr_.
*/
extern void contact_print(struct cellule *cptr);

#endif /* _CONTACT_H_ */
