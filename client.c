#include "client.h"

int main(int argc, char **argv)
{
	int sockfd = create_client();
	connect_to_server(sockfd, "127.0.0.1", 59487);
	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, send_command,
	                   (void*)&sockfd) < 0) {
		perror("could not create thread");
	}
	printf("Command-handler assigned\n");
	fflush(stdout);
	while (1) {}
	printf("close Socket\n");
	close(sockfd);
	return 0;
}

int create_client()
{
	int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("Fail to create a socket.");
	} else {
		printf("Client started...\n");
	}
	fflush(stdout);
	return sockfd;
}

void connect_to_server(const int sockfd, const char *server_addr,
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
	}
	pthread_t thread_id;
	if (pthread_create(&thread_id, NULL, connection_handler,
	                   (void*)&sockfd) < 0) {
		perror("could not create thread");
	}
	printf("Handler assigned\n");
	fflush(stdout);
}

void *send_command(void *server_sockfd)
{
	int sockfd = *(int*)server_sockfd;
	char message[] = {"Hi there"};
	while (1) {
		char c = ' ';
		int pid = 0;
		printf("which? ");
		if (!scanf(" %c", &c)) {
			break;
		} else if (c == 'k') {
			close(sockfd);
			break;
		} else if (c > 'k') {
			printf("ERROR: COMMAND_NOT_FOUND\n");
			continue;
		}
		if (c != 'a') {
			printf("pid? ");
			scanf("%d", &pid);
		}
		sprintf(message, "%c%d", c, pid);
		send(sockfd, message, sizeof(message), 0);
	}
}

void *connection_handler(void *server_sockfd)
{
	int sockfd = *(int*)server_sockfd;
	int read_size;
	char input_buffer[BUFSIZ] = {};
	fflush(stdout);
	while ((read_size = recv(sockfd, input_buffer, sizeof(input_buffer), 0)) > 0 ) {
		input_buffer[read_size] = '\0';
		printf("Get: %s\n", input_buffer);
		fflush(stdout);
		memset(input_buffer, 0, sizeof(input_buffer));
	}
	if (read_size == 0) {
		printf("Client disconnected");
		fflush(stdout);
	} else if (read_size == -1) {
		perror("recv failed");
	}
	return 0;
}
