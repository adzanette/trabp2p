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

int sock_tcp;
int sock_udp;
struct sockaddr_in echoserver_udp;

pthread_attr_t atributes;
pthread_cond_t condition;

pthread_t thread_hello;
pthread_t thread_send_fetch;

symbol_t seeds;

t_list* threads;

/** \todo Apagar variáveis de teste */
int argca;
char ** argva;

void Die(char *mess) { perror(mess); exit(1); }

int create_socket(int ip, int port, int protocol){

  int sock;	
  struct sockaddr_in echoserver;

  if(protocol == TCP){

    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
      Die("Failed to create socket TCP!\n");
    }

    memset(&echoserver, 0, sizeof(echoserver));       
    echoserver.sin_family = AF_INET;                  
    echoserver.sin_addr.s_addr = ip;  
    echoserver.sin_port = htons(port);   

    if (connect(sock,(struct sockaddr *) &echoserver,sizeof(echoserver)) < 0) {
      return 0;
    }
    return sock;
  }
  return -1;
}

int join(){

  char echo[BUFFSIZE]; 
  char buffer[BUFFSIZE];
  int pos;

  pos = 0;

  echo[pos++] = COMMAND_JOIN;

  if (send(sock_tcp, echo, pos, 0) != pos) {
    Die("Mismatch in number of sent bytes!\n");
  }

  printf("Connection ");	
  recv(sock_tcp, buffer, SIZE_OPERATION, 0);

  if ((int)buffer[0] == COMMAND_JOIN + CORRECT_ANSWER){
    printf("OK!\n");
    return 1;
  }else{
    printf("Fail!\n");
    return 0;
  }

}

int publish(char* file){

  char echo[BUFFSIZE]; 
  char buffer[BUFFSIZE];
  int filename_len;
  int pos;

  filename_len = strlen(file);

  pos = 0;
  echo[pos++] = COMMAND_PUBLISH;
  memcpy(echo+pos,(char *) &(filename_len),sizeof(int));
  pos += sizeof(int);
  memcpy(echo+pos, file, filename_len);
  pos += filename_len;
	
  if (send(sock_tcp, echo, pos, 0) != pos) {
    Die("Mismatch in number of sent bytes!\n");
  }

  printf("File %s ", file);

  recv(sock_tcp, buffer, SIZE_OPERATION, 0);
	
  if (buffer[0] == COMMAND_PUBLISH + CORRECT_ANSWER){
    printf("Published!\n");
    return 1;
  }else{
    printf("not Published!\n");
    return 0;
  }

}

void search(char* file){

  char echo[BUFFSIZE]; 
  char buffer[BUFFSIZE];
  int filename_len;
  int bytes; 
  int i;
  int ip;
  int pos;
  int bool;
  entry_t* aux;
  int size;

  filename_len = strlen(file);

  pos = 0;
  echo[pos++] = COMMAND_SEARCH;
  memcpy(echo+pos,(char *) &(filename_len),sizeof(int));
  pos += sizeof(int);
  memcpy(echo+pos, file, filename_len);
  pos += filename_len;

  if (send(sock_tcp, echo, pos, 0) != pos) {
    Die("Mismatch in number of sent bytes!\n");
  }

  i = 1;
  while (1) {
		
    recv(sock_tcp, buffer, SIZE_OPERATION + SIZE_BOOLEAN, 0);
		
    if( buffer[0] == COMMAND_SEARCH + CORRECT_ANSWER ){

      bool = buffer[1];
     
      
      recv(sock_tcp, buffer, SIZE_INT, 0);

      ip = * ((int *) buffer);
      printf("Seed %d: ", i);
      printf("%d.%d.%d.%d\n", (int) buffer[0],(int) buffer[1],(int) buffer[2],(int) buffer[3]);
      i++;

      aux = (entry_t *) malloc(sizeof(entry_t));
      aux->name = file;
      aux->ip = ip;

      insert(&seeds, aux);

      if(bool == SEARCH_END) break;
    } else if ( buffer[0] == (COMMAND_SEARCH + INCORRECT_ANSWER) ){
	printf("No seeds for you!\n");
	break;
    }
  }
}

