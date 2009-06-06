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

struct all_data {

  struct network * net_data;

  int ip_address;

  int socket;
  
};

int handler(void * data) {

  struct all_data * net_data;

  net_data = (struct all_data *) data;

};

/**
 * \brief A thread principal da rede
 * 
 * \param data Os dados da rede
 * \return Valor de erro ou sucesso
 * \retval E_OK Sucesso
 * \retval Outro Erro
 */
int main_function(void * data) {

  struct network * net_data;

  int client_socket;

  struct sockaddr_in client_address;

  pthread_t new;  
  
  struct all_data * new;

  net_data = (struct network *) data;

  while (!(net_data->end)) {

    client_socket = accept(serversock, (struct sockaddr *) &client_address,sizeof(client_address));
    if (client_socket < 0) {
      exit(E_ERROR_ACCEPTING_CONNECTION)
    };

#ifdef DEBUG
    fprintf(F_MESSAGES, "Cliente conectado no IP: %s\n",inet_ntoa(echoclient.sin_addr));
#endif

    new = (struct all_data *) malloc(sizeof(struct all_data));

    new->socket = client_socket;
    new->ip_address = echoclient.sin_addr;
    new->net_data = net_data;

    pthread_create(&new,NULL,handler,(void *) new);
    
  }

  pthread_exit(E_OK);
  
};

int create_socket(struct network * net_data) {

  int err;

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

  err = listen(serversock, MAX_CONNECTIONS);
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
struct network * init_network_server(int (*callback)(int operation, void * data, void * ret_data)) {

  struct network * retorno;
  int creation;
 
  retorno = (struct network *) malloc(struct network);

  if (!retorno) {
    return NULL;
  };

  retorno->callback = callback;

  creation = create_socket(retorno);
  if (creation) {
    free(retorno);
    return NULL;
  };

  creation = pthread_create(&main_thread,NULL,main_function,(void *) retorno);
  if (creation) {
    free(retorno);
    return NULL;
  };

  return retorno;

};
