/***************************************************************************
 *   Copyright (C) 2009 by Leonardo de Miranda Borba, Adriano Zanette      *
 *   lmborba@gmail.com, adrzanette@gmail.com                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "hash_table.h"


int hashify(char * name) {

  char * p;
  unsigned int h = 0;
  unsigned int g;
  
  for (p = name; (*p) != EOST; p = p+1) {
    
    h = (h << 4) + (*p);
    
    if (g = h & 0xf000000) {
      
      h = h ^ (g>>24);
      h = h ^ g;
      
    };
    
  };
  
  return (h % PRIME);
  
};


int compare(char * str1, char * str2) {
  
  char * i1;
  char * i2;
  
  i1 = str1;
  i2 = str2;
  
  while ((*i1 != 0) && (*i2 != 0)) {
    
    if (*i1 != *i2) {
      break;
    };

    i1++;
    i2++;

  };

  return (*i1 - *i2);

};


int init_table(symbol_t* table) {

  int i;
  
  if(table == NULL) {
    return __TRUE;
  };
  
  for(i=0; i<PRIME; i++) {
    (table->ll)[i] = NULL;
  };
  
  return __FALSE;

};


void free_linkedlist(linkedlist* ll) {
	
  linkedlist* next;

  next = ll;

  while(next != NULL){

    ll = ll->next;
    free(next->head);
    free(next);
    next = ll;

  };
  
};


void free_table(symbol_t* table) {
  
  int i;
  
  for(i=0; i<PRIME; i++) {
    free_linkedlist((table->ll)[i]);
  };

};


int create_new(linkedlist ** enter, entry_t * insertion, linkedlist * next) {
  
  *enter = (linkedlist *) malloc(sizeof(linkedlist));

  if (!(*enter)) {

    return __FALSE;
    
  };
  
  (*enter)->head = insertion;
  
  (*enter)->next = next;

  return __TRUE;
  
};


int insert(symbol_t * table, entry_t * entry) {

  int hash_value;
  int comp_value;
  linkedlist * aux;
  linkedlist * anter;
  linkedlist * create;

  hash_value = hashify(entry->name);

  if (!((table->ll)[hash_value])) {
    create_new(&((table->ll)[hash_value]),entry,NULL);
    return __FALSE;
  };

  anter = NULL;

  for (aux = (table->ll)[hash_value]; aux != NULL; aux = aux->next) {

    comp_value = compare((aux->head)->name,entry->name);

    if (comp_value == 0) {
      if (((aux->head)->ip) == (entry->ip)) return ERRO;
    } else if (comp_value > 0) {
      break;
    };

    anter = aux;

    
  };

  if (anter == NULL) {
    
    if (create_new(&((table->ll)[hash_value]),entry,aux)) {
	return __FALSE;
    } else {
        return ERRO;
    }

  } else {

    if (!create_new(&create,entry,aux)) {
	return ERRO;
    };

    anter->next = create;

    return __FALSE;

  };

};

linkedlist* lookup(symbol_t * table, char* name) {

  int index;
  linkedlist * ll_aux;
  int found = 0;
  linkedlist * ret;
  linkedlist * ret_aux;	
  
  ret = NULL;
  index = hashify(name);
  
  for (ll_aux = (table->ll)[index]; ll_aux != NULL; ll_aux = ll_aux->next) {
    if (!compare(ll_aux->head->name, name)){
      if (ret == NULL) {
	ret = (linkedlist *) malloc(sizeof(linkedlist));
	ret->head = ll_aux->head;
	ret->next = NULL;
      } else {
	ret_aux = (linkedlist *) malloc(sizeof(linkedlist));
	ret_aux->head = ll_aux->head;
	ret_aux->next = ret;
	ret = ret_aux;
      }
    }
  }

  return ret;
}
