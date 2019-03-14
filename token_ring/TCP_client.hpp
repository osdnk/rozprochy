//
// Created by Timelock on 11.03.2019.
//

#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP

#include "TokenRingUtils.hpp"
#include "Token.hpp"
#include <iostream>
#include <list>

class TCP_client {
private:
    SOCKET socket_in;
    SOCKET socket_out;
    sockaddr_in neighbor_address;
    int my_listening_port;
    bool last_client;
    std::string id;
    std::list<std::string   > all_peers;


    token *token_buffer;

    void initialize_sockets(int port_number, int neighbor_port);
    void hello_protocol(bool first_client);
    void send_token();
    SOCKET accept_connection();
    void set_neighbor_port(int neighbor_port);
    void process_new_client();
    void process_not_my_msg();
    void process_my_msg();
public:

    // if neighbor_port == port_number -> this means that essentially we are the first client
    TCP_client(int port_number, int neighbor_port, std::string identifier);

    void run();
};


#endif //TCP_CLIENT_HPP
