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

#include "register.h"
#include <errors.h>
#include <netserver.h>
#include <stdio.h>

void evaluate_commands(char c) {

  return;

};

char * get_string(char * data) {

  int size;

  size = *((int *) data);
  data[sizeof(int)+size] = 0;

  return data+sizeof(int);

};

int join (int ip_address, char * ret_data, struct all_information * all_data) {
  
  linkedlist * ll_aux;

  ll_aux = (linkedlist *) malloc(sizeof(linkedlist));
  ll_aux->head = (entry_t *) malloc(sizeof(entry_t));
  ll_aux->head->ip = ip_address;
  ll_aux->next = all_data->servents;
  all_data->servents->next = ll_aux;
  
  return E_OK;

}

int handle_operations (int operation, void * data, void * ret_data, int ip_address,void * all_data) {

  switch (operation) {

  case 1: {
    return -join(ip_address,(char *) ret_data,(struct all_information *) all_data);
  };
  case 2: {
    //  return -publish(get_string((char *) data),(char *) ret_data,(struct all_information *) all_data);
  };
  case 3: {
    //return -search(get_string((char *) data),(char *) ret_data,(struct all_information *) all_data);
  };

  };

  return E_OK;

};

void init_program(struct all_information * all) {

  all_data->servents = NULL;
  
};

int main(int argv, char ** argc) {

  struct network * net;
  struct all_information all;
  char c;

  init_program(&all);

  net = init_network_server(handle_operations,(void *) &all);
  if (net == NULL) {
    printf("Erro Criando o Socket\n");
    return -1;
  };

  while (c != 'q') {

    c = getchar();
    
    evaluate_commands(c);

  };

  close(net->socket);

  //  end_network_server(net);
  
  return E_OK;

};
