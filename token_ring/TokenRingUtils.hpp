//
// Created by Timelock on 11.03.2019.
//

#ifndef TOKEN_RING_TOKENRINGUTILS_H
#define TOKEN_RING_TOKENRINGUTILS_H

#include <iostream>
//Socket windows utils
#include <winsock2.h>


SOCKET init_udp_socket();

SOCKET init_tcp_socket();

void bind_socket(SOCKET &socket, int port_number);

#endif //TOKEN_RING_TOKENRINGUTILS_H
