#include <iostream>
#include "TCP_client.hpp"

int main() {

    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != NO_ERROR)
        std::cout << "Initialization error.\n" << std::endl;
    std::string client_id = "Klient 1";
    TCP_client* new_client = new TCP_client(3000, client_id, TCP, true);
}