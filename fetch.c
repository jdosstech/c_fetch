#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

// Function: fetchURL
int fetchURL(const char *url, char *file)
{
  FILE *fp;
  char *cmd = malloc(sizeof(char) * (strlen(url) + strlen(file) + 20));
  sprintf(cmd, "wget -O %s %s", file, url);
  fp = popen(cmd, "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    return -1;
  }
  pclose(fp);
  free(cmd);
  return 0;
}

int socket_connect(char *host, in_port_t port){
	struct hostent *hp;
	struct sockaddr_in addr;
	int on = 1, sock;     

	if((hp = gethostbyname(host)) == NULL){
		herror("gethostbyname");
		exit(1);
	}
	bcopy(hp->h_addr, &addr.sin_addr, hp->h_length);
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char *)&on, sizeof(int));

	if(sock == -1){
		perror("setsockopt");
		exit(1);
	}
	
	if(connect(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1){
		perror("connect");
		exit(1);

	}
	return sock;
}
 
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]){
	int fd;
  FILE *fp;
	char buffer[BUFFER_SIZE];

  if( argc < 3 )
  {
    printf("Usage: %s <url> <file> | %s <hostname> <port> <file>\n", argv[0], argv[0]);
    return 1;
  }
  else if( argc == 3 )
  {
    fetchURL(argv[1], argv[2]);
    return 0;
  }
       
	fd = socket_connect(argv[1], atoi(argv[2])); 
	write(fd, "GET /\r\n", strlen("GET /\r\n")); // write(fd, char[]*, len);  
	bzero(buffer, BUFFER_SIZE);
	
  fp = fopen(argv[3], "w");
	while(read(fd, buffer, BUFFER_SIZE - 1) != 0){
		fprintf(fp, "%s", buffer);
		bzero(buffer, BUFFER_SIZE);
	}

	shutdown(fd, SHUT_RDWR); 
	close(fd); 

	return 0;
}


