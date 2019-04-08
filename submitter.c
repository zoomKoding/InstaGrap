#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>


#define LENGTH 512

int
main(int argc, char const *argv[])
{
	struct sockaddr_in serv_addr;
	int sock_fd ;
	int s, len ;
	char buffer[1024] = {0};
	char * data ;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0) ;
	if (sock_fd <= 0) {
		perror("socket failed : ") ;
		exit(EXIT_FAILURE) ;
	}

	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(8090);
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		perror("inet_pton failed : ") ;
		exit(EXIT_FAILURE) ;
	}

	if (connect(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("connect failed : ") ;
		exit(EXIT_FAILURE) ;
	}

	char* fs_name = "./hello.c";
	char sdbuf[LENGTH];
	printf("[Client] Sending %s to the Server... ", fs_name);
	FILE *fs = fopen(fs_name, "r");
	if(fs == NULL)
	{
		printf("ERROR: File %s not found.\n", fs_name);
		exit(1);
	}

	bzero(sdbuf, LENGTH);
	int fs_block_sz;
	while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs)) > 0)
	{
	    if(send(sock_fd, sdbuf, fs_block_sz, 0) < 0)
	    {
	        fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fs_name, errno);
	        break;
	    }
	    bzero(sdbuf, LENGTH);
	}
	printf("Ok File %s from Client was Sent!\n", fs_name);

	shutdown(sock_fd, SHUT_WR) ;

	// char buf[1024] ;
	// data = 0x0 ;
	// len = 0 ;
	// while ( (s = recv(sock_fd, buf, 1023, 0)) > 0 ) {
	// 	buf[s] = 0x0 ;
	// 	if (data == 0x0) {
	// 		data = strdup(buf) ;
	// 		len = s ;
	// 	}
	// 	else {
	// 		data = realloc(data, len + s + 1) ;
	// 		strncpy(data + len, buf, s) ;
	// 		data[len + s] = 0x0 ;
	// 		len += s ;
	// 	}

	// }
	// printf("client> %s\n", data);

}
