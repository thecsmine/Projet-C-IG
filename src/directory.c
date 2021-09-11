#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "contact.h"
#include "directory.h"
#include "hash.h"

/*
    Crée un nouvel annuaire contenant _len_ listes vides.
*/
struct dir* dir_create(uint32_t len) {
    struct dir* my_dir = malloc(sizeof(struct dir));
    if (len < 10) {
        len = 10;
    }
    my_dir->contact_number = 0;
    my_dir->array_len = len;
    my_dir->array = malloc(len * sizeof(struct cellule)); // tableau initialisé avec des pointeurs nuls
    for (uint32_t i = 0; i < len; i++) {
        my_dir->array[i] = NULL;
    }
    return my_dir;
}

/*
    Insère un nouveau contact dans l'annuaire _dir_, construit à partir des nom et
    numéro passés en paramètre. Si il existait déjà un contact du même nom, son
    numéro est remplacé et la fonction retourne une copie de l'ancien numéro.
    Sinon, la fonction retourne NULL.
*/
ei_widgetclass_t *dir_insert(struct dir* dir, const char* name, ei_widgetclass_t *wclass_ptr) {
    uint32_t index = hash(name) % (dir->array_len);
    ei_widgetclass_t *old = contact_insert(&(dir->array[index]), name, wclass_ptr);
    if (old == NULL) {
        dir->contact_number++;
    }
    if (dir->contact_number > 0.75 * dir->array_len) {
        dir_resize_double(dir);
    }
    return old;
}

/*
    Retourne le numéro associé au nom _name_ dans l'annuaire _dir_.
    Si aucun contact ne correspond, retourne NULL.
*/
ei_widgetclass_t *dir_lookup_num(struct dir* dir, const char* name) {
    uint32_t index = hash(name) % (dir->array_len);
    return contact_search(dir->array[index], name);
}

/*
    Supprime le contact de nom _name_ de l'annuaire _dir_. Si aucun contact ne
    correspond, ne fait rien.
*/
void dir_delete(struct dir* dir, const char* name) {
    uint32_t index = hash(name) % (dir->array_len);
    contact_delete(&(dir->array[index]), name);
    dir->contact_number--;
    if (dir->contact_number < 0.15 * dir->array_len) {
        dir_resize_divise(dir);
    }
}

/*
    Libère la mémoire associée à l'annuaire _dir_.
*/
void dir_free(struct dir* dir) {
    for (uint32_t i = 0; i < dir->array_len; i++) {
        contact_free(dir->array[i]);
    }
    free(dir->array);
    free(dir);
}

/*
    Affiche sur la sortie standard le contenu de l'annuaire _dir_.
*/
void dir_print(struct dir* dir) {
    for (uint32_t i = 0; i < dir->array_len; i++) {
    	printf("[%u] ", i);
        contact_print(dir->array[i]);
        printf("\n");
    }
}

/*
    Double la taille de l'annuaire _dir_.
*/
void dir_resize_double(struct dir* dir) {
    uint32_t old_len = dir->array_len;
    dir->array_len *= 2;
    dir_resize(dir, old_len);
}

/*
    Divise la taille de l'annuaire _dir_ en deux.
*/
void dir_resize_divise(struct dir* dir) {
    uint32_t old_len = dir->array_len;
    dir->array_len /= 2;
    if (dir->array_len <= 10) {
        dir->array_len = 10;
    }
    dir_resize(dir, old_len);
}

/*
    Re-remplit l'annuaire _dir_ selon sa nouvelle taille (array_len)
*/
void dir_resize(struct dir* dir, uint32_t old_len) {
    struct cellule** new_array = malloc(dir->array_len * sizeof(struct cellule)); // initialisation nouveau tableau vide
    for (uint32_t i=0; i<dir->array_len; i++) {
        new_array[i] = NULL;
    }
    for (uint32_t i = 0; i < old_len; i++) { // remplissage nouveau tableau
        for (struct cellule* cel = dir->array[i]; cel != NULL; cel = cel->next) {
            uint32_t hash_index = hash(cel->contact->name) % (dir->array_len);
            contact_insert(&(new_array[hash_index]), cel->contact->name, cel->contact->wclass_ptr);
        }
        contact_free(dir->array[i]); // liste de contact en place i libérée
    }
    free(dir->array);
    dir->array = new_array;
}