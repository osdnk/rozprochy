//
// Created by Timelock on 11.03.2019.
//

#ifndef TOKEN_RING_TOKENRINGUTILS_H
#define TOKEN_RING_TOKENRINGUTILS_H

#include <iostream>
//Socket windows utils
#include <sys/socket.h>
#include <vector>

#define SOCKET int
#define INVALID_SOCKET  ((SOCKET)(~0))
#define SOCKET_ERROR (-1)

SOCKET init_udp_socket();

SOCKET init_tcp_socket();

void bind_socket(SOCKET &socket, int port_number);
std::vector<std::string> split(std::string str,std::string sep);

#endif //TOKEN_RING_TOKENRINGUTILS_H
