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
#include <string.h>

void evaluate_commands(char c) {

  return;

};

char * get_string(char * data) {

  int size;

  size = *((int *) data);
  data[sizeof(int)+size] = 0;

  return data+sizeof(int);

};

int connected(int ip_address, struct all_information * all_data) {

  linkedlist * ll_aux;
  
  for (ll_aux=all_data->servents;ll_aux;ll_aux=ll_aux->next) {

    if (ll_aux->head->ip == ip_address) {
      return 1;
    };

  };

  return 0;

};

int join (int ip_address, char * ret_data, struct all_information * all_data) {
  
  linkedlist * ll_aux;

  ll_aux = (linkedlist *) malloc(sizeof(linkedlist));
  if (!ll_aux) {
    ret_data[0] = INCORRECT_ANSWER + COMMAND_JOIN;
    return 1;
  };

  ll_aux->head = (entry_t *) malloc(sizeof(entry_t));
  if (!(ll_aux->head)) {
    free(ll_aux);
    ret_data[0] = INCORRECT_ANSWER + COMMAND_JOIN;
    return 1;
  };

  ll_aux->head->ip = ip_address;

  pthread_mutex_lock(&(all_data->servents_mutex));

  ll_aux->next = all_data->servents;
  all_data->servents = ll_aux;

  pthread_mutex_unlock(&(all_data->servents_mutex));

  ret_data[0] = CORRECT_ANSWER + COMMAND_JOIN;
  return 1;

}

int publish(char * name, int ip, char * ret_data, struct all_information * all) {

  entry_t* dado;
  int ret;
  int size;

  if (!(connected(ip,all))) {
    ret_data[0] = INCORRECT_ANSWER + COMMAND_PUBLISH;
    return 1;
  };
  
  dado = (entry_t *) malloc(sizeof(entry_t));
  if (!dado) {
    ret_data[0] = INCORRECT_ANSWER + COMMAND_PUBLISH;
    return 1;
  };
  size = strlen(name);
  dado->name = (char *) malloc(size+1);
  strncpy(dado->name,name,size+1);
  dado->ip = ip;

  pthread_mutex_lock(&(all->stable_mutex));
  ret = insert(&(all->stable), dado);
  pthread_mutex_unlock(&(all->stable_mutex));
  
  if (ret){
    ret_data[0] = INCORRECT_ANSWER + COMMAND_PUBLISH;
    return 1; /* não inseriu, caso já tenha um arquivo com mesmo nome e mesmo ip */
  };

  ret_data[0] = CORRECT_ANSWER + COMMAND_PUBLISH;
  return 1; /* inseriu certo */
  
};

int search(char * name, int ip, char * ret_data, struct all_information * all){

  linkedlist * ret;
  linkedlist * aux;
  int pos;

  if (!connected(ip,all)) {
    ret_data[0] = INCORRECT_ANSWER + COMMAND_SEARCH;
    return 1;
  };
  
  ret = lookup(&(all->stable), name);

  if (!ret) {
    ret_data[0] = INCORRECT_ANSWER + COMMAND_SEARCH;
    return 1;
  };

  pos = 0;

  for (aux=ret;aux;aux=aux->next) {

    if (connected(aux->head->ip,all)) {
      ret_data[pos++] = CORRECT_ANSWER + COMMAND_SEARCH;
      ret_data[pos++] = SEARCH_NOT_END;
      memcpy(ret_data+pos,(char *) &(aux->head->ip),sizeof(int));
      pos += sizeof(int);
    };

  };

  ret_data[pos-sizeof(int)-1] = SEARCH_END;

  return pos;

};

int handle_operations (int operation, void * data, void * ret_data, int ip_address,void * all_data) {

  switch (operation) {

  case 1: {
    return join(ip_address,(char *) ret_data,(struct all_information *) all_data);
  };
  case 2: {
    return publish(get_string((char *) data),ip_address,(char *) ret_data,(struct all_information *) all_data);
  };
  case 3: {
    return search(get_string((char *) data),ip_address,(char *) ret_data,(struct all_information *) all_data);
  };

  };

};

void init_program(struct all_information * all) {

  all->servents = NULL;
  
  init_table(&(all->stable));

  pthread_mutex_init(&(all->servents_mutex),NULL);
  pthread_mutex_init(&(all->stable_mutex),NULL);
  
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
