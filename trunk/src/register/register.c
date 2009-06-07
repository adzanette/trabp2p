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

  size = *((int *) data_c);
  data[sizeof(int)+size] = 0;

  return data+sizeof(int);

};

int handle_operations (int operation, void * data, void * ret_data, int ip_address) {

  switch (operation) {

  case 1: {
    return join(ip_address,ret_data);
  };
  case 2: {
    return publish(get_string((char *) data),ret_data);
  };
  case 3: {
    return search(get_string((char *) data),ret_data);
  };
 
  };

  return E_OK;

};

int main(int argv, char ** argc) {

  struct network * net;
  char c;

  net = init_network_server(handle_operations);
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
