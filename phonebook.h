#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <mongoc/mongoc.h>

void insert_contact(mongoc_collection_t *collection);
void delete_contact(mongoc_collection_t *collection);
void search_contact(mongoc_collection_t *collection);
void list_contacts(mongoc_collection_t *collection);

#endif