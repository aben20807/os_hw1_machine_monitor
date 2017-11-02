#include "client.h"

int main(int argc, char **argv)
{
	int sockfd = create_client();
	connect_to_server(sockfd, "127.0.0.1", 59487);
	char message[] = {"Hi there"};
	char receive_message[100] = {};
	send(sockfd, message, sizeof(message), 0);
	recv(sockfd, receive_message, sizeof(receive_message), 0);
	printf("%s", receive_message);
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
}
