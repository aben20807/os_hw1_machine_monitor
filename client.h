#ifndef CLIENT_H
#define CLIENT_H

#include "protocol.h"

static inline int create_client();
static inline void connect_to_server(const int sockfd, const char *server_addr,
                                     const int port);
static inline void send_command(const int sockfd);
static inline void print_receive_info(const int sockfd);
static inline void print_interface();

#endif
