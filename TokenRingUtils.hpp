//
// Created by Timelock on 11.03.2019.
//

#ifndef TOKEN_RING_TOKENRINGUTILS_H
#define TOKEN_RING_TOKENRINGUTILS_H

#include <iostream>
//Socket windows utils
#include <winsock2.h>

enum token_type {
    TOK_MSG, TOK_INIT
};
enum protocol_type {
    UDP, TCP
};

class token {
public:
    char source[64] = {};
    char dest[64] = {};
    char data[1024] = {};
    token_type type;
    bool is_free;

    explicit token(token_type tok_type) {
        type = tok_type;
        is_free = true;
    }

};


SOCKET init_udp_socket() {
    SOCKET init = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (init == INVALID_SOCKET) {
        std::cout << "Error creating UDP socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    } else return init;
}

SOCKET init_tcp_socket() {
    SOCKET init = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (init == INVALID_SOCKET) {
        std::cout << "Error creating TCP socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return INVALID_SOCKET;
    } else return init;
}

//bind a socket to a port on localhost
void bind_socket(SOCKET &socket, int port_number) {
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(port_number);

    int bind_result = bind(socket, (sockaddr *) &address, sizeof(address));

    if (bind_result == SOCKET_ERROR){
        std::cout << "Error binding the socket" << std::endl;
        socket = INVALID_SOCKET;
        return;
    }

}


#endif //TOKEN_RING_TOKENRINGUTILS_H
