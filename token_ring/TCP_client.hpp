//
// Created by Timelock on 11.03.2019.
//

#ifndef TCP_CLIENT_HPP
#define TCP_CLIENT_HPP

#include "TokenRingUtils.hpp"
#include "Token.hpp"
#include <iostream>
#include <netinet/in.h>

class TCP_client {
private:
    SOCKET socket_in;
    SOCKET socket_out;
    SOCKET multicast_socket;
    sockaddr_in neighbor_address;
    int neighbor_port;
    int my_listening_port;
    bool want_to_send;
    bool passflag;
    std::string id;
    std::vector<std::string> all_peers;



    const std::string multicast_ip = "239.0.0.1";
    const int multicast_port = 3000;
    sockaddr_in multicast_address;

    token *token_buffer;

    void initialize_sockets(int port_number, int neighbor_port);
    void initialize_multicast();
    void hello_protocol(bool first_client);
    void send_token();
    void receive_token(SOCKET from);
    int accept_connection();
    void set_neighbor_port(int neighbor_port);
    void process_new_client();
    void process_not_my_msg();
    void process_my_msg();
    void add_peer(std::string peer_id);
    void randomize_need_to_send();
    void send_random_message();
    std::string get_random_destination();
    bool has_peer(std::string peer);
    void log();
public:

    // if neighbor_port == port_number -> this means that essentially we are the first client
    TCP_client(int port_number, int neighbor_port, std::string identifier);

    void run(bool initial_client);
};


#endif //TCP_CLIENT_HPP
