#include "client.h"

int main(int argc, char **argv)
{
	// write someting here...
	//socket的建立
	int sockfd = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd == -1) {
		printf("Fail to create a socket.");
	} else {
		printf("Client started...\n");
	}
	fflush(stdout);
	//socket的連線

	struct sockaddr_in info;
	memset(&info, 0, sizeof(info));
	info.sin_family = PF_INET;

	//localhost test
	info.sin_addr.s_addr = inet_addr("127.0.0.1");
	info.sin_port = htons(59487);

	int err = connect(sockfd, (struct sockaddr *)&info, sizeof(info));
	if (err == -1) {
		printf("Connection error");
		fflush(stdout);
	}

	//Send a message to server
	char message[] = {"Hi there"};
	char receive_message[100] = {};
	int n = 5;
	while (n--) {
		send(sockfd, message, sizeof(message), 0);
	}
	recv(sockfd, receive_message, sizeof(receive_message), 0);

	printf("%s", receive_message);
	printf("close Socket\n");
	close(sockfd);
	return 0;
}
