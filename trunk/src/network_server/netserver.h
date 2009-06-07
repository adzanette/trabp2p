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

#ifndef __NETSERVER_H
#define __NETSERVER_H

#include <pthread.h>
#include <errors.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 12021
#define MAX_CONNECTIONS 30
#define F_MESSAGES stdout

#define BUF_SIZE 512

#define SIZE_OPERATION 1
#define SIZE_SEARCH_BOOLEAN 1
#define SIZE_FILE_NAME_SIZE 4

/** 
 * \brief Estrutura que usará a rede diretamente
 */
struct network {

  int socket; /**< O socket propriamente dito */

  int (*callback)(int operation, void * data, void * ret_data,int ip_address, void *all_data); 
  /**< Callback a ser chamada quando recebe uma operação a realizar */

  struct sockaddr_in server_address;

  pthread_t main_thread; /**< A thread principal da rede */

  int end;

  void * data;

};

struct network * init_network_server(int (*callback)(int operation, void * data, void * ret_data, int ip_address, void * all_data), void * data);

#endif
