// Partly taken from https://www.geeksforgeeks.org/socket-programming-cc/

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT_WORKER 8018

void child_proc(int conn){
	char buf[1024] ;
	char * data = 0x0, * orig = 0x0 ;
	int len = 0 ;
	int s ;

	while ( (s = recv(conn, buf, 1023, 0)) > 0 ) {
		buf[s] = 0x0 ;
		if (data == 0x0) {
			data = strdup(buf) ;
			len = s ;
		}
		else {
			data = realloc(data, len + s + 1) ;
			strncpy(data + len, buf, s) ;
			data[len + s] = 0x0 ;
			len += s ;
		}

	}
	printf("worker> student_id received: %s\n", data) ;

	// orig = data ;

	shutdown(conn, SHUT_WR) ;
	if (orig != 0x0)
		free(orig) ;
	pid_t child_pid ;
	int exit_code ;
	char *student_id = data;
	child_pid = fork() ;
	if (child_pid == 0) {
		printf("worker> testcase test begins\n");
		freopen("testcase/1.in", "r", stdin);
		freopen("1.out", "w", stdout);
		execl(student_id, 0);
	}
	else {
		wait(0);
		printf("worker> testcase test ends\n");
		len = 20;
		char *message1 = "testcase test over.\n";
		while (len > 0 && (s = send(conn, message1, len, 0)) > 0) {
			data += s ;
			len -= s ;
		}
	}
}

int
main(int argc, char const *argv[])
{
	int listen_fd, new_socket ;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
  	int port_num;
	char buffer[1024] = {0};

	if(strcmp(argv[1], "-p") == 0){
		port_num = atoi(argv[2]);
		printf("Port %d starts listening...\n", port_num);
	}

	else {
		printf("Please give a right command with an available port number.\n");
		printf("usage : ./worker -p [port_num]\n");
		return 0;
	}

	listen_fd = socket(AF_INET /*IPv4*/, SOCK_STREAM /*TCP*/, 0 /*IP*/) ;
	if (listen_fd == 0)  {
		perror("socket failed : ");
		exit(EXIT_FAILURE);
	}

	memset(&address, '0', sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY /* the localhost*/ ;
	address.sin_port = htons(PORT_WORKER);
	if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed : ");
		exit(EXIT_FAILURE);
	}

	while (1) {
		if (listen(listen_fd, 16 /* the size of waiting queue*/) < 0) {
			perror("listen failed : ");
			exit(EXIT_FAILURE);
		}

		new_socket = accept(listen_fd, (struct sockaddr *) &address, (socklen_t*)&addrlen) ;
		if (new_socket < 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}

		if (fork() > 0) {
			child_proc(new_socket) ;
		}
		else {
			close(new_socket) ;
		}
	}
}
