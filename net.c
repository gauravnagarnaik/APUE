#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <sys/stat.h>

#define BUFSIZE (4 * 1024)
#define MIN_PORT 1
#define MAX_PORT 65535

int flag_c;
int flag_d;
int flag_i;
int flag_p;

char* dir;
char* address;
char*file;
int port;

int sock;
int msgsock;
int rval;
struct sockaddr_in server;
struct sockaddr_in client;
struct timeval to;
char buf[1024];
char *client_addr;
fd_set ready;
socklen_t length;

void 
net(){
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("opening stream socket");
    exit(1);
  }
  printf("socket success!,sockfd = %d\n",sock);

  server.sin_family = AF_INET;

  if (flag_i == 1)
  {
    server.sin_addr.s_addr = inet_addr(address);
  }else{
    server.sin_addr.s_addr = INADDR_ANY;
  }

  if (flag_p == 1)
  {
    server.sin_port = htons(port);
  }else{
    server.sin_port = 8080; 
  }

  if (bind(sock, (struct sockaddr *)&server, sizeof(server))) {
    perror("binding stream socket");
    exit(1);
  }

  printf("bind success!\n");

  length = sizeof(server);
  if (getsockname(sock, (struct sockaddr *)&server, &length)) {
    perror("Socket name: ");
    exit(1);
  }

  printf("Socket has port #%d\n", ntohs(server.sin_port));

  listen(sock, 5);
  do {
    FD_ZERO(&ready);
    FD_SET(sock, &ready);
    to.tv_sec = 5;
    to.tv_usec = 0;
    if (select(sock + 1, &ready, 0, 0, &to) < 0) {
      perror("select");
      continue;
    }
    if (FD_ISSET(sock, &ready)) {
      length = sizeof(client);
      msgsock = accept(sock, (struct sockaddr *)&client, &length);
      client_addr = inet_ntoa(client.sin_addr);
      printf("Client connection from %s!\n", client_addr);
      if (msgsock == -1)
        perror("accept");
      else do {
        bzero(buf, sizeof(buf));
        if ((rval = read(msgsock, buf, BUFSIZE)) < 0)
          perror("reading stream message");
        else if (rval == 0)
          printf("Ending connection from %s.\n", client_addr);
        else
          printf("Client (%s) sent: %s", client_addr, buf);
      } while (rval > 0);
      close(msgsock);
    }
  } while (1);
}
