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

#include "netserver.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct all_data {

  struct network * net_data;

  int ip_address;

  int socket;
  
};

void * handler(void * data) {

  struct all_data * a_data;
  char buffer[BUF_SIZE];
  char buffer_out[BUF_SIZE];
  int size;
  int operation;
  int string_size;
  int ret_size;

  a_data = (struct all_data *) data;

  size = read(a_data->socket,buffer,SIZE_OPERATION);

  while (size >= 0) {

    operation = buffer[0];

    switch (operation) {

    case 2: case 3: {

      size = 0;
      while (size < SIZE_FILE_NAME_SIZE) {
	size += read(a_data->socket,buffer+size,SIZE_FILE_NAME_SIZE-size);
      };

      string_size = *((int *) buffer);

      size = 0;
      while (size < string_size) {
	size += read(a_data->socket,buffer+SIZE_FILE_NAME_SIZE+size,string_size-size);
      };

      break;

    };

    case 1: case 5: {
      break;
    };

    default: {
      return (void *) E_UNKNOWN_MESSAGE;
    };
      
    };

    ret_size = (*(a_data->net_data->callback))(operation,buffer,buffer_out,a_data->ip_address);

    if (ret_size < 0) {
      return (void *) -ret_size;
    } else if (ret_size > 0) {
      write(a_data->socket,buffer,ret_size);
    };

    size = read(a_data->socket,buffer,SIZE_OPERATION);

  };

  close(a_data->socket);

  return E_OK;

};

/**
 * \brief A thread principal da rede
 * 
 * \param data Os dados da rede
 * \return Valor de erro ou sucesso
 * \retval E_OK Sucesso
 * \retval Outro Erro
 */
void * main_function(void * data) {

  struct network * net_data;

  int client_socket;

  struct sockaddr_in client_address;

  pthread_t new;  
  
  struct all_data * a_data;
  
  int address_size;

  net_data = (struct network *) data;

  address_size = sizeof(client_address);

  while (!(net_data->end)) {
    
    client_socket = accept(net_data->socket, (struct sockaddr *) &client_address,&address_size);
    if (client_socket < 0) {
      exit(E_ERROR_ACCEPTING_CONNECTION);
    };

    //#ifdef DEBUG
    fprintf(F_MESSAGES, "Cliente conectado no IP: %s\n",inet_ntoa(client_address.sin_addr));
    //#endif

    a_data = (struct all_data *) malloc(sizeof(struct all_data));

    a_data->socket = client_socket;
    a_data->ip_address = client_address.sin_addr.s_addr;
    a_data->net_data = net_data;

    pthread_create(&new,NULL,handler,(void *) a_data);
    
  }

  pthread_exit(E_OK);
  
};

int create_socket(struct network * net_data) {

  int err;
  struct sockaddr_in server_address;

  net_data->socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (net_data->socket < 0) {
    return E_ERROR_CREATING_SOCKET;
  };

  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(SERVER_PORT);

  err = bind(net_data->socket, (struct sockaddr *) &server_address, sizeof(server_address));
  if (err < 0) {
    return E_ERROR_BINDING_SOCKET;
  };

  err = listen(net_data->socket, MAX_CONNECTIONS);
  if (err < 0) {
    return E_ERROR_LISTENING_SOCKET;
  };  

  return E_OK;

};

/**
 * \brief Inicializa a rede do servidor
 * 
 * \param callback Função a ser acionada quando recebe um comando do cliente
 * \return Estrutura com os dados da rede.
 * \retval NULL Erro
 */
struct network * init_network_server(int (*callback)(int operation, void * data, void * ret_data, int ip_address)) {

  struct network * retorno;
  int creation;
 
  retorno = (struct network *) malloc(sizeof(struct network));

  if (!retorno) {
    return NULL;
  };

  retorno->callback = callback;
  retorno->end = 0;

  creation = create_socket(retorno);
  if (creation) {
    free(retorno);
    return NULL;
  };

  creation = pthread_create(&(retorno->main_thread),NULL,main_function,(void *) retorno);
  if (creation) {
    free(retorno);
    return NULL;
  };

  return retorno;

};
