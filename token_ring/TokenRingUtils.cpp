#include <iostream>
//Socket windows utils
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>


#include "TokenRingUtils.hpp"

SOCKET init_udp_socket() {
    SOCKET init = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (init == INVALID_SOCKET) {
        std::cout << "Error creating UDP socket: " << "Blank" << std::endl;
        return INVALID_SOCKET;
    } else return init;
}

SOCKET init_tcp_socket() {
    SOCKET init = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (init == INVALID_SOCKET) {
        std::cout << "Error creating TCP socket: " <<"Blank"<< std::endl;
        return INVALID_SOCKET;
    } else return init;
}

void bind_socket(SOCKET &socket, int port_number) {
    sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(port_number);

    int bind_result = bind(socket, (sockaddr *) &address, sizeof(address));

    if (bind_result == SOCKET_ERROR) {
        std::cout << "Error binding the socket with port number: " << port_number << std::endl;
        socket = INVALID_SOCKET;
        return;
    }

}


std::vector<std::string> split(std::string str,std::string sep){
    char* cstr=const_cast<char*>(str.c_str());
    char* current;
    std::vector<std::string> arr;
    current=strtok(cstr,sep.c_str());
    while(current!= nullptr){
        arr.push_back(current);
        current=strtok(nullptr,sep.c_str());
    }
    return arr;
}


