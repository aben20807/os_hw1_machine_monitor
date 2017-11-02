#ifndef CLIENT_H
#define CLIENT_H

#include "protocol.h"

int create_client();
void connect_to_server(const int sockfd, const char *server_addr,
                       const int port);

#endif
