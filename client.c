#include "client.h"

int main(int argc, char **argv)
{
	int sockfd = create_client();
	connect_to_server(sockfd, "127.0.0.1", 59487);
	send_command(sockfd);
	printf("\nClose socket\n");
	close(sockfd);
	return 0;
}

static inline int create_client()
{
	int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("Fail to create a socket.\n");
	} else {
		// printf("Client started...\n");
	}
	fflush(stdout);
	return sockfd;
}

static inline void connect_to_server(const int sockfd, const char *server_addr,
                                     const int port)
{
	struct sockaddr_in info;
	memset(&info, 0, sizeof(info));
	info.sin_family = PF_INET;
	info.sin_addr.s_addr = inet_addr(server_addr);
	info.sin_port = htons(port);
	int err = connect(sockfd, (struct sockaddr *)&info, sizeof(info));
	if (err == -1) {
		printf("Connection error\n");
		fflush(stdout);
		exit(EXIT_FAILURE);
	}
}

static inline void send_command(const int sockfd)
{
	while (true) {
		print_interface();
		char command = ' ';
		int pid = 0;
		printf("which? ");
		if (!scanf(" %c", &command)) {
			break;
		} else if (command == 'k') {
			close(sockfd);
			break;
		} else if (command > 'k' || command < 'a') {
			printf("ERROR: COMMAND_NOT_FOUND\n");
			continue;
		}
		if (command != 'a') {
			printf("pid? ");
			if (!scanf("%d", &pid)) {
				break;
			}
			if (isdigit(pid)) {
				printf("ERROR: input pid\n");
				continue;
			}
		}
		struct monitor_protocol package;
		package.command = command;
		package.pid = pid;
		send(sockfd, &package, sizeof(package), 0);
		print_receive_info(sockfd);
	}
}

static inline void print_receive_info(const int sockfd)
{
	int read_size;
	struct monitor_protocol package;
	fflush(stdout);
	if ((read_size = recv(sockfd, &package, sizeof(package), 0)) > 0 ) {
		printf("\n%s %s\n\n", package.description, package.info);
		fflush(stdout);
	}
	if (read_size == 0) {
		printf("Client disconnected\n");
		fflush(stdout);
	} else if (read_size == -1) {
		perror("recv failed");
	}
}

static inline void print_interface()
{
	printf("%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n%s\n",
	       "================================================",
	       "(a)list all process ids",
	       "(b)thread's IDs",
	       "(c)child's PIDs",
	       "(d)process name",
	       "(e)state of process(D,R,S,T,t,W,X,Z)",
	       "(f)command line of executing process(cmdline)",
	       "(g)parent's PID",
	       "(h)all ancestors of PIDs",
	       "(i)virtual memory size(VmSize)",
	       "(j)physical memory size(VmRSS)",
	       "(k)exit"
	      );
}