void * fetch(void* file_void){

  char echo[BUFFSIZE]; 
  char buffer[BUFFSIZE];
  int pos;
  int received;
  linkedlist* ll_seeds;
  linkedlist* ll_aux;
  int sock;
  struct sockaddr_in echoserver;
  FILE* out;
  int filename_len;
  int file_size;
  char file_content[SIZE_SEND];	
  char* file;

  file = (char *) file_void;

  ll_seeds = lookup(&seeds, file);

  ll_aux = ll_seeds;

  while(1){
		
    sock = create_socket(ll_seeds->head->ip, PORT_FETCH_IN, TCP);

    if (!sock) {

      if(ll_aux->next == NULL){
	ll_aux = ll_seeds;
      }else ll_aux = ll_aux->next;
    }else{
      filename_len = strlen(file);
      pos = 0;
      echo[pos++] = COMMAND_FETCH;
      memcpy(echo+pos,(char *) &(filename_len),sizeof(int));
      pos += sizeof(int);
      memcpy(echo+pos, file, filename_len);
      pos += filename_len;
			
      if(send(sock, echo, pos, 0) != pos){
	Die("Mismatch in number of sent bytes!\n");
      }

      recv(sock, buffer, SIZE_OPERATION + SIZE_BOOLEAN, 0);

      if (buffer[0] == COMMAND_FETCH + CORRECT_ANSWER){
	if(buffer[1] == __FALSE){
	  if(ll_aux->next == NULL){
	    ll_aux = ll_seeds;
	  }else ll_aux = ll_aux->next;
	}else break;
      }
    }
  }
	
	
  printf("Downloading %s.\n", file);

  out = fopen(file, "w");
  while(1){
    recv(sock, buffer, SIZE_OPERATION + SIZE_BOOLEAN, 0);
		
    if (buffer[0] == COMMAND_FETCH + CORRECT_ANSWER){
      if(buffer[1] == FETCH_END) break;	

      recv(sock, buffer, SIZE_INT, 0);

      pos = * ((int *) buffer);			

      received = recv(sock, file_content, pos, 0);	
      file_content[received] = '\0';

      fprintf(out, file_content);
    }else{

      printf("Download Fail!\n");
      break; 
    }
  }
  fclose(out);

  printf("File %s downloaded!\n", file);

  publish(file);
}


void * send_file(void * socket){

  int sock;
  char buffer[BUFFSIZE];
  int received = -1;
  char* file;
  int bool;
  FILE* file_send;
  char echo[BUFFSIZE];
  int filename_size; 
  char read_file[SIZE_SEND];
  int file_size;
  int pos;
  char send_bytes[SIZE_SEND];
  int bytes;

  sock = (int)socket;

  recv(sock, buffer, SIZE_OPERATION, 0);
	
  if (buffer[0] == COMMAND_FETCH){
    recv(sock, buffer, SIZE_INT, 0);

    filename_size = * ((int *) buffer);
    received = recv(sock, buffer, filename_size, 0);
    buffer[received] = '\0';
    file = (char *) malloc((filename_size+1)*sizeof(char));
    strcpy(file, buffer);

    file_send = fopen(file, "r");

    if (file_send == NULL){

      pos = 0;
      echo[pos++] = COMMAND_FETCH + CORRECT_ANSWER;
      echo[pos++] = __FALSE;
  		
      if (send(sock, echo, pos, 0) != pos) {
	Die("Failed to send bytes to client");
      }
    }else{

      pos = 0;
      echo[pos++] = COMMAND_FETCH + CORRECT_ANSWER;
      echo[pos++] = __TRUE;
  		
      if (send(sock, echo, pos, 0) != pos) {
	Die("Failed to send bytes to client");
      }

      while(!feof(file_send)){

	bytes = fread(read_file, 1, SIZE_SEND - 6, file_send);	

	pos = 0;
	send_bytes[pos++] = COMMAND_FETCH + CORRECT_ANSWER;
	send_bytes[pos++] = FETCH_NOT_END;
	memcpy(send_bytes+pos,(char *) &(bytes),sizeof(int));
	pos += sizeof(int);
	memcpy(send_bytes+pos, read_file, bytes);
	pos += bytes;
	
	if (send(sock, send_bytes, pos, 0) != pos) {
	  Die("Failed to send bytes to client");
	}
      }

      pos = 0;
      send_bytes[pos++] = COMMAND_FETCH + CORRECT_ANSWER;
      send_bytes[pos++] = FETCH_END;
			
      if (send(sock, send_bytes, pos, 0) != pos) {
	Die("Failed to send bytes to client");
      }

      fclose(file_send);
    }
  }
  close(sock);

}

