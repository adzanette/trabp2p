/***************************************************************************
 *   Copyright (C) 2009 by Leonardo de Miranda Borba, Adriano Zanette      *
 *   lmborba@gmail.com,adrzanette@gmail.com                                *
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


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>
#include "../hash_table/hash_table.h"

#define MAXPENDING	5
#define BUFFSIZE	255
#define SIZE_SEND	4096

#define PORT_TCP	5678
#define PORT_UDP	5679
#define PORT_FETCH      5680
#define PORT_FETCH_IN   (atoi(argva[2]))
#define PORT_FETCH_OUT  (atoi(argva[3]))

#define COMMAND_JOIN	1
#define COMMAND_PUBLISH	2
#define COMMAND_SEARCH	3
#define COMMAND_FETCH	4
#define COMMAND_HELLO	5

#define INCORRECT_ANSWER	24
#define CORRECT_ANSWER		16

#define SEARCH_END	0
#define SEARCH_NOT_END	1

#define FETCH_END	0
#define FETCH_NOT_END	1

#define SIZE_OPERATION	1
#define SIZE_BOOLEAN	1
#define SIZE_INT	4

#define TCP	1
#define UDP	2

/**
 * Tipo abstrato das entradas na lista de threads.
 */
typedef struct{
	int id; /**< identificador da thread. */
	pthread_t thread; /**< thread que executará a função. */
	//pthread_cond_t condition; /**< referencia a uma entrada na tabela. */
}data;

/**
 * Tipo abstrato de uma lista encadeada de threads.
 */
typedef struct T_LIST{
	data head;  /**< referencia a uma entrada na lista. */
	struct T_LIST *next; /**< referencia para o próximo elemento da lista. */
}t_list;


/** \brief Cria um socket TCP ou UDP.
 *
 * @param ip Indica o ip ao qual deseja conectar o socket
 * @param port Indica a porta a se conectar
 * @param port Protocolo
 * @return 1 se a resposta do servidor for afirmativa e 0 caso contrário.
 */
int create_socket(int ip, int port, int protocol);

/** \brief Envia requisição ao registro P2P para fazer parte da rede.
 *
 * @return 1 se a resposta do servidor for afirmativa e 0 caso contrário.
 */
int join();

/** \brief Envia um arquivo ao servidor para ser publicado.
 *
 * @param file string que contém o nome do arquivo a ser publicado
 * @return 1 se a resposta do servidor for afirmativa e 0 caso contrário.
 */
int publish(char* file);

/** \brief Envia o nome de um arquivo a ser buscado no servidor e o servidor devolve os ip's que tem esse arquivo.
 *
 * Obs: armazena os ip's dos seeds na tabela hash seeds
 * @param file string que contém o nome do arquivo a ser buscado
 *
 */
void search(char* file);

/** \brief Faz o download de um arquivo que se encontra em um outro peer 
 *
 * @param file arquivo a ser baixado
 * @param thread_id identificador da thread que está executando o download
 */
void * fetch(void* file_void);

/** \brief Envia um arquivo requisitado por um seed.
 *
 * @param sock sock no qual deve enviar o arquivo
 */
void * send_file(void* socket);

/** \brief Fica escutando em uma porta, por requisições de outros peers.
 *
 * Obs: Chama a função send_file para enviar o arquivo
 */
void * reply_fetch();

/** \brief Manda uma mensagem HELLO para o servidor a cada 60s por um socket udp.
 *
 * @param ip ip ao qual deve se conectar para mandar as mensagens
 */
void * hello(void* ip_void);
