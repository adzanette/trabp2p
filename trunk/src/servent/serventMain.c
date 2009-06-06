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

#include "servent.h"
#include <stdio.h>
#include <sys/socket.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "symbol_table.h"
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#define BUFFSIZE 32
#define PORT 5678


void Die(char *mess) { perror(mess); exit(1); }

pthread_attr_t atributes;
pthread_cond_t condition;

pthread_t thread;


int join(){

	int echolen;
	char echo[255]; 
	char buffer[BUFFSIZE];
	int bytes; 

	echo[0] = (char)1;
	echo[1] = '\0';
	echolen(echo);

	if (send(sock, echo, echolen, 0) != echolen) {
		Die("Mismatch in number of sent bytes");
	}

	printf("Connection ");

	bytes = 0;
	bytes = recv(sock, buffer, BUFFSIZE-1, 0))
	buffer[bytes] = '\0'; 

	if ((int)buffer[0] == 9){
		printf("OK!");
	}else{
		printf("Fail!");
	}
}

int publish(char* file){
	/*SEND FILE + IP*/
}

int search(char* file){

	int echolen;
	char echo[255]; 
	char buffer[BUFFSIZE];
	int bytes; 

	echo[0] = (char)1;
	echo[1] = '\0';
	strcat(echo, file);
	echolen(echo);

	if (send(sock, echo, echolen, 0) != echolen) {
		Die("Mismatch in number of sent bytes");
	}

	/* Receive the word back from the server */
	fprintf(stdout, "Received: ");
	while (received < echolen) {
		int bytes = 0;
		if ((bytes = recv(sock, buffer, BUFFSIZE-1, 0)) < 1) {
			Die("Failed to receive bytes from server");
		}
  		received += bytes;
  		buffer[bytes] = '\0';        /* Assure null terminated string */
  		fprintf(stdout, buffer);
	}
}

int fetch(char* file){
	/*REQUEST DOWNLOAD*/
}

int send(char* file, char* ip){
	/*SEND FILE*/
}

int reply_fetch(){
	/*função de uma thread que fica escutando por fetchs e envia os arquivos quando solicitado */
}

int hello(){

	clock_t current, last;
	current = clock();
	int echolen;
	char echo[255];

	for(;;){
		last = clock();
		if(difftime(last, current) > 60){
			echo[0] = (char)5;
			echo[1] = '\0';
			echolen = strlen(echo);
  			if (send(sock, echo, echolen, 0) != echolen) {
  				Die("Mismatch in number of sent bytes");
  			}
			current = last;
		}
	}
}



int main(int argv, char ** argc) {
	
	char* op;
	char* name;
	char* ip;
	char* file;
	char* my_ip;
	nt sock;
	struct sockaddr_in echoserver;
	char buffer[BUFFSIZE];	

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		Die("Failed to create socket");
	}

	/* Construct the server sockaddr_in structure */
	memset(&echoserver, 0, sizeof(echoserver));       /* Clear struct */
	echoserver.sin_family = AF_INET;                  /* Internet/IP */
	echoserver.sin_addr.s_addr = inet_addr(argv[1]);  /* IP address */
	echoserver.sin_port = htons(PORT);       /* server port */
	/* Establish connection */
	if (connect(sock,(struct sockaddr *) &echoserver,sizeof(echoserver)) < 0) {
		Die("Failed to connect with server");
	}
	
	pthread_mutex_init(&servents_mutex,NULL);
	pthread_mutex_init(&stable_mutex,NULL);

	pthread_attr_init(&atributes);
	pthread_attr_setdetachstate(&atributes,PTHREAD_CREATE_JOINABLE);
	pthread_cond_init(&(condition),NULL);
	pthread_create(thread,&atributes,hello); /* envia hello a cada 60s */	

	do{	
		scanf("%s", &op);
		if (!strcomp(op, "join")){
			scanf("%s", &ip);
			join(ip);
		}else if (!strcomp(op, "publish")){
			scanf("%s", &file);
			publish(file, my_ip);
		}else if (!strcomp(op, "search")){
			scanf("%s", &search);
			search(file, my_ip)
		}else if (!strcmp(op, "fetch"))
			scanf("%s", &file);
			fetch(file);
		}else if (strcomp(op, exit)){
			printf("Invalid Command!\n");
		}
	}while(!strcomp(op, "exit"));

	close(sock);
	return 0;
}