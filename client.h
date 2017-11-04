#ifndef CLIENT_H
#define CLIENT_H

#include "protocol.h"

int create_client();
void connect_to_server(const int sockfd, const char *server_addr,
                       const int port);
void send_command(int sockfd);
void print_receive_info(int sockfd);
void print_interface();

#endif