void * reply_fetch(){

  int serversock, clientsock;
  struct sockaddr_in echoserver, echoclient;
  pthread_t* thread;

  if ((serversock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    Die("Failed to create socket");
  }

  memset(&echoserver, 0, sizeof(echoserver));       
  echoserver.sin_family = AF_INET;                  
  echoserver.sin_addr.s_addr = htonl(INADDR_ANY);   
  echoserver.sin_port = htons(PORT_FETCH_OUT);       

  if (bind(serversock, (struct sockaddr *) &echoserver,sizeof(echoserver)) < 0) {
    Die("Failed to bind the server socket");
  }


  if (listen(serversock, MAXPENDING) < 0) {
    Die("Failed to listen on server socket");
  }

  while (1) {
    unsigned int clientlen = sizeof(echoclient);
		
    if ((clientsock = accept(serversock, (struct sockaddr *) &echoclient,&clientlen)) < 0) {
      Die("Failed to accept client connection");
    }
    printf( "Client connected: %s\n",inet_ntoa(echoclient.sin_addr));

    thread = (pthread_t *) malloc(sizeof(pthread_t));
    pthread_create(thread, &atributes, send_file, (void *)clientsock);
  }
}

void * hello(void* ip_void){

  //  clock_t current, last;
  time_t current,last;
  int pos;
  char echo[BUFFSIZE];
  char* ip;

  time(&current);

  ip = (char *)ip_void;

  for(;;){
    time(&last);
    //printf("%d,%d,%d\n",last-current,last,current);
    if(difftime(last,current) > 10){
      pos = 0;
      echo[pos++] = COMMAND_HELLO;
			
      if (sendto(sock_udp, echo, pos, 0,(struct sockaddr *) &echoserver_udp,sizeof(echoserver_udp)) != 1) {
	Die("Mismatch in number of sent bytes!\n");
      }else{
	printf("HELLO sent!\n");
      }
      current = last;
    }
    sleep(1);
  }
}

void publish_all(DIR* path){
	struct dirent *dir;
	DIR* path_aux;

	while(dir = readdir(path)){
		if (strcmp(dir->d_name, ".") && strcmp(dir->d_name, "..")){
			if (!(path_aux = opendir(dir->d_name))){ 
				publish(dir->d_name); 
	  	}else{
				publish_all(path_aux);
			}
		}  
  }
	close(path);
}

int main(int argc, char** argv){
	
  char op[BUFFSIZE];
  char name[BUFFSIZE];
  char ip[BUFFSIZE];
  char file[BUFFSIZE];
  int id;
  t_list* thread_aux;
   DIR *path;

  /** \todo Apagar seguintes comandos */
  argca = argc;
  argva = argv;

  threads = NULL;

  init_table(&seeds);
	
  id = 0;

	if (!(path = opendir("SharedP2P"))){ 
		mkdir("SharedP2P", 744);
	}else{
		publish_all(path);
	}
  closedir (path);

  sock_tcp = create_socket(inet_addr(argv[1]), PORT_TCP, TCP);	

	join();

  if ((sock_udp = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    Die("Failed to create socket UDP!\n");
  }
  memset(&echoserver_udp, 0, sizeof(echoserver_udp));  
  echoserver_udp.sin_family = AF_INET;             
  echoserver_udp.sin_addr.s_addr = inet_addr(argv[1]);  
  echoserver_udp.sin_port = htons(PORT_UDP); 


  pthread_attr_init(&atributes);
  pthread_attr_setdetachstate(&atributes,PTHREAD_CREATE_JOINABLE);
  pthread_cond_init(&(condition),NULL);

  pthread_create(&thread_hello,&atributes,hello, (void *)argv[1]); // envia hello a cada 60s 	
  pthread_create(&thread_send_fetch,&atributes,reply_fetch, NULL); // envia arquivos requisitados por outros peers

  do{	
    scanf("%s", op);
    if (!strcmp(op, "publish")){
      scanf("%s", file);
      publish(file);
    }else if (!strcmp(op, "search")){
      scanf("%s", file);
      search(file);
    }else if (!strcmp(op, "fetch")){
      scanf("%s", file);
      thread_aux = (t_list *) malloc(sizeof(t_list));
      thread_aux->head.id = id;
      id++;
      pthread_create(&(thread_aux->head.thread), &atributes, fetch, (void *)file);
      if (threads == NULL){
				threads = thread_aux;
				threads->next = NULL;
      }else{
				thread_aux->next = threads;
				threads = thread_aux;
      }
    }else if (strcmp(op, "exit")){
      printf("Invalid Command!\n");
    }
  } while (strcmp(op, "exit"));

  close(sock_tcp);
  close(sock_udp);

  return 0;
}


/*teste
  int main(int argc, char** argv){
	
  char* op;
  char* name;
  char* ip;
  char* file;
  int id;
  t_list* thread_aux;
  entry_t* aux;
	
  threads = NULL;
  init_table(&seeds);
  aux = (entry_t *) malloc(sizeof(entry_t));
  aux->name = "testano.txt";
  aux->ip = inet_addr("127.0.0.1");
  insert(&seeds, aux);
  id = 0;

  pthread_attr_init(&atributes);
  pthread_attr_setdetachstate(&atributes,PTHREAD_CREATE_JOINABLE);
  pthread_cond_init(&(condition),NULL);
  pthread_create(&thread_send_fetch,&atributes,reply_fetch, NULL);
  file = "testano.txt";
  thread_aux = (t_list *) malloc(sizeof(t_list));
  thread_aux->head.id = id;
  id++;
  pthread_create(&(thread_aux->head.thread), &atributes, fetch, (void *)file);

  while(1);

  close(sock_tcp);
  close(sock_udp);
		
  return 0;
  }*/
